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

#include "credits/libav.h"
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

#ifndef DEFAULT_INPUT
#define DEFAULT_INPUT "../../lib/essentia/test/audio/recorded/cat_purrrr.wav"
#endif

#ifndef DEFAULT_OUTPUT
#define DEFAULT_OUTPUT "loadresults.yaml"
#endif

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

#ifndef AUDIOLOAD_H
#define AUDIOLOAD_H

class AudioLoad
{
  private:
    string audioFile = DEFAULT_INPUT;
    string outputFile = DEFAULT_OUTPUT;
    int saveOutput = 0;
    
    Pool stereoPool;
    Pool leftPool;
    Pool rightPool;
    Pool monoPool;
  
  public:
    AudioLoad(string inFile, string outFile);
    void GetAudioData(string inFile);
    void GetStereoChannel(string inFile);
    void GetMonoChannel(string inFile, string source);
    void GetLeftChannel(string inFile);
    void GetRightChannel(string inFile);
};

#endif
