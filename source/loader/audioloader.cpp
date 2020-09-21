/*
 * Copyright (C) 2006-2016  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
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
 */

#include <iostream>
#include "audioloader.h"

// AudioLoad Constructor for initialization.
AudioLoad::AudioLoad(string inFile, string outFile)
{
  audioFile = inFile;
  
  if (outFile != "")
  {
    saveOutput = 1;
  }
  
  outputFile = outFile;
  
  essentia::init();
}

// Get the audio signal data and details from a given audio file.
void AudioLoad::GetAudioData(string inFile)
{
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Algorithm* loader =  factory.create("AudioLoader",
		                      "filename", audioFileName,
				      "computeMD5", true);
  
  loader->output("audio") >> PC(stereoPool, "audio");
  loader->output("sampleRate") >> PC(stereoPool, "samplerate");
  loader->output("numberChannels") >> PC(stereoPool, "channels");
  loader->output("md5") >> PC(stereoPool, "md5");
  loader->output("codec") >> PC(stereoPool, "codec");
  loader->output("bit_rate") >> PC(stereoPool, "bit_rate");

  Network n(loader);
  n.run();
}

// Get a stereo audio signal data from a given audio file.
void AudioLoad::GetStereoChannel(string inFile)
{
  GetAudioData(inFile);
}

// Get a mono audio signal data from a given audio file - channel source can be chosen(using MonoMixer).
void AudioLoad::GetMonoChannel(string inFile, string source)
{
  GetStereoChannel(inFile);
}

// Get the left channel of a stereo audio signal from a given audio file.
void AudioLoad::GetLeftChannel(string inFile)
{
  GetStereoChannel(inFile);
  
  for(int i = 0; i < stereoPool.value<vector<StereoSample>>("audio").size(); ++i) 
  {
    stereoPool.value<vector<StereoSample>>("audio")[i].left() >> PC(leftPool, "leftstream");
  }
}

// Get the right channel of a stereo audio signal from a given audio file.
void AudioLoad::GetRightChannel(string inFile)
{
  GetStereoChannel(inFile);

  for(int i = 0; i < stereoPool.value<vector<StereoSample>>("audio").size(); ++i)
  {
    stereoPool.value<vector<StereoSample>>("audio")[i].right() >> PC(rightPool, "rightstream");
  }
}

int main(int argc, char* argv[])
{
  string audioFileName = DEFAULT_INPUT;
  string outputFileName = "results.yaml";
  int outfile = 0;
  
  if (argc < 2)
  {
    cout << "Error: incorrect number of arguments." << endl;
    cout << "Usage: " << argv[0] << " audio_input yaml_output[OPTIONAL]" << endl;
    creditLibAV();
    cout << "Using the default file : " << audioFileName << endl;
  }
  else if (argc == 2)
  {
    audioFileName = argv[1];
  }
  else
  {
    outfile = 1;
    audioFileName = argv[1];
    outputFileName = argv[2];
  }

  // Register the algorithm in the factory(ies).
  essentia::init();

  Pool stereoPool;
  Pool leftPool;
  Pool rightPool;

  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Algorithm* loader =  factory.create("AudioLoader",
		                      "filename", audioFileName,
				      "computeMD5", true);
  
  loader->output("audio") >> PC(stereoPool, "audio");
  loader->output("sampleRate") >> PC(stereoPool, "samplerate");
  loader->output("numberChannels") >> PC(stereoPool, "channels");
  loader->output("md5") >> PC(stereoPool, "md5");
  loader->output("codec") >> PC(stereoPool, "codec");
  loader->output("bit_rate") >> PC(stereoPool, "bit_rate");

  Network n(loader);
  n.run();

  int noofchannels = (int)stereoPool.value<Real>("channels");
  cout << "-------- Start Audio Data --------" << endl;
  //vector<Real> audioBuffer = stereoPool.value<vector<StereoSample>>("audio");
  if (noofchannels == 2)
  {
    cout << "Channel 0 :" << endl;
    for(int i = 0; i < stereoPool.value<vector<StereoSample>>("audio").size(); ++i) 
    {
      cout << stereoPool.value<vector<StereoSample>>("audio")[i].left() << ", ";
    }
    
    cout << endl << "Channel 1 :" << endl;
    for(int i = 0; i < stereoPool.value<vector<StereoSample>>("audio").size(); ++i)
    {
      cout << stereoPool.value<vector<StereoSample>>("audio")[i].right() << ", ";
    }
    cout << endl;
  }
  else
  {
    cout << "Audio Data (Only 1 channel --> Left) :" << endl;
    for(int i = 0; i < stereoPool.value<vector<StereoSample>>("audio").size(); ++i)
    {
      cout << stereoPool.value<vector<StereoSample>>("audio")[i].left() << ", ";
    }
    cout << endl;
  }
  cout << "-------- End Audio Data --------" << endl;

  //cout << "Audio Data:\n" << p.value<vector<StereoSample>>("audio") << endl;
  cout << "Sample Rate: " << stereoPool.value<Real>("samplerate") << endl;
  cout << "No. of Channels: " << noofchannels << endl;
  cout << "md5sum: " << stereoPool.value<string>("md5") << endl;
  cout << "Codec: " << stereoPool.value<string>("codec") << endl;
  cout << "Bit Rate: " << (int)stereoPool.value<Real>("bit_rate") << endl;
  
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
  
  n.clear();

  essentia::shutdown();

  return 0;
}
