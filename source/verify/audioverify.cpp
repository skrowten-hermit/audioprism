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
#include "audioverify.h"

// AudioVerify Constructor for initialization.
AudioVerify::AudioVerify(vector<Real> source, vector<Real> sink) : sourceSignal(source), sinkSignal(sink)
{
  essentia::init();
}

// Get the auto-correlation function (ACF) for a given signal vector input.
vector<Real> AudioVerify::CalcACFSeq(vector<Real> signalVector)
{
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Algorithm* acfalgo =  factory.create("AutoCorrelation");

  // Return the ACF sequence.
}

// Get the cross-correlation function (CCF) for a given set of 2 signal vectors as input.
vector<Real> AudioVerify::CalcCCFSeq(vector<Real> sourceSigVector, vector<Real> sinkSigVector)
{
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Algorithm* acfalgo =  factory.create("CrossCorrelation");

  // Return the CCF sequence.
}

// Calculate the delay from an input CCF or ACF sequence.
Real AudioVerify::CalcDelay(vector<Real> sigCCFSeq)
{
  // Return the delay.
}

// Calculate the RMS value from a given signal vector input.
Real AudioVerify::CalcRMS(vector<Real> signalVector)
{
  // Return the RMS value.
}

// Get the right channel of a stereo audio signal from a given audio file.
int AudioVerify::AudioExists(vector<Real> sourceSigVector, vector<Real> sinkSigVector)
{
  // Return 1 if audio exists, -1 if no audio.
}

void  AudioVerify::SetProperties(vector<Real> sourceSigVector, vector<Real> sinkSigVector)
{
  // Set audioExists, gainFactor, audioSimilarityIndex, audioDelay, sourceRMS, sinkRMS, RMSMatch after calculating sourceAutoCorrSeq, sinkAutoCorrSeq, audioCrossCorrSeq.
}

