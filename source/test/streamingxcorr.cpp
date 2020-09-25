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
#include "standard/crosscorrelation.h"
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
  Pool monoPool;
  Pool corrPool;

  typedef vector<Real> monoVector;
  typedef vector<Real> corrVector;

  int frameSize = 2048;
  int hopSize = 1024;

  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Algorithm* loader =  factory.create("AudioLoader",
                                      "filename", audioFileName,
                                      "computeMD5", true);
  Algorithm* mono = AlgorithmFactory::create("MonoMixer",
                                             "type", "left");
  Algorithm* fc = factory.create("FrameCutter",
                                 "frameSize", frameSize,
                                 "hopSize", hopSize,
                                 "silentFrames", "noise");
  Algorithm* xcorr = AlgorithmFactory::create("CrossCorrelation");
  //standard::Algorithm* xcorr = standard::AlgorithmFactory::create("CrossCorrelation");
  
  loader->output("audio") >> PC(stereoPool, "audio");
  loader->output("sampleRate") >> PC(stereoPool, "samplerate");
  loader->output("numberChannels") >> PC(stereoPool, "channels");
  loader->output("md5") >> PC(stereoPool, "md5");
  loader->output("codec") >> PC(stereoPool, "codec");
  loader->output("bit_rate") >> PC(stereoPool, "bit_rate");
  loader->output("audio") >> mono->input("audio");
  loader->output("numberChannels") >> mono->input("numberChannels");
  mono->output("audio") >> PC(monoPool, "monoaudio");
  /*mono->output("audio") >> monoVector;
  //mono->output("audio") >> xcorr->input("arrayX");
  monoVector >> xcorr->input("arrayX");
  //monoPool.value<vector<Real>>("monoaudio") >> xcorr->input("arrayX");
  //mono->output("audio") >> xcorr->input("arrayX");
  //mono->output("audio") >> xcorr->input("arrayY");
  monoVector >> xcorr->input("arrayY");
  //monoPool.value<vector<Real>>("monoaudio") >> xcorr->input("arrayY");
  //mono->output("audio") >> xcorr->input("arrayY");
  xcorr->output("crossCorrelation") >> PC(monoPool, "correlation");
  xcorr->output("crossCorrelation") >> corrVector;*/
  mono->output("audio") >> fc->input("signal");
  fc->output("frame") >> xcorr->input("arrayX");
  fc->output("frame") >> xcorr->input("arrayY");
  xcorr->output("crossCorrelation") >> PC(corrPool, "crosscorr");
  //xcorr->output("crossCorrelation").set(corrVector);

  Network n(loader);
  n.run();

  /*monoVector = monoPool.value<vector<Real>>("monoaudio");
  xcorr->input("arrayX").set(monoVector);
  xcorr->input("arrayY").set(monoVector);
  xcorr->output("crossCorrelation").set(corrVector);
  xcorr->compute();*/
  
  cout << "Mono Audio Data:\n" << monoPool.value<vector<Real>>("monoaudio") << endl;
  cout << "Cross Correlation:\n" << corrPool.value<vector<vector<Real>>>("crosscorr") << endl;
  
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
