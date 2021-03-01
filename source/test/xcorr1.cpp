#include <iostream>
#include "credits/libav.h"
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include "standard/monomixer.h"
#include "standard/crosscorrelation.h"
#include <essentia/scheduler/network.h>

#define DEFAULT_INPUT "../../lib/essentia/test/audio/recorded/cat_purrrr.wav"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

int main(int argc, char* argv[])
{
  string audioFileSource = DEFAULT_INPUT;
  string audioFileSink = DEFAULT_INPUT;
  string outputFileName = "results.yaml";
  int outfile = 0;
  
  if (argc < 3)
  {
    cout << "Error: incorrect number of arguments." << endl;
    cout << "Usage: " << argv[0] << " audio_input1 audio_input2 yaml_output[OPTIONAL]" << endl;
    creditLibAV();
    cout << "Using the default file(s) : " << audioFileSource << " " << audioFileSink << endl;
  }
  else if (argc == 3)
  {
    audioFileSource = argv[1];
    audioFileSink = argv[2];
  }
  else
  {
    outfile = 1;
    audioFileSource = argv[1];
    audioFileSink = argv[2];
    outputFileName = argv[3];
  }

  // Register the algorithm in the factory(ies).
  essentia::init();

  Pool stereoPool;
  Pool monoPool;
  Pool corrPool;

  vector<Real> monoVector, corrVector;
  vector<StereoSample> audioBuffer;
  Real samplerate;
  int noofchannels, bitrate;
  string md5sum, codec;

  AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
  Algorithm* loader =  factory.create("AudioLoader",
                                      "filename", audioFileSource,
                                      "computeMD5", true);
  Algorithm* mono = factory.create("MonoMixer",
                                   "type", "left");
 
  loader->output("audio").set(audioBuffer);
  loader->output("sampleRate").set(samplerate);
  loader->output("numberChannels").set(noofchannels);
  loader->output("md5").set(md5sum);
  loader->output("codec").set(codec);
  loader->output("bit_rate").set(bitrate);

  loader->compute();
  
  mono->input("audio").set(audioBuffer);
  mono->input("numberChannels").set(noofchannels);
  mono->output("audio").set(monoVector);

  mono->compute();

  int samplesize;
  samplesize = monoVector.size();
  
  Algorithm* xcorr = factory.create("CrossCorrelation",
                                    "maxLag", samplesize);
  
  xcorr->input("arrayX").set(monoVector);
  xcorr->input("arrayY").set(monoVector);
  xcorr->output("crossCorrelation").set(corrVector);

  xcorr->compute();

  //cout << "Mono Audio Data:\n" << monoVector << endl;
  cout << "Size of mono audio data : " << monoVector.size() << endl;
  //cout << "Cross Correlation:\n" << corrPool.value<vector<vector<Real>>>("crosscorr") << endl;
  //cout << "Cross Correlation:\n" << corrVector << endl;
  cout << "Size of Cross Correlation Vector : " << corrVector.size() << endl;
  
  // write results to file
  if (outfile == 1)
  {
    cout << "-------- writing results to file " << outputFileName << " --------" << endl;
    
    standard::Algorithm* output = standard::AlgorithmFactory::create("YamlOutput",
		                                                     "filename", outputFileName);
    
    output->input("pool").set(stereoPool);
    output->compute();
    
    delete output;
  }

  essentia::shutdown();

  return 0;
}
