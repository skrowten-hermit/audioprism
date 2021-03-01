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
  vector<Real> FreqBands {0, 50, 100, 150, 200, 300, 400, 510, 630, 770, 920, 1080,
                          1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 
                          6400, 7700, 9500, 12000, 15500, 20500, 27000};
  vector<Real> frame, ACFrame;
  vector<Real> FreqBandEnergy, signalPSD, signalSNRspectral;
  vector<Real> acorrVectorSource;
  vector<Real> fftSource, PSDsource, PSDvector, PSvector;
  Real signalRMS, signalSNRinst, signalSNRavg, signalLoudness;

  Algorithm* fcA = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
  /*Algorithm* win = factory.create("Windowing",
                                  "type", "blackmanharris62");*/
  Algorithm* acorrSource = factory.create("AutoCorrelation",//);
                                        "normalization", "standard");
                                        //"generalized", true);
  Algorithm* psd = factory.create("Spectrum");
  Algorithm* ps = factory.create("PowerSpectrum");
  Algorithm* fft = factory.create("Spectrum");
  Algorithm* fbands = factory.create("FrequencyBands",
                                     "sampleRate", sourceSampleRate);
  Algorithm* rms = factory.create("RMS");
  Algorithm* snr = factory.create("SNR",
                                  "sampleRate", sourceSampleRate,
                                  "frameSize", frameSize,
				  "noiseThreshold", -30,
                                  //"MAAlpha", 0.95,
                                  //"MMSEAlpha", 0.98,
                                  //"NoiseAlpha", 0.9,
                                  "useBroadbadNoiseCorrection", false);
  Algorithm* loudness = factory.create("Loudness");
  
  acorrSource->input("array").set(monoVectorSource);
  acorrSource->output("autoCorrelation").set(acorrVectorSource);
  
  acorrSource->compute();

  pool.add("autocorrelation.Source", acorrVectorSource);

  fft->input("frame").set(monoVectorSource);
  fft->output("spectrum").set(fftSource);
  
  fft->compute();

  fbands->input("spectrum").set(fftSource);
  fbands->output("bands").set(FreqBandEnergy);

  fbands->compute();

  pool.add("freqbandenergies.Source", FreqBandEnergy);

  rms->input("array").set(monoVectorSource);
  rms->output("rms").set(signalRMS);

  rms->compute();

  pool.add("rms.Source", signalRMS);

  fcA->input("signal").set(monoVectorSource);
  fcA->output("frame").set(frame);
  /*win->input("frame").set(frame);
  win->output("frame").set(windowedFrame);*/

  /*snr->input("frame").set(monoVectorSource);
  snr->output("instantSNR").set(signalSNRinst);
  snr->output("averagedSNR").set(signalSNRavg);
  snr->output("spectralSNR").set(signalSNRspectral);

  snr->compute();

  pool.add("instantSNR.Source", signalSNRinst);
  pool.add("averagedSNR.Source", signalSNRavg);
  pool.add("spectralSNR.Source", signalSNRspectral);*/
  
  snr->input("frame").set(frame);
  snr->output("instantSNR").set(signalSNRinst);
  snr->output("averagedSNR").set(signalSNRavg);
  snr->output("spectralSNR").set(signalSNRspectral);

  while (true)
  {
    fcA->compute();

    if (!frame.size())
    {
      break;
    }
    
    //win->compute();
    snr->compute();

    pool.add("instantSNR.Source", signalSNRinst);
    pool.add("averagedSNR.Source", signalSNRavg);
    pool.add("spectralSNR.Source", signalSNRspectral);
  }

  loudness->input("signal").set(monoVectorSource);
  loudness->output("loudness").set(signalLoudness);

  loudness->compute();

  pool.add("loudness.Source", signalLoudness);

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

  ps->input("signal").set(monoVectorSource);
  ps->output("powerSpectrum").set(PSvector);

  ps->compute();

  pool.add("PowerSpectrum.vector", PSvector);

  cout << "Size of mono audio data of source : " << sampleSize << endl;
  cout << "Size of ACF sequence of source : " << acorrVectorSource.size() << endl;
  cout << "Size of PSD sequence of source (by frames) : " << PSDsource.size() << endl;
  cout << "Size of PSD sequence of source (full vector) : " << PSDvector.size() << endl;
  cout << "Size of PowerSpectrum of source : " << PSvector.size() << endl;
  //cout << "FFT output : " << fftSource << endl;
  cout << "Frequency Bands : \n" << FreqBands << endl;
  cout << "No. of frequencies listed in the band : " << FreqBands.size() << endl;
  cout << "Energy of Frequency Bands : \n" << FreqBandEnergy << endl;
  cout << "Size of Frequency Band energies computed : " << FreqBandEnergy.size() << endl;
  cout << "RMS Value : " << signalRMS << endl;
  cout << "SNR (instant) : " << signalSNRinst << endl;
  cout << "SNR (average) : " << signalSNRavg << endl;
  //cout << "SNR (spectral) : " << signalSNRspectral << endl;
  cout << "Loudness:\n" << signalLoudness << endl;
  
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
