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
#include "audiodefects.h"

// AudioAttrs Constructor for initialization.
AudioBug::AudioBug(vector<Real> audioSigVector) : audioSignalVector(audioSigVector)
{
  essentia::init();
}

// Check for click/pop sounds.
void AudioBug::DetectClickPop(vector<Real> audioSigVector)
{
  // Return if click exists or not, location(s) (in time) if exists.
}

// Check for silence within an audio signal.
void AudioBug::DetectSilence(vector<Real> audioSigVector)
{
  // Return if silence exists or not, location(s) (in time) if exists.
}

// Check for discontinuity within an audio signal.
void AudioBug::DetectDiscontinuity(vector<Real> audioSigVector)
{
  // Return if discontinuity exists or not, location(s) (in time) if exists.
}

// Check for the presence of white noise within an audio signal.
void AudioBug::DetectWhiteNoise(vector<Real> audioSigVector)
{
  // Return if white noise exists or not, location(s) (in time) if exists.
}

// Check if background noise exists in an audio signal.
void AudioBug::DetectBGNoise(vector<Real> audioSigVector)
{
  // Return if background noise exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if the audio signal fades.
void AudioBug::DetectFade(vector<Real> audioSigVector)
{
  // Return if fade exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if the audio signal is chopped.
void AudioBug::DetectChop(vector<Real> audioSigVector)
{
  // Return if chop exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if a side-tone exists in the audio signal.
void AudioBug::DetectSideTone(vector<Real> audioSigVector)
{
  // Return if side-tone exists or not.
  // Return the SNR.
}

// Check if hum exists in the audio signal.
void AudioBug::DetectHum(vector<Real> audioSigVector)
{
  // Return if hum exists, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if echo exists in the audio signal.
void AudioBug::DetectEcho(vector<Real> audioSigVector)
{
  // Return if echo exists or not, location(s) (in time) if exists.
  // Return the SNR.
}


// Check if metallic audio exists in the audio signal.
void AudioBug::DetectMetallicAudio(vector<Real> audioSigVector)
{
  // Return if metallic audio exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

