#include <iostream>
#include <cmath>
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
  string audioFileSink = DEFAULT_INPUT;
  string outputFileName = "results.yaml";
  //int outfile = 0;

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
    audioFileSource = argv[1];
    audioFileSink = argv[2];
    outputFileName = argv[3];
  }
  
  // Initialize audio data variables.
  vector<Real> monoVectorSource, monoVectorSink;
  vector<StereoSample> audioBufferSource, audioBufferSink;
  Real sourceSampleRate, sinkSampleRate;
  int sourceChannels, sinkChannels;
  int sourceBitRate, sinkBitRate;
  string sourcemd5sum, sinkmd5sum;
  string sourceCodec, sinkCodec;
  
  Pool pool;

  // Register the algorithm in the factory(ies).
  essentia::init();

  AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
  Algorithm* sourceLoader =  factory.create("AudioLoader",
                                            "filename", audioFileSource,
                                            "computeMD5", true);
  Algorithm* sinkLoader =  factory.create("AudioLoader",
                                          "filename", audioFileSink,
					  "computeMD5", true);
  Algorithm* sourceMono = factory.create("MonoMixer",
                                         "type", "left");
  Algorithm* sinkMono = factory.create("MonoMixer",
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

  sinkLoader->output("audio").set(audioBufferSink);
  sinkLoader->output("sampleRate").set(sinkSampleRate);
  sinkLoader->output("numberChannels").set(sinkChannels);
  sinkLoader->output("md5").set(sinkmd5sum);
  sinkLoader->output("codec").set(sinkCodec);
  sinkLoader->output("bit_rate").set(sinkBitRate);
  
  sinkLoader->compute();
  
  //pool.add("sink.Data", audioBufferSink);
  pool.add("sink.Channels", sinkChannels);
  pool.add("sink.md5sum", sinkmd5sum);
  pool.add("sink.SampleRate", sinkSampleRate);
  pool.add("sink.BitRate", sinkBitRate);
  pool.add("sink.Codec", sinkCodec);

  sourceMono->input("audio").set(audioBufferSource);
  sourceMono->input("numberChannels").set(sourceChannels);
  sourceMono->output("audio").set(monoVectorSource);
  
  sourceMono->compute();

  pool.add("source.MonoData", monoVectorSource);

  sinkMono->input("audio").set(audioBufferSink);
  sinkMono->input("numberChannels").set(sinkChannels);
  sinkMono->output("audio").set(monoVectorSink);
  
  sinkMono->compute();
  
  pool.add("sink.MonoData", monoVectorSink);

  int sampleSize, sinkSize, xCorrSize;
  sampleSize = monoVectorSource.size();
  xCorrSize = sampleSize;
  sinkSize = monoVectorSink.size();
  Real sourceVar, sinkVar, sourceStdDev, sinkStdDev, xcorrNormFactor;
  Real corrVectorPeak, acorrSourcePeak, acorrSinkPeak;
  vector<Real> corrVector, acorrVectorSource, acorrVectorSink;

  cout << "Size of mono audio data of source : " << monoVectorSource.size() << endl;
  cout << "Size of mono audio data of sink : " << monoVectorSink.size() << endl;

  if (sinkSize > sampleSize)
  {
    monoVectorSource.resize(monoVectorSource.size() + (sinkSize - sampleSize), 0);
    xCorrSize = sinkSize;
  }
  else if (sinkSize < sampleSize)
  {
    monoVectorSink.resize(monoVectorSink.size() + (sampleSize - sinkSize), 0);
  }

  Algorithm* acorrSource = factory.create("AutoCorrelation",//);
                                          "normalization", "standard");
                                          //"generalized", true);
  Algorithm* acorrSink = factory.create("AutoCorrelation",//);
                                        "normalization", "standard");
                                        //"generalized", true);
  
  acorrSource->input("array").set(monoVectorSource);
  acorrSource->output("autoCorrelation").set(acorrVectorSource);
  
  acorrSource->compute();

  pool.add("autocorrelation.Source", acorrVectorSource);

  acorrSink->input("array").set(monoVectorSink);
  acorrSink->output("autoCorrelation").set(acorrVectorSink);

  acorrSink->compute();

  pool.add("autocorrelation.Sink", acorrVectorSink);

  Algorithm* xcorr = factory.create("CrossCorrelation",
                                    "maxLag", xCorrSize,
				    "minLag", -xCorrSize);
  Algorithm* sourceVectorVar = factory.create("Variance");
  Algorithm* sinkVectorVar = factory.create("Variance");
  
  xcorr->input("arrayX").set(monoVectorSource);
  xcorr->input("arrayY").set(monoVectorSink);
  xcorr->output("crossCorrelation").set(corrVector);

  xcorr->compute();

  sourceVectorVar->input("array").set(monoVectorSource);
  sourceVectorVar->output("variance").set(sourceVar);
  sinkVectorVar->input("array").set(monoVectorSink);
  sinkVectorVar->output("variance").set(sinkVar);

  sourceVectorVar->compute();
  sinkVectorVar->compute();

  sourceStdDev = sqrt(sourceVar);
  sinkStdDev = sqrt(sinkVar);
  xcorrNormFactor = 1 / (sampleSize * sourceStdDev * sinkStdDev);
  
  pool.add("crosscorrelation.Data", corrVector);
  pool.add("crosscorrelation.SourceVectorVariance", sourceVar);
  pool.add("crosscorrelation.SourceStdDev", sourceStdDev);
  pool.add("crosscorrelation.SinkVectorVariance", sinkVar);
  pool.add("crosscorrelation.SinkStdDev", sinkStdDev);
  pool.add("crosscorrelation.NormFactor", xcorrNormFactor);
  
  transform(corrVector.begin(), corrVector.end(), corrVector.begin(), [xcorrNormFactor](Real &c){ return c * xcorrNormFactor; });
  corrVectorPeak = *max_element(std::begin(corrVector), std::end(corrVector));

  acorrSourcePeak = *max_element(std::begin(acorrVectorSource), std::end(acorrVectorSource));
  acorrSinkPeak = *max_element(std::begin(acorrVectorSink), std::end(acorrVectorSink));

  pool.add("crosscorrelation.Normalized", corrVector);
  pool.add("crosscorrelation.PeakValue", corrVectorPeak);

  //cout << "Mono Audio Data:\n" << monoVector << endl;
  cout << "Size of mono audio data of source : " << monoVectorSource.size() << endl;
  cout << "Size of mono audio data of sink : " << monoVectorSink.size() << endl;
  //cout << "Cross Correlation:\n" << corrPool.value<vector<vector<Real>>>("crosscorr") << endl;
  //cout << "Cross Correlation:\n" << corrVector << endl;
  cout << "Size of Cross Correlation Vector : " << corrVector.size() << endl;
  cout << "Source Auto-Correlation Peak : " << acorrSourcePeak << endl;
  cout << "Sink Auto-Correlation Peak : " << acorrSinkPeak << endl;
  cout << "Cross-Correlation Peak : " << corrVectorPeak << endl;
  
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
