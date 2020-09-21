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
#include "audioattrs.h"

// AudioAttrs Constructor for initialization.
AudioAttrs::AudioAttrs(vector<Real> audioSigVector) : audioSignalVector(audioSigVector)
{
  essentia::init();
}

// Get the fundamental frequency (F0) of the audio signal.
void AudioAttrs::GetF0Freq(vector<Real> audioSigVector)
{
  // Return the fundamental frequency F0.
}

// Get all the frequencies present in the audio signal.
void AudioAttrs::GetFreqComponents(vector<Real> audioSigVector)
{
  // Return an array of frequencies.
}

// Get the gain factor of the audio signal.
void AudioAttrs::GetGainFactor(vector<Real> audioSigVector)
{
  // Return gain factor.
}

// Get the gain of the audio signal in dB.
void AudioAttrs::GetGainDB(vector<Real> audioSigVector)
{
  // Return gain in dB.
}

// Get the SNR of the audio signal.
void AudioAttrs::GetSNR(vector<Real> audioSigVector)
{
  // Return the SNR.
}

