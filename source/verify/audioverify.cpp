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
 * File         : source/verify/audioverify.cpp.
 * Description  : Functions to compare and verify 2 given audio streams.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "audioverify.h"
#include "apmath.h"

// AudioVerify Constructor for initialization.
AudioVerify::AudioVerify(esstd::AlgorithmFactory& saf, 
                         std::vector<Real> source, 
                         std::vector<Real> sink, 
                         Pool srcAttrs, Pool snkAttrs, 
                         std::string outFile, int saveData) : 
                         outputFile(outFile), saveOutput(saveData), AF(saf), 
                         sourceSignal(source), sinkSignal(sink)
                         
{
  //essentia::init();

  sourceRMS = srcAttrs.value<Real>("RMS");
  sinkRMS = snkAttrs.value<Real>("RMS");

  tie(sourceSignal, sinkSignal) = comp2padd(source, sink);

  sourceSize = sourceSignal.size();
  sinkSize = sinkSignal.size();
  xCorrSize = sourceSize;

  audioCrossCorrSeq = CalcCCFSeq();
  corrVectorPeak = GetCorrPeak();
  RMSMatch = CalcRMSMatch();
  audioExists = AudioExists();

  verifyData = SetVerifyData(srcAttrs.value<std::string>("Description"), 
                             snkAttrs.value<std::string>("Description"), 
                             saveOutput);
}

// Get the cross-correlation function (CCF) for a given set of 2 signal vectors 
// as input.
std::vector<Real> AudioVerify::CalcCCFSeq()
{
  Real sourceVar, sinkVar;
  std::vector<Real> corrVector;
  
  ccfAlgo =  AF.create("CrossCorrelation");
  sourceVarAlgo = AF.create("Variance");
  sinkVarAlgo = AF.create("Variance");

  ccfAlgo->output("crossCorrelation").set(corrVector);

  ccfAlgo->compute();

  sourceVarAlgo->input("array").set(sourceSignal);
  sourceVarAlgo->output("variance").set(sourceVar);
  
  sinkVarAlgo->input("array").set(sinkSignal);
  sinkVarAlgo->output("variance").set(sinkVar);

  sourceVarAlgo->compute();
  
  sinkVarAlgo->compute();

  Real sourceStdDev = sqrt(sourceVar);
  Real sinkStdDev = sqrt(sinkVar);
  Real xcorrNormFactor = 1 / (sourceSize * sourceStdDev * sinkStdDev);
  
  transform(corrVector.begin(), corrVector.end(), corrVector.begin(), 
            [xcorrNormFactor](Real &c){ return c * xcorrNormFactor; });

  delete ccfAlgo;
  delete sourceVarAlgo;
  delete sinkVarAlgo;

  // Return the CCF sequence.
  return corrVector;
}

// Get the verification metric - cross-correlation peak.
Real AudioVerify::GetCorrPeak()
{
  Real corrVectorPeak;
  
  corrVectorPeak = *max_element(std::begin(audioCrossCorrSeq), 
                                std::end(audioCrossCorrSeq));

  return corrVectorPeak;
}

// Calculate the RMS value from a given signal vector input.
Real AudioVerify::CalcRMSMatch()
{
  Real RMSMatchPerc;

  gainFactor = sinkRMS / sourceRMS;
  RMSMatchPerc = gainFactor * 100;

  // Return the RMS match value (recorded wrt source).
  return RMSMatchPerc;
}

// Calculate the delay from an input CCF or ACF sequence.
Real AudioVerify::CalcDelay()
{
  Real delay = 0.0;
  // Return the delay.
  return delay;
}

// Get the right channel of a stereo audio signal from a given audio file.
int AudioVerify::AudioExists()
{
  // Return 1 if audio exists, -1 if no audio.
  if (corrVectorPeak > APAE_THRESHOLD)
    return 1;
  else
    return -1;
}

// Store all the attributes in a Pool data structure.
Pool AudioVerify::SetVerifyData()
{
  // Set audioExists, gainFactor, audioSimilarityIndex, audioDelay, sourceRMS, 
  // sinkRMS, RMSMatch after calculating sourceAutoCorrSeq, sinkAutoCorrSeq, 
  // audioCrossCorrSeq.
  Pool vPool;

  vPool.set("Source", srcDescr);
  vPool.set("Degraded", snkDescr);
  vPool.set("AudioExists", audioExists);
  vPool.set("CorrVectorPeak", corrVectorPeak);
  vPool.set("GainFactor", gainFactor);
  vPool.set("RMSMatch", RMSMatch);

  return vPool;
}

// Store (for file printing) all the attributes in a Pool data structure.
Pool AudioVerify::SetVerifyData(std::string srcDescr, 
                                std::string snkDescr, int split)
{
  // Set audioExists, gainFactor, audioSimilarityIndex, audioDelay, sourceRMS, 
  // sinkRMS, RMSMatch after calculating sourceAutoCorrSeq, sinkAutoCorrSeq, 
  // audioCrossCorrSeq.
  Pool vPool;

  vPool.set(srcDescr + "-->" + snkDescr + ".AudioExists", audioExists);
  vPool.set(srcDescr + "-->" + snkDescr + ".CorrVectorPeak", 
                 corrVectorPeak);
  vPool.set(srcDescr + "-->" + snkDescr + ".GainFactor", gainFactor);
  vPool.set(srcDescr + "-->" + snkDescr + ".RMSMatch", RMSMatch);

  if (split == 1)
  {
    Output = AF.create("YamlOutput", "filename", outputFile);
    
    Output->input("pool").set(vPool);
    
    Output->compute();
    
    delete Output;
  }

  return vPool;
}

// AudioVerify Destructor for closure.
AudioVerify::~AudioVerify()
{
  // 
}
