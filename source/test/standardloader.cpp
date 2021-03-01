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
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>

#define DEFAULT_INPUT "../../lib/essentia/test/audio/recorded/cat_purrrr.wav"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

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

  AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
  Algorithm* loader =  factory.create("AudioLoader",
		                      "filename", audioFileName,
				      "computeMD5", true);



  //loader->output("audio").set(audioBuffer);
  //loader->output("audio") >> PC(stereoPool, "audio");
  //loader->output("sampleRate") >> PC(stereoPool, "samplerate");
  //loader->output("numberChannels") >> PC(stereoPool, "channels");
  //loader->output("md5") >> PC(stereoPool, "md5");
  //loader->output("codec") >> PC(stereoPool, "codec");
  //loader->output("bit_rate") >> PC(stereoPool, "bit_rate");

  //Network n(loader);
  //n.run();
 
  loader->compute();

  //while (true)
  //{   
    stereoPool.add("audio", loader->output("audio"));
    stereoPool.add("samplerate", loader->output("sampleRate")); 
    stereoPool.add("channels", loader->output("numberChannels"));
    stereoPool.add("md5", loader->output("md5"));
    stereoPool.add("codec", loader->output("codec"));
    stereoPool.add("bitrate", loader->output("bit_rate"));

  //} 
  
  vector<Real> audioBuffer = stereoPool.value<vector<StereoSample>>("audio");
  float samplerate = stereoPool.value<Real>("samplerate"); 
  int noofchannels = (int)stereoPool.value<Real>("channels");
  string md5sum = stereoPool.value<string>("md5");
  string codec = stereoPool.value<string>("codec");
  int bitrate = (int)stereoPool.value<Real>("bit_rate");
  
  cout << "-------- Start Audio Data --------" << endl;
  cout << "Audio Data:\n" << audioBuffer << endl;
  cout << "-------- End Audio Data --------" << endl;
  cout << "Sample Rate: " << (float)stereoPool.value<Real>("samplerate") << endl;
  cout << "No. of Channels: " << noofchannels << endl;
  cout << "md5sum: " << md5sum << endl;
  cout << "Codec: " << codec << endl;
  cout << "Bit Rate: " << bitrate << endl;
  
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
  
  //n.clear();

  essentia::shutdown();

  return 0;
}
