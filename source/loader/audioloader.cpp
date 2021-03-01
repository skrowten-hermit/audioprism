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
 * File         : source/loader/audioloader.cpp.
 * Description  : Functions to load and extract audio streams from a audio file.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "audioloader.h"

// AudioLoad Constructor for initialization.
AudioLoad::AudioLoad(esstd::AlgorithmFactory& saf, std::string inFile, 
                     std::string outFile, std::string description, int saveData)
                    : audioFile(inFile), outputFile(outFile), 
                    saveOutput(saveData), AF(saf), fileTag(description)
{ 
  //essentia::init();

  AudioBuffer = GetAudioData();
  MonoBuffer = GetMonoData();

  loadPool = StoreAudio();
}

// Get the audio signal data and details from a given audio file.
std::vector<StereoSample> AudioLoad::GetAudioData()
{
  Real srate;
  int chnls;
  int brate;
  std::string md5s;
  std::string codec; 
  
  Loader =  AF.create("AudioLoader", "filename", audioFile, "computeMD5", true);
  
  Loader->output("audio").set(Buffer);
  Loader->output("sampleRate").set(srate);
  Loader->output("numberChannels").set(chnls);
  Loader->output("md5").set(md5s);
  Loader->output("codec").set(codec);
  Loader->output("bit_rate").set(brate);

  Loader->compute();

  delete Loader;

  return Buffer;
}

// Get a mono audio signal data from a given audio file - channel source can be 
// chosen (using MonoMixer).
std::vector<Real> AudioLoad::GetMonoData()
{
  std::vector<Real> MonoStream;

  esstd::Algorithm* MonoData;
  MonoData = AF.create("MonoMixer", "type", "left");

  MonoData->input("audio").set(AudioBuffer);
  MonoData->input("numberChannels").set(Channels);
  MonoData->output("audio").set(MonoStream);

  MonoData->compute();

  delete MonoData;

  return MonoStream;
}

/* // Get the left channel of a stereo audio signal from a given audio file.
std::vector<Real> AudioLoad::GetLeftChannel()
{
  for(int i = 0; 
      i < audioData.value<std::vector<StereoSample>>("audio").size(); ++i)
  {
    audioData.value<std::vector<StereoSample>>("audio")[i].left() >> 
    PC(audioData, "LeftChannel");
  }
}

// Get the right channel of a stereo audio signal from a given audio file.
std::vector<Real> AudioLoad::GetRightChannel()
{
  for(int i = 0; 
      i < audioData.value<std::vector<StereoSample>>("audio").size(); ++i)
  {
    audioData.value<std::vector<StereoSample>>("audio")[i].right() >> 
    PC(audioData, "RightChannel");
  }
} */

// Store all the audio data in a Pool data structure.
Pool AudioLoad::StoreAudio()
{
  Pool aPool;

  // Pool function add()/set() don't support a vector of SteroSample type.
  // audioData.set("Audio", AudioBuffer);
  audioData.set("MonoAudio", MonoBuffer);

  audioInfo.set("Channels", Channels);
  audioInfo.set("md5sum", md5sum);
  audioInfo.set("SampleRate", SampleRate);
  audioInfo.set("BitRate", BitRate);
  audioInfo.set("Codec", Codec);

  aPool.merge(audioInfo);
  aPool.merge(audioData);

  return aPool;
}

// Store (for file printing) all the audio data in a Pool data structure.
Pool AudioLoad::StoreAudio(std::string description, int split)
{
  Pool aPool;

  // Pool function add()/set() don't support a vector of SteroSample type.
  // audioData.set(fileTag + ".Audio", AudioBuffer); 
  audioData.set(fileTag + ".MonoAudio", MonoBuffer);

  audioInfo.set(fileTag + ".Channels", Channels);
  audioInfo.set(fileTag + ".md5sum", md5sum);
  audioInfo.set(fileTag + ".SampleRate", SampleRate);
  audioInfo.set(fileTag + ".BitRate", BitRate);
  audioInfo.set(fileTag + ".Codec", Codec);

  aPool.merge(audioInfo);
  aPool.merge(audioData);

  if (split == 1)
  {
    std::cout << "-------- writing results to file " << outputFile 
              << " --------" << std::endl;
    
    Output = AF.create("YamlOutput", "filename", outputFile);
    Output->input("pool").set(audioData);
    Output->input("pool").set(audioInfo);
    Output->compute();

    delete Output;
  }

  return aPool;
}

// AudioLoad Destructor for closure.
AudioLoad::~AudioLoad()
{
  //essentia::shutdown();
}
