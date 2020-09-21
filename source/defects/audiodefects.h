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
#define DEFAULT_OUTPUT "defectresults.yaml"
#endif

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

#ifndef AUDIOBUG_H
#define AUDIOBUG_H

class AudioBug
{
  private:
    vector<Real> audioSignalVector;
    string outputFile = DEFAULT_OUTPUT;
    int saveOutput = 0;
    
  public:
    AudioBug(vector<Real> audioSigVector);
    void DetectClickPop(vector<Real> audioSigVector);
    void DetectSilence(vector<Real> audioSigVector);
    void DetectDiscontinuity(string inFile, string source);
    void DetectWhiteNoise(string inFile);
    void DetectBGNoise(string inFile);
    void DetectFade(vector<Real> audioSigVector);
    void DetectChop(vector<Real> audioSigVector);
    void DetectSideTone(vector<Real> audioSigVector);
    void DetectHum(vector<Real> audioSigVector);
    void DetectEcho(vector<Real> audioSigVector);
    void DetectMetallicAudio(vector<Real> audioSigVector);
};

#endif
