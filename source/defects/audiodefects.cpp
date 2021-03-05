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
                   std::string description, int saveData, bool conOut) : 
                  saveOutput(saveData), consoleOut(conOut), AF(saf), 
                  audioSignalVector(audioSigVector), fileTag(description)
{
  //essentia::init();

  SampleRate = attrs.value<Real>(fileTag + ".SampleRate");
  
  std::cout << std::endl;
  std::cout << "Identifying Audio Signal Defects in " << fileTag << "...." << 
               std::endl;
  
  clickExists = DetectClickPop();
  gapsExists = DetectDiscontinuity();
  nbExists = DetectNoiseBurst();

  defectsData = SetDefectInfo();

  if (consoleOut == true)
  {
    projectData();
    printPool();
  }
}

// Check for abnormal click/pop sounds.
int AudioBug::DetectClickPop()
{
  int frameSize = 2048;
  int hopSize = 1024;
  // int sampleSize = audioSignalVector.size();
  
  std::vector<Real> frame, CDframe;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Detecting clicks/pops in Audio signal " << fileTag << 
                 "...." << std::endl;
  }
  
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
  
  clickStartsLen = clickStarts.size();
  clickEndsLen = clickEnds.size();

  delete fc;
  delete cd;

  if (clickStartsLen > 0 || clickEndsLen > 0)
  {
    if (clickStartsLen > clickEndsLen)
      numClicks = clickStartsLen;
    else if (clickStartsLen < clickEndsLen)
      numClicks = clickEndsLen;
    else
      numClicks = clickStartsLen;
  }

  if (consoleOut == true)
    std::cout << "Click/pop detection in " << fileTag << " done...." << 
                 std::endl;

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

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Detecting discontinuities in Audio signal of " << fileTag 
              << "...." << std::endl;
  }
  
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

  if (consoleOut == true)
    std::cout << "Discontinuity detection in " << fileTag << " done...." << 
                 std::endl;

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

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Detecting noise bursts in Audio signal of " << fileTag 
              << "...." << std::endl;
  }
  
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

  if (consoleOut == true)
    std::cout << "Noise bursts detection in " << fileTag << " done...." << 
                 std::endl;

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
  dPool.set(fileTag + ".ClickExists", clickExists);
  dPool.set(fileTag + ".ClickStarts", clickStarts);
  dPool.set(fileTag + ".ClickEnds", clickEnds);
  dPool.set(fileTag + ".NumClicks", numClicks);
  //dPool.set(fileTag + ".ClickDurations", clickLengths);
  dPool.set(fileTag + ".GapsExists", gapsExists);
  dPool.set(fileTag + ".GapStarts", gapStarts); 
  dPool.set(fileTag + ".GapEnds", gapEnds); 
  dPool.set(fileTag + ".NumGaps", numGaps); 
  //dPool.set(fileTag + ".GapDurations", gapLengths); 
  dPool.set(fileTag + ".NoiseBurstsIdxs", nbIndexes);
  dPool.set(fileTag + ".NumNoiseBursts", numNBSamples);
  //dPool.set(fileTag + ".NoiseBurstsDurations", nbLengths);

  return dPool;
}

// Store (for file printing) all the attributes in a Pool data structure.
void AudioBug::WriteToFile()
{
  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Storing Audio defects from " << fileTag << " externally...." 
              << std::endl;
    std::cout << "-------- writing results to file " << outputFile 
              << " --------" << std::endl;
  }
    
  Output = AF.create("YamlOutput", "filename", outputFile);
  Output->input("pool").set(defectsData);
  Output->compute();
  delete Output;

  if (consoleOut == true)
    std::cout << "Audio defects from " << fileTag << " stored externally...." 
              << std::endl;
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

// Print the members of the AudioLoad class to the console.
void AudioBug::projectData()
{
  std::cout << std::endl;
  std::cout << "The following are data from " << fileTag << ":" << std::endl;
  std::cout << "Sample Rate : " << SampleRate << std::endl;
  std::cout << "Clicks Present : " << clickExists << std::endl;
  std::cout << "Clicks Starts Length : " << clickStartsLen << std::endl;
  std::cout << "Clicks Ends Length : " << clickEndsLen << std::endl;
  std::cout << "No. of Clicks : " << numClicks << std::endl;
  std::cout << "Gaps Present : " << gapsExists << std::endl;
  std::cout << "Gaps Starts Length : " << gapStartsLen << std::endl;
  std::cout << "Gaps Ends Length : " << gapEndsLen << std::endl;
  std::cout << "No. of Gaps : " << numGaps << std::endl;
  std::cout << "Noise Bursts Present : " << nbExists << std::endl;
  std::cout << "No. of Noise Bursts : " << numNBSamples << std::endl;
}

// Display the data from the data structure on the console.
void AudioBug::printPool()
{
  std::cout << std::endl;
  std::cout << "The following are data from internal data structure:" << 
               std::endl;
  std::cout << "Clicks Present : " << defectsData.value<Real>(fileTag + ".ClickExists") 
            << std::endl;
  std::cout << "Clicks Starts : \n" << 
               defectsData.value<std::vector<Real>>(fileTag + ".ClickStarts") << std::endl;
  std::cout << "Clicks Ends : \n" << 
               defectsData.value<std::vector<Real>>(fileTag + ".ClickEnds") << std::endl;
  std::cout << "No. of Clicks : " << 
               defectsData.value<Real>(fileTag + ".NumClicks") << std::endl;
  std::cout << "Gaps Present : " << 
               defectsData.value<Real>(fileTag + ".GapsExists") << std::endl;
  std::cout << "Gaps Starts : \n" << 
               defectsData.value<std::vector<Real>>(fileTag + ".GapStarts") << 
               std::endl;
  std::cout << "Gaps Ends : \n" << 
               defectsData.value<std::vector<Real>>(fileTag + ".GapEnds") << std::endl;
  std::cout << "No. of Gaps : " << defectsData.value<Real>(fileTag + ".NumGaps") 
            << std::endl;
  std::cout << "Noise Bursts : \n" << 
               defectsData.value<std::vector<Real>>(fileTag + ".NoiseBurstsIdxs") 
            << std::endl;
  std::cout << "No. of Noise Bursts : " << 
               defectsData.value<Real>(fileTag + ".NumNoiseBursts") << std::endl;
}

AudioBug::~AudioBug()
{
  //
}

