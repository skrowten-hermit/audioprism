#include <iostream>
#include <cmath>
#include <complex>
#include <algorithm>
#include "credits/libav.h"
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include "standard/monomixer.h"
#include "standard/crosscorrelation.h"
#include "stats/variance.h"
#include <essentia/scheduler/network.h>

#define DEFAULT_INPUT "../../lib/essentia/test/audio/recorded/cat_purrrr.wav"
#define DEFAULT_OUTPUT "results.yaml"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

int main(int argc, char* argv[])
{
  // Initialize I/O variables.
  string audioFileSource = DEFAULT_INPUT;
  string outputFileName = "results.yaml";
  //int outfile = 0;
  
  if (argc < 2)
  {
    cout << "Error: incorrect number of arguments." << endl;
    cout << "Usage: " << argv[0] << " audio_input yaml_output[OPTIONAL]" << endl;
    creditLibAV();
    cout << "Using the default file : " << audioFileSource << endl;
  }
  else if (argc == 3)
  {
    audioFileSource = argv[1];
    outputFileName = argv[2];
  }
  else
  {
    audioFileSource = argv[1];
  }
  
  // Initialize audio data variables.
  vector<Real> monoVectorSource;
  vector<StereoSample> audioBufferSource;
  Real sourceSampleRate;
  int sourceChannels;
  int sourceBitRate;
  string sourcemd5sum;
  string sourceCodec;
  
  Pool pool;

  // Register the algorithm in the factory(ies).
  essentia::init();

  AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
  Algorithm* sourceLoader =  factory.create("AudioLoader",
                                            "filename", audioFileSource,
                                            "computeMD5", true);
  Algorithm* sourceMono = factory.create("MonoMixer",
                                         "type", "left");

  sourceLoader->output("audio").set(audioBufferSource);
  sourceLoader->output("sampleRate").set(sourceSampleRate);
  sourceLoader->output("numberChannels").set(sourceChannels);
  sourceLoader->output("md5").set(sourcemd5sum);
  sourceLoader->output("codec").set(sourceCodec);
  sourceLoader->output("bit_rate").set(sourceBitRate);
  
  sourceLoader->compute();
  
  //pool.add("source.Data", audioBufferSource);
  pool.add("source.Channels", sourceChannels);
  pool.add("source.md5sum", sourcemd5sum);
  pool.add("source.SampleRate", sourceSampleRate);
  pool.add("source.BitRate", sourceBitRate);
  pool.add("source.Codec", sourceCodec);

  sourceMono->input("audio").set(audioBufferSource);
  sourceMono->input("numberChannels").set(sourceChannels);
  sourceMono->output("audio").set(monoVectorSource);
  
  sourceMono->compute();

  pool.add("source.MonoData", monoVectorSource);

  int sampleSize;
  int frameSize = 2048;
  int hopSize = 1024;
  sampleSize = monoVectorSource.size();
  
  vector<Real> GDframe;
  vector<Real> gapStartvec, gapEndvec;
  vector<Real> gapStarts, gapEnds, gapLengths;

  Algorithm* fcG = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
 
  Algorithm* gdProb = factory.create("GapsDetector",
                                     "frameSize", frameSize,
                                     "hopSize", hopSize,
                                     "sampleRate", sourceSampleRate);

  fcG->input("signal").set(monoVectorSource);
  fcG->output("frame").set(GDframe);
  
  gdProb->input("frame").set(GDframe); 
  gdProb->output("starts").set(gapStartvec);
  gdProb->output("ends").set(gapEndvec);

  while (true)
  {
    fcG->compute();

    if (!GDframe.size())
    {
      break;
    }
    
    //win->compute();
    gdProb->compute();

    gapStarts.insert(gapStarts.end(), gapStartvec.begin(), gapStartvec.end());
    gapEnds.insert(gapEnds.end(), gapEndvec.begin(), gapEndvec.end());
  }

  pool.add("GapsDetector.Start", gapStarts);
  pool.add("GapsDetector.End", gapEnds);

  cout << "Size of mono audio data of source : " << sampleSize << endl;
  cout << "Size of GapStarts vector : " << gapStarts.size() << endl;
  cout << "Size of GapEnds vector : " << gapEnds.size() << endl;
  
  // write results to file
  cout << "-------- writing results to file " << outputFileName << " --------" << endl;
    
  standard::Algorithm* output = standard::AlgorithmFactory::create("YamlOutput",
                                                                   "filename", outputFileName);
    
  output->input("pool").set(pool);
  output->compute();
    
  delete output;

  essentia::shutdown();

  return 0;
}
