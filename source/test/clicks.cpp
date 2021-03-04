#include <iostream>
#include <cmath>
#include <complex>
#include <algorithm>
#include "../credits/libav.h"
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
  
  vector<Real> CDframe, DDframe, GDframe, HDframe, NDframe;
  vector<Real> clickStartvec, clickEndvec;
  vector<Real> clickStarts, clickEnds;

  Algorithm* fcC = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
 
  Algorithm* cdProb = factory.create("ClickDetector",
                                     //"detectionThreshold", 50,
                                     "frameSize", frameSize,
                                     "hopSize", hopSize,
                                     "sampleRate", sourceSampleRate);

  fcC->input("signal").set(monoVectorSource);
  fcC->output("frame").set(CDframe);
  
  cdProb->input("frame").set(CDframe); 
  cdProb->output("starts").set(clickStartvec);
  cdProb->output("ends").set(clickEndvec);

  while (true)
  {
    fcC->compute();

    if (!CDframe.size())
    {
      break;
    }
    
    //win->compute();
    cdProb->compute();
    
    clickStarts.insert(clickStarts.end(), clickStartvec.begin(), clickStartvec.end());
    clickEnds.insert(clickEnds.end(), clickEndvec.begin(), clickEndvec.end());
  }

  pool.add("ClickDetector.Start", clickStarts);
  pool.add("ClickDetector.End", clickEnds);
  
  cout << "Size of mono audio data of source : " << sampleSize << endl;
  cout << "Size of ClickStarts vector : " << clickStarts.size() << endl;
  cout << "Size of ClickEnds vector : " << clickEnds.size() << endl;
  
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
