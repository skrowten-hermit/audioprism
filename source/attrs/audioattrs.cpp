/*
 *******************************************************************************
 * 
 * This file is part of AudioPrism Project.
 *
 * AudioPrism is a free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 * 
 *******************************************************************************
 * 
 * File         : source/attrs/audioattrs.cpp.
 * Description  : Functions to extract audio stream attributes/information.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "audioattrs.h"

// AudioAttrs Constructor for initialization.
AudioAttrs::AudioAttrs(esstd::AlgorithmFactory& saf, 
                       std::vector<Real> audioSigVector, 
                       Pool attrPool, std::string description, 
                       int saveData, bool conOut) : saveOutput(saveData), 
                      consoleOut(conOut), AF(saf), signalVector(audioSigVector), 
                      fileTag(description)
{
  //essentia::init();
  
  std::cout << std::endl;
  std::cout << "Audio signal parameter computation from " << fileTag << 
               "...." << std::endl;
  
  SampleSize = signalVector.size();
  SampleRate = attrPool.value<Real>("SampleRate");
  Channels = attrPool.value<Real>("Channels");

  signalAutoCorr = CalcACFSeq();
  signalFreqBandEnergy = GetFreqComponents();
  signalPSD = CalcPSD();
  signalRMS = CalcRMS();
  signalSNR = CalcSNR();
  signalLoudness = CalcLoudness();

  audioAttrs = StoreAttrs();

  if (consoleOut == true)
  {
    projectData();
    printPool();
  }
}

// Get the auto-correlation function (ACF) for a given signal vector input.
std::vector<Real> AudioAttrs::CalcACFSeq()
{
  std::vector<Real> ACF;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Computing auto-correlation function from the signal for " << 
    fileTag << "...." << std::endl;
  }

  acfAlgo = AF.create("AutoCorrelation");
  
  acfAlgo->input("array").set(signalVector);
  acfAlgo->output("autoCorrelation").set(signalAutoCorr);

  acfAlgo->compute();

  sigACFSize = signalAutoCorr.size();

  delete acfAlgo;

  if (consoleOut == true)
    std::cout << "Auto-correlation function computation for " << fileTag << " done...." << 
    std::endl;

  // Return the ACF sequence.

  return ACF;
}

// Get the fundamental frequency (F0) of the audio signal.
Real AudioAttrs::GetF0Freq()
{
  // Return the fundamental frequency F0.
  Real F0 = 0.0;

  return F0;
}

// Get all the frequencies present in the audio signal.
std::vector<Real> AudioAttrs::GetFreqComponents()
{
  std::vector<Real> FFT, FBE;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Extracting the frequency components from " << fileTag << 
    "'s signal...." << std::endl;
  }
  
  fftAlgo = AF.create("Spectrum");
  fbandsAlgo = AF.create("FrequencyBands", "sampleRate", SampleRate);

  fftAlgo->input("frame").set(signalVector);
  fftAlgo->output("spectrum").set(FFT);

  fftAlgo->compute();

  fbandsAlgo->input("spectrum").set(FFT);
  fbandsAlgo->output("bands").set(FBE);

  fbandsAlgo->compute();

  signalFFT = FFT;
  signalFreqBandEnergy = FBE;
  sigFFTSize = signalFFT.size();
  numFreqBands = signalFreqBandEnergy.size();

  delete fftAlgo;
  delete fbandsAlgo;

  if (consoleOut == true)
    std::cout << "Frequency component extraction for " << fileTag << 
    " done...." << std::endl;

  // Return an of frequencies.

  return FBE;
}

// Get the power spectral density (PSD) of the audio signal.
std::vector<Real> AudioAttrs::CalcPSD()
{
  int frameSize = 2048;
  int hopSize = 1024;
  
  std::vector<Real> ACFrame, PSDFrame, PSDvector;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Calculating the power spectral density of " << fileTag << 
    "'s signal...." << std::endl;
  }

  fcAlgo = AF.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
  psdAlgo = AF.create("Spectrum");
  
  fcAlgo->input("signal").set(signalAutoCorr);
  fcAlgo->output("frame").set(ACFrame);

  psdAlgo->input("frame").set(ACFrame);
  psdAlgo->output("spectrum").set(PSDFrame);

  while (true)
  {
    fcAlgo->compute();
    
    if (!ACFrame.size())
    {
      break;
    }
    
    psdAlgo->compute();
    
    PSDvector.insert(PSDvector.end(), PSDFrame.begin(), PSDFrame.end());
  }

  sigPSDSize = PSDvector.size();

  delete fcAlgo;
  delete psdAlgo;

  if (consoleOut == true)
    std::cout << "Power spectral density computation for " << fileTag <<
    " done...." << std::endl;

  // Return the power spectral density.

  return PSDvector;
}

// Calculate the RMS of the audio signal vector.
Real AudioAttrs::CalcRMS()
{
  Real RMS;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Calculating the RMS of " << fileTag << "'s signal...." << 
    std::endl;
  }

  rmsAlgo = AF.create("RMS");
  
  rmsAlgo->input("array").set(signalVector);
  rmsAlgo->output("rms").set(RMS);

  rmsAlgo->compute();

  delete rmsAlgo;

  if (consoleOut == true)
    std::cout << "RMS computation for " << fileTag << " done...." << std::endl;

  // Return the root mean square of the signal.

  return RMS;
}

// Get the SNR of the audio signal.
Real AudioAttrs::CalcSNR()
{
  int frameSize = 2048;
  int hopSize = 1024;
  
  std::vector<Real> SNRFrame, SpectralSNR;
  Real avgSNR, instSNR;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Calculating the SNR from " << fileTag << "'s signal...." << 
    std::endl;
  }

  fcAlgo = AF.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
  snrAlgo = AF.create("SNR", "sampleRate", SampleRate, 
                      "frameSize", frameSize, "noiseThreshold", -30,
                      //"MAAlpha", 0.95,
                      //"MMSEAlpha", 0.98,
                      //"NoiseAlpha", 0.9,
                      "useBroadbadNoiseCorrection", false);

  fcAlgo->input("signal").set(signalVector);
  fcAlgo->output("frame").set(SNRFrame);
  
  snrAlgo->input("frame").set(SNRFrame);
  snrAlgo->output("instantSNR").set(instSNR);
  snrAlgo->output("averagedSNR").set(avgSNR);
  snrAlgo->output("spectralSNR").set(SpectralSNR);

  while (true)
  {
    fcAlgo->compute();

    if (!SNRFrame.size())
    {
      break;
    }
    
    snrAlgo->compute();
  }

  delete fcAlgo;
  delete snrAlgo;

  if (consoleOut == true)
    std::cout << "SNR computation from " << fileTag << " done...." << std::endl;

  // Return the average SNR.

  return avgSNR;
}

// Calculate the Loudness factor of the audio signal.
Real AudioAttrs::CalcLoudness()
{
  Real sigLoudness;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Calculating the loudness factor from " << fileTag << 
    "'s signal...." << std::endl;
  }
  
  loudAlgo = AF.create("Loudness");

  loudAlgo->input("signal").set(signalVector);
  loudAlgo->output("loudness").set(sigLoudness);

  loudAlgo->compute();

  delete loudAlgo;

  if (consoleOut == true)
    std::cout << "Loudness factor computation from " << fileTag << " done...." 
    << std::endl;

  // Return the Loudness metric.

  return sigLoudness;
}

// Store all the attributes in a Pool data structure.
Pool AudioAttrs::StoreAttrs()
{
  Pool attrsPool;
  
  attrsPool.set("Description", fileTag);
  attrsPool.set("SampleSize", SampleSize);
  attrsPool.set("BitRate", BitRate);
  attrsPool.set("SampleRate", SampleRate);
  attrsPool.set("Channels", Channels);
  attrsPool.set("md5sum", md5sum);
  attrsPool.set("Codec", Codec);
  attrsPool.set("RMS", signalRMS);
  attrsPool.set("SNR", signalSNR);
  attrsPool.set("Loudness", signalLoudness);

  return attrsPool;
}

// Store (for file printing) all the attributes in a Pool data structure.
Pool AudioAttrs::StoreAttrs(std::string description, int split)
{
  Pool attrsPool;
  
  attrsPool.set(description + ".Description", fileTag);
  attrsPool.set(description + ".SampleSize", SampleSize);
  attrsPool.set(description + ".BitRate", BitRate);
  attrsPool.set(description + ".SampleRate", SampleRate);
  attrsPool.set(description + ".Channels", Channels);
  attrsPool.set(description + ".md5sum", md5sum);
  attrsPool.set(description + ".Codec", Codec);
  attrsPool.set(description + ".RMS", signalRMS);
  attrsPool.set(description + ".SNR", signalSNR);
  attrsPool.set(description + ".Loudness", signalLoudness);

  if (split == 1)
  {
    std::cout << "-------- writing results to file " << outputFile 
              << " --------" << std::endl;

    Output = AF.create("YamlOutput", "filename", outputFile);
    
    Output->input("pool").set(attrsPool);
    Output->compute();
    
    delete Output;
  }

  return attrsPool;
}

// Print the members of the AudioLoad class to the console.
void AudioAttrs::projectData()
{
  std::vector<Real> v = signalFreqBandEnergy;
  std::cout << std::endl;
  std::cout << "The following are data from " << fileTag << ":" << std::endl;
  std::cout << "Sample Size : " << SampleSize << std::endl;
  std::cout << "Channels : " << Channels << std::endl;
  std::cout << "Sample Rate : " << SampleRate << std::endl;
  std::cout << "Bit Rate : " << BitRate << std::endl;
  std::cout << "md5sum : " << md5sum << std::endl;
  std::cout << "Codec : " << Codec << std::endl;
  std::cout << "RMS of Signal : " << signalRMS << std::endl;
  std::cout << "SNR of Signal : " << signalSNR << std::endl;
  std::cout << "Loudness Factor of Signal : " << signalLoudness << std::endl;
  std::cout << "Size of Signal's FFT : " << sigFFTSize << std::endl;
  std::cout << "Size of Signal's ACF : " << sigACFSize << std::endl;
  std::cout << "Freq. Bands Present in Signal : ";
  std::copy(v.begin(), v.end(), std::ostream_iterator<Real>(std::cout, " "));
  std::cout << std::endl;
  std::cout << "No. of Freq. Bands Present : " << numFreqBands << std::endl;
}

// Display the data from the data structure on the console.
void AudioAttrs::printPool()
{
  std::cout << std::endl;
  std::cout << "The following are data from internal data structure:" << 
               std::endl;
  std::cout << "Sample Size : " << audioAttrs.value<Real>("SampleSize") << 
               std::endl;
  std::cout << "Channels : " << audioAttrs.value<Real>("Channels") << 
               std::endl;
  std::cout << "Sample Rate : " << audioAttrs.value<Real>("SampleRate") << 
               std::endl;
  std::cout << "Bit Rate : " << audioAttrs.value<Real>("BitRate") << std::endl;
  std::cout << "md5sum : " << audioAttrs.value<std::string>("md5sum") << 
               std::endl;
  std::cout << "Codec : " << audioAttrs.value<std::string>("Codec") << 
               std::endl;
  std::cout << "RMS of Signal : " << audioAttrs.value<Real>("RMS") << 
               std::endl;
  std::cout << "SNR of Signal : " << audioAttrs.value<Real>("SNR") << 
               std::endl;
  std::cout << "Loudness Factor of Signal : " << 
               audioAttrs.value<Real>("Loudness") << std::endl;
}

AudioAttrs::~AudioAttrs()
{
  //
}
