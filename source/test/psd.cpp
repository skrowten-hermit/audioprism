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
  }
  else
  {
    audioFileSource = argv[1];
    outputFileName = argv[2];
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
  vector<Real> frame, ACFrame;
  vector<Real> signalPSD;
  vector<Real> acorrVectorSource;
  vector<Real> PSDsource, PSDvector;

  Algorithm* acorrSource = factory.create("AutoCorrelation",//);
                                        "normalization", "standard");
                                        //"generalized", true);
  Algorithm* psd = factory.create("Spectrum");
  
  acorrSource->input("array").set(monoVectorSource);
  acorrSource->output("autoCorrelation").set(acorrVectorSource);
  
  acorrSource->compute();

  pool.add("autocorrelation.Source", acorrVectorSource);

  Algorithm* fcC = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
  
  acorrSource->input("array").set(monoVectorSource);
  acorrSource->output("autoCorrelation").set(acorrVectorSource);

  acorrSource->compute();

  fcC->input("signal").set(acorrVectorSource);
  fcC->output("frame").set(ACFrame);

  psd->input("frame").set(ACFrame);
  psd->output("spectrum").set(PSDsource);

  //int i = 1;

  while (true)
  {
    fcC->compute();
    //cout << "Frame : " << i << endl;
    
    if (!ACFrame.size())
    {
      break;
    }
    
    psd->compute();
    
    PSDvector.insert(PSDvector.end(), PSDsource.begin(), PSDsource.end());
    pool.add("PSD.frames", PSDsource);
    //i++;
  }

  pool.add("PSD.vector", PSDvector);

  cout << "Size of mono audio data of source : " << sampleSize << endl;
  cout << "Size of ACF sequence of source : " << acorrVectorSource.size() << endl;
  cout << "Size of PSD sequence of source (by frames) : " << PSDsource.size() << endl;
  cout << "Size of PSD sequence of source (full vector) : " << PSDvector.size() << endl;
  
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
