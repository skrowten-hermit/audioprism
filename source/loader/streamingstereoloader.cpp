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
#include "credits/libav.h"
//#include <credits/libav.h>
//#include "libav.h"
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include "standard/monomixer.h"
#include <essentia/scheduler/network.h>

#define DEFAULT_INPUT "../../lib/essentia/test/audio/recorded/cat_purrrr.wav"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

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
  Pool monoPool;

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
  
  //standard::Algorithm* mono = standard::AlgorithmFactory::create("MonoMixer",
  //		                                                 "type", "left");
  //mono->input(stereoPool.value<vector<StereoSample>>("audio")).set(monoPool);
  //mono->compute();
  //mono->output("audio") >> PC(monoPool, "monoaudio");
  
  Algorithm* mono = AlgorithmFactory::create("MonoMixer",
		                             "type", "left");
  mono->input(stereoPool.value<vector<StereoSample>>("audio")).set(monoPool);
  mono->process();
  mono->output("audio") >> PC(monoPool, "monoaudio");

  cout << "Mono Audio Data:\n" << monoPool.value<vector<Real>>("monoaudio") << endl;
  
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
