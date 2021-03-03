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
                     std::string outFile, std::string description, int saveData,
                     bool conOut) : audioFile(inFile), outputFile(outFile), 
                    saveOutput(saveData), consoleOut(conOut), AF(saf), 
                    fileTag(description)
{ 
  //essentia::init();
  
  std::cout << std::endl;
  std::cout << "Loading Audio file " << audioFile << "...." << std::endl;

  AudioBuffer = GetAudioData();
  MonoBuffer = GetMonoData();

  loadPool = StoreAudio();

  if (consoleOut == true)
  {
    projectData();
    printPool();
  }
}

// Get the audio signal data and details from a given audio file.
std::vector<StereoSample> AudioLoad::GetAudioData()
{
  std::vector<StereoSample> Buffer;
  Real srate;
  int chnls;
  int brate;
  std::string md5s;
  std::string codec; 

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Reading Audio file " << audioFile << "...." << std::endl;
  }
  
  Loader =  AF.create("AudioLoader", "filename", audioFile, "computeMD5", true);
  
  Loader->output("audio").set(Buffer);
  Loader->output("sampleRate").set(srate);
  Loader->output("numberChannels").set(chnls);
  Loader->output("md5").set(md5s);
  Loader->output("codec").set(codec);
  Loader->output("bit_rate").set(brate);

  Loader->compute();

  Channels = (Real) chnls;
  SampleRate = srate;
  BitRate = (Real) brate;
  md5sum = md5s;
  Codec = codec;
  BSize = Buffer.size();

  delete Loader;

  if (consoleOut == true)
    std::cout << audioFile << " read done...." << std::endl;

  return Buffer;
}

// Get a mono audio signal data from a given audio file - channel source can be 
// chosen (using MonoMixer).
std::vector<Real> AudioLoad::GetMonoData()
{
  std::vector<Real> MonoStream;
  int chnls;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Extracting Audio data from " << fileTag << "...." << 
                 std::endl;
  }

  MonoData = AF.create("MonoMixer", "type", "left");

  MonoData->input("audio").set(AudioBuffer);
  MonoData->input("numberChannels").set(chnls);
  MonoData->output("audio").set(MonoStream);

  MonoData->compute();

  MBSize = MonoStream.size();

  delete MonoData;

  if (consoleOut == true)
    std::cout << "Audio data extracted from " << fileTag << " done...." << 
                 std::endl;

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

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Storing Audio data from " << audioFile << " internally...." 
              << std::endl;
  }

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

  if (consoleOut == true)
    std::cout << "Audio data from " << fileTag << " stored internally...." << 
                 std::endl;

  return aPool;
}

// Store (for file printing) all the audio data in a Pool data structure.
Pool AudioLoad::StoreAudio(std::string description, int split)
{
  Pool aPool;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Storing Audio data from " << audioFile << " externally...." 
              << std::endl;
  }

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

  if (consoleOut == true)
    std::cout << "Audio data from " << fileTag << " stored externally...." << 
                 std::endl;

  return aPool;
}

// Print the members of the AudioLoad class to the console.
void AudioLoad::projectData()
{
  std::cout << std::endl;
  std::cout << "The following are data from " << fileTag << ":" << std::endl;
  std::cout << "Channels : " << Channels << std::endl;
  std::cout << "Sample Rate : " << SampleRate << std::endl;
  std::cout << "Bit Rate : " << BitRate << std::endl;
  std::cout << "md5sum : " << md5sum << std::endl;
  std::cout << "Codec : " << Codec << std::endl;
  std::cout << "Buffer Size : " << BSize << std::endl;
  std::cout << "Signal Size : " << MBSize << std::endl;
}

// Display the data from the data structure on the console.
void AudioLoad::printPool()
{
  std::cout << std::endl;
  std::cout << "The following are data from internal data structure:" << std::endl;
  std::cout << "Channels : " << loadPool.value<Real>("Channels") << std::endl;
  std::cout << "Sample Rate : " << loadPool.value<Real>("SampleRate") << std::endl;
  std::cout << "Bit Rate : " << loadPool.value<Real>("BitRate") << std::endl;
  std::cout << "md5sum : " << loadPool.value<std::string>("md5sum") << std::endl;
  std::cout << "Codec : " << loadPool.value<std::string>("Codec") << std::endl;
}

// AudioLoad Destructor for closure.
AudioLoad::~AudioLoad()
{
  //essentia::shutdown();
}
