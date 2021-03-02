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
}

// Get the auto-correlation function (ACF) for a given signal vector input.
std::vector<Real> AudioAttrs::CalcACFSeq()
{
  std::vector<Real> ACF;

  acfAlgo = AF.create("AutoCorrelation");
  
  acfAlgo->input("array").set(signalVector);
  acfAlgo->output("autoCorrelation").set(signalAutoCorr);

  acfAlgo->compute();

  delete acfAlgo;

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
  
  fftAlgo = AF.create("Spectrum");
  fbandsAlgo = AF.create("FrequencyBands", "sampleRate", SampleRate);

  fftAlgo->input("frame").set(signalVector);
  fftAlgo->output("spectrum").set(FFT);

  fftAlgo->compute();

  fbandsAlgo->input("spectrum").set(FFT);
  fbandsAlgo->output("bands").set(FBE);

  fbandsAlgo->compute();

  delete fftAlgo;
  delete fbandsAlgo;

  // Return an of frequencies.

  return FBE;
}

// Get the gain factor of the audio signal.
std::vector<Real> AudioAttrs::CalcPSD()
{
  int frameSize = 2048;
  int hopSize = 1024;
  
  std::vector<Real> ACFrame, PSDFrame, PSDvector;

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

  delete fcAlgo;
  delete psdAlgo;

  // Return the power spectral density.

  return PSDvector;
}

// Get the gain of the audio signal in dB.
Real AudioAttrs::CalcRMS()
{
  Real RMS;

  rmsAlgo = AF.create("RMS");
  
  rmsAlgo->input("array").set(signalVector);
  rmsAlgo->output("rms").set(RMS);

  rmsAlgo->compute();

  delete rmsAlgo;

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

  // Return the average SNR.

  return avgSNR;
}

// Get the SNR of the audio signal.
Real AudioAttrs::CalcLoudness()
{
  Real sigLoudness;
  
  loudAlgo = AF.create("Loudness");

  loudAlgo->input("signal").set(signalVector);
  loudAlgo->output("loudness").set(sigLoudness);

  loudAlgo->compute();

  delete loudAlgo;

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

AudioAttrs::~AudioAttrs()
{
  //
}
