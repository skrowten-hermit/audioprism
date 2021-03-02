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
 * File         : source/defects/audiodefects.cpp.
 * Description  : Functions to identify the defects in a given audio stream.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "audiodefects.h"

// AudioBug Constructor for initialization.
AudioBug::AudioBug(esstd::AlgorithmFactory& saf, 
                   std::vector<Real> audioSigVector, Pool attrs, 
                   int saveData, bool conOut) : saveOutput(saveData), 
                  consoleOut(conOut), AF(saf), audioSignalVector(audioSigVector)
{
  //essentia::init();

  std::string fileTag = attrs.value<std::string>("Description");
  Real SampleRate = attrs.value<Real>("SampleRate");
  
  clickExists = DetectClickPop();
  gapsExists = DetectDiscontinuity();
  nbExists = DetectNoiseBurst();

  defectsData = SetDefectInfo();
}

// Check for abnormal click/pop sounds.
int AudioBug::DetectClickPop()
{
  int cStartsLen, cEndsLen;
  int frameSize = 2048;
  int hopSize = 1024;
  // int sampleSize = audioSignalVector.size();
  
  std::vector<Real> frame, CDframe;
  
  fc = AF.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
  cd = AF.create("ClickDetector", "frameSize", frameSize, "hopSize", hopSize,
                 //"detectionThreshold", 50,
                 "sampleRate", SampleRate);
  
  fc->input("signal").set(audioSignalVector);
  fc->output("frame").set(CDframe);
  
  cd->input("frame").set(frame); 
  cd->output("starts").set(clickStarts);
  cd->output("ends").set(clickEnds);

  while (true)
  {
    fc->compute();

    if (!frame.size())
    {
      break;
    }
    
    cd->compute();
   }
  
  cStartsLen = clickStarts.size();
  cEndsLen = clickEnds.size();

  delete fc;
  delete cd;

  if (cStartsLen > 0 || cEndsLen > 0)
  {
    if (cStartsLen > cEndsLen)
      numClicks = cStartsLen;
    else if (cStartsLen < cEndsLen)
      numClicks = cEndsLen;
    else
      numClicks = cStartsLen;
  }

  // Return if click exists or not.
  if (numClicks > 0)
    return 1;
  else
    return 0;
}

// Check for discontinuity (abnormal breaks or gaps) within an audio signal.
int AudioBug::DetectDiscontinuity()
{
  int dStartsLen, dEndsLen;
  int frameSize = 2048;
  int hopSize = 1024;
  // int sampleSize = audioSignalVector.size();
  
  std::vector<Real> frame;
  
  fc = AF.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
  gd = AF.create("GapsDetector", "frameSize", frameSize, "hopSize", hopSize, 
                 "sampleRate", SampleRate);

  fc->input("signal").set(audioSignalVector);
  fc->output("frame").set(frame);
  
  gd->input("frame").set(frame); 
  gd->output("starts").set(gapStarts);
  gd->output("ends").set(gapEnds);

  while (true)
  {
    fc->compute();

    if (!frame.size())
    {
      break;
    }
    
    gd->compute();
  }

  dStartsLen = gapStarts.size();
  dEndsLen = gapEnds.size();

  delete fc;
  delete gd;

  if (dStartsLen > 0 || dEndsLen > 0)
  {
    if (dStartsLen > dEndsLen)
      numGaps = dStartsLen;
    else if (dStartsLen < dEndsLen)
      numGaps = dEndsLen;
    else
      numGaps = dStartsLen;
  }

  // Return if discontinuity exists or not.
  if (numGaps > 0)
    return 1;
  else
    return 0;
}

// Check for the presence of noise bursts within an audio signal.
int AudioBug::DetectNoiseBurst()
{
  int frameSize = 2048;
  int hopSize = 1024;
  // int sampleSize = audioSignalVector.size();
  
  std::vector<Real> frame, noiseburstIdxs; 
  int frameNo = 1;
  int offset = 0;
  
  fc = AF.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
  nb = AF.create("NoiseBurstDetector");

  fc->input("signal").set(audioSignalVector);
  fc->output("frame").set(frame);
  
  nb->input("frame").set(frame); 
  nb->output("indexes").set(noiseburstIdxs);

  while (true)
  {
    offset = frameNo * frameSize;
    
    fc->compute();

    if (!frame.size())
    {
      break;
    }
    
    if (isSilent(frame)) 
    {
      frameNo++;
      continue;
    }

    nb->compute();
    
    // Manipulate noiseburstIdxs to reflect sample numbers wrt signal vector instead of frame. Add offset and store in nbIndexes. This can be converted to seconds too.
    
    frameNo++;
  }

  numNBSamples = noiseburstIdxs.size();

  // Return if discontinuity exists or not, location(s) (in time) if exists.
  if (numNBSamples > 0)
    return 1;
  else
    return 0;
}

// Store all the attributes in a Pool data structure.
Pool AudioBug::SetDefectInfo()
{
  Pool dPool;
  // Return if silence exists or not, location(s) (in time) if exists.
  dPool.set("ClickExists", clickExists);
  dPool.set("ClickStarts", clickStarts);
  dPool.set("ClickEnds", SampleRate);
  //dPool.set("ClickDurations", clickLengths);
  dPool.set("GapsExists", gapsExists);
  dPool.set("GapStarts", gapStarts); 
  //dPool.set("GapDurations", gapLengths); 
  dPool.set("NoiseBurstsIdxs", nbIndexes);
  //dPool.set("NoiseBurstsDurations", nbLengths);

  return dPool;
}

// Store (for file printing) all the attributes in a Pool data structure.
Pool AudioBug::SetDefectInfo(std::string description, int split)
{
  Pool dPool;
  // Return if silence exists or not, location(s) (in time) if exists.
  dPool.set(description + ".ClickExists", clickExists);
  dPool.set(description + ".ClickStarts", clickStarts);
  dPool.set(description + ".ClickEnds", SampleRate);
  //dPool.set(description + ".ClickDurations", clickLengths);
  dPool.set(description + ".GapsExists", gapsExists);
  dPool.set(description + ".GapStarts", gapStarts); 
  //dPool.set(description + ".GapDurations", gapLengths); 
  dPool.set(description + ".NoiseBurstsIdxs", nbIndexes);
  //dPool.set(description + ".NoiseBurstsDurations", nbLengths);

  if (split == 1)
  {
    std::cout << "-------- writing results to file " << outputFile 
              << " --------" << std::endl;
              
    Output = AF.create("YamlOutput", "filename", outputFile);
    
    Output->input("pool").set(dPool);
    
    Output->compute();
    
    delete Output;
  }

  return dPool;
}

// Check for silence within an audio signal.
void AudioBug::DetectSilence()
{
  // Return if silence exists or not, location(s) (in time) if exists.
}

// Check for the presence of white noise within an audio signal.
void AudioBug::DetectWhiteNoise()
{
  // Return if white noise exists or not, location(s) (in time) if exists.
}

// Check if background noise exists in an audio signal.
void AudioBug::DetectBGNoise()
{
  // Return if background noise exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if the audio signal fades.
void AudioBug::DetectFade()
{
  // Return if fade exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if the audio signal is chopped.
void AudioBug::DetectChop()
{
  // Return if chop exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if a side-tone exists in the audio signal.
void AudioBug::DetectSideTone()
{
  // Return if side-tone exists or not.
  // Return the SNR.
}

// Check if hum exists in the audio signal.
void AudioBug::DetectHum()
{
  // Return if hum exists, location(s) (in time) if exists.
  // Return the SNR.
}

// Check if echo exists in the audio signal.
void AudioBug::DetectEcho()
{
  // Return if echo exists or not, location(s) (in time) if exists.
  // Return the SNR.
}


// Check if metallic audio exists in the audio signal.
void AudioBug::DetectMetallicAudio()
{
  // Return if metallic audio exists or not, location(s) (in time) if exists.
  // Return the SNR.
}

AudioBug::~AudioBug()
{
  //
}

