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
                         Pool srcAttrs, Pool snkAttrs, std::string srcdesc, 
                         std::string snkdesc, int saveData, std::string outFile, 
                         bool conOut) : 
                        outputFile(outFile), saveOutput(saveData), 
                        consoleOut(conOut), AF(saf), sourceSignal(source), 
                        sinkSignal(sink), srcDescr(srcdesc), snkDescr(snkdesc)
{
  //essentia::init();
  
  std::cout << std::endl << "Acquired signal vectors length :" << std::endl;
  std::cout << "Source signal samples : " << sourceSignal.size() << std::endl;
  std::cout << "Sink signal samples : " << sinkSignal.size() << std::endl;
  
  std::cout << std::endl;
  std::cout << "Comparing Audio files " << srcDescr << " (source) and " << 
               snkDescr << " (target)...." << std::endl;

  sourceRMS = srcAttrs.value<Real>(srcDescr + ".RMS");
  sinkRMS = snkAttrs.value<Real>(snkDescr + ".RMS");

  tie(sourceSignal, sinkSignal) = comp2padd(source, sink);

  sourceSize = sourceSignal.size();
  sinkSize = sinkSignal.size();
  
  std::cout << std::endl << "Normalized signal vectors length :" << std::endl;
  std::cout << "Source signal samples : " << sourceSize << std::endl;
  std::cout << "Sink signal samples : " << sinkSize << std::endl;

  xCorrSize = sourceSize;

  audioCrossCorrSeq = CalcCCFSeq();
  corrVectorPeak = GetCorrPeak();
  RMSMatch = CalcRMSMatch();
  gainDB = 20 * log10(sinkRMS / sourceRMS);
  audioExists = AudioExists();

  verifyInfo = SetVerifyData();

  if (consoleOut == true)
  {
    projectData();
    printPool();
  }
}

// Get the cross-correlation function (CCF) for a given set of 2 signal vectors 
// as input.
std::vector<Real> AudioVerify::CalcCCFSeq()
{
  Real normFactor = 0.0;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Calculating cross-correlation function of " << srcDescr << 
                 " and " << snkDescr << "...." << std::endl;
  }
  
  ccfAlgo =  AF.create("CrossCorrelation");
  sourceVarAlgo = AF.create("Variance");
  sinkVarAlgo = AF.create("Variance");

  ccfAlgo->input("arrayX").set(sourceSignal);
  ccfAlgo->input("arrayY").set(sinkSignal);
  ccfAlgo->output("crossCorrelation").set(corrVector);

  ccfAlgo->compute();

  sourceVarAlgo->input("array").set(sourceSignal);
  sourceVarAlgo->output("variance").set(sourceVar);
  
  sinkVarAlgo->input("array").set(sinkSignal);
  sinkVarAlgo->output("variance").set(sinkVar);

  sourceVarAlgo->compute();
  
  sinkVarAlgo->compute();

  xCorrVecSize = corrVector.size();

  sourceStdDev = sqrt(sourceVar);
  sinkStdDev = sqrt(sinkVar);
  normFactor = 1 / (sourceSize * sourceStdDev * sinkStdDev);
  
  transform(corrVector.begin(), corrVector.end(), corrVector.begin(), 
            [normFactor](Real &c){ return c * normFactor; });

  // Above lambda function can only capture local scoped variables.
  xcorrNormFactor = normFactor; 

  verifyData.set(srcDescr + "--x--" + snkDescr + ".CorrVector", corrVector);
  verifyData.set(srcDescr + "--x--" + snkDescr + ".SourceVariance", sourceVar);
  verifyData.set(srcDescr + "--x--" + snkDescr + ".SourceStdDev", sourceStdDev);
  verifyData.set(srcDescr + "--x--" + snkDescr + ".SinkVariance", sinkVar);
  verifyData.set(srcDescr + "--x--" + snkDescr + ".SinkStdDev", sinkStdDev);

  delete ccfAlgo;
  delete sourceVarAlgo;
  delete sinkVarAlgo;

  if (consoleOut == true)
    std::cout << "Computation of cross-correlation function of " << srcDescr <<
                 " and " << snkDescr << " done...." << std::endl;

  // Return the CCF sequence.
  return corrVector;
}

// Get the verification metric - cross-correlation peak.
Real AudioVerify::GetCorrPeak()
{
  Real corrVectorPeak;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Retrieving the cross-correlation peak...." << std::endl;
  }
  
  corrVectorPeak = *max_element(std::begin(audioCrossCorrSeq), 
                                std::end(audioCrossCorrSeq));

  if (consoleOut == true)
    std::cout << "Cross-correlation peak retrieved...." << std::endl;

  return corrVectorPeak;
}

// Calculate the RMS value from a given signal vector input.
Real AudioVerify::CalcRMSMatch()
{
  Real RMSMatchPerc;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Calculating the RMS match percentage...." << std::endl;
  }

  gainFactor = sinkRMS / sourceRMS;
  RMSMatchPerc = gainFactor * 100;

  if (consoleOut == true)
    std::cout << "RMS match percentage computed...." << std::endl;

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
std::string AudioVerify::AudioExists()
{
  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Checking if audio exists...." << std::endl;
  }

  // Return 1 if audio exists, -1 if no audio.
  if (-(APGA_THRESHOLD) <= round(gainDB) && round(gainDB) <= APGA_THRESHOLD)
    return "Good Audio";
  else if (round(gainDB) < APNA_THRESHOLD)
    return "No Audio";
  else if (APGA_THRESHOLD < round(gainDB) && round(gainDB) <= APHG_THRESHOLD)
    return "Audio With Moderately High Gain";
  else if (APHG_THRESHOLD < round(gainDB) && round(gainDB) <= APVH_THRESHOLD)
    return "Audio With High Gain";
  else if (round(gainDB) > APVH_THRESHOLD)
    return "Audio With Very High Gain";
  else if (APLG_THRESHOLD <= round(gainDB) && round(gainDB) < -(APGA_THRESHOLD))
    return "Audio With Moderately Low Gain";
  else if (APVL_THRESHOLD <= round(gainDB) && round(gainDB) < APLG_THRESHOLD)
    return "Audio With Low Gain";
  else if (APNA_THRESHOLD <= round(gainDB) && round(gainDB) < APVL_THRESHOLD)
    return "Audio With Very Low Gain";
  else
    return "N/A";

  // TODO: Add code to check if defects are there.
}

// Store all the attributes in a Pool data structure.
Pool AudioVerify::SetVerifyData()
{
  // Set audioExists, gainFactor, audioSimilarityIndex, audioDelay, sourceRMS, 
  // sinkRMS, RMSMatch after calculating sourceAutoCorrSeq, sinkAutoCorrSeq, 
  // audioCrossCorrSeq.
  Pool vPool;

  vPool.set(srcDescr + "--x--" + snkDescr + ".Source", srcDescr);
  vPool.set(srcDescr + "--x--" + snkDescr + ".Degraded", snkDescr);
  vPool.set(srcDescr + "--x--" + snkDescr + ".AudioExists", audioExists);
  vPool.set(srcDescr + "--x--" + snkDescr + ".CorrVectorSize", xCorrSize);
  vPool.set(srcDescr + "--x--" + snkDescr + ".CorrVectorPeak", 
            corrVectorPeak);
  vPool.set(srcDescr + "--x--" + snkDescr + ".GainFactor", gainFactor);
  vPool.set(srcDescr + "--x--" + snkDescr + ".GainDecibels", gainDB);
  vPool.set(srcDescr + "--x--" + snkDescr + ".RMSMatch", RMSMatch);

  return vPool;
}

// Store (for file printing) all the attributes in a Pool data structure.
void AudioVerify::WriteToFile()
{
  Pool vPool;

  if (consoleOut == true)
  {
    std::cout << std::endl;
    std::cout << "Storing Audio verification details between " << srcDescr << 
                 " and " << snkDescr << " externally...." << std::endl;
    std::cout << "-------- writing results to file " << outputFile 
              << " --------" << std::endl;
  }

  vPool.merge(verifyData);
  vPool.merge(verifyInfo);
    
  Output = AF.create("YamlOutput", "filename", outputFile);
  Output->input("pool").set(vPool);
  Output->compute();
  delete Output;

  if (consoleOut == true)
    std::cout << "Audio verification details between " << srcDescr << " and " 
              << snkDescr << " externally...." << std::endl;
}

// Print the members of the AudioLoad class to the console.
void AudioVerify::projectData()
{
  std::cout << std::endl;
  std::cout << "The following are data from verifier operations between " << 
               srcDescr << " and " << snkDescr << ":" << std::endl;
  std::cout << "Audio Present : " << audioExists << std::endl;
  std::cout << "RMS of Source Signal : " << sourceRMS << std::endl;
  std::cout << "Sample Size of Source Signal : " << sourceSize << std::endl;
  std::cout << "Variance of Source Signal : " << sourceVar << std::endl;
  std::cout << "Std. Dev of Source Signal : " << sourceStdDev << std::endl;
  std::cout << "RMS of Degraded Signal : " << sinkRMS << std::endl;
  std::cout << "Sample Size of Degraded Signal : " << sinkSize << std::endl;
  std::cout << "Variance of Degraded Signal : " << sinkVar << std::endl;
  std::cout << "Std. Dev of Degraded Signal : " << sinkStdDev << std::endl;
  std::cout << "CCF Vector Size (actual) : " << xCorrVecSize << std::endl;
  std::cout << "CCF Vector Size (to be used) : " << xCorrSize << std::endl;
  std::cout << "CCF Peak : " << corrVectorPeak << std::endl;
  std::cout << "RMS Match : " << RMSMatch << std::endl;
  std::cout << "Gain Factor : " << gainFactor << std::endl;
  std::cout << "Gain in Decibels (dB) : " << gainDB << std::endl;
}

// Display the data from the data structure on the console.
void AudioVerify::printPool()
{
  std::cout << std::endl;
  std::cout << "The following are data from internal data structure:" << 
               std::endl;
  std::cout << "Audio Source Signal : " << 
               verifyInfo.value<std::string>(srcDescr + "--x--" + snkDescr + 
                                             ".Source") << std::endl;
  std::cout << "Audio Sink Signal : " << 
               verifyInfo.value<std::string>(srcDescr + "--x--" + snkDescr + 
                                             ".Degraded") << std::endl;
  std::cout << "Audio Present : " << 
               verifyInfo.value<std::string>(srcDescr + "--x--" + snkDescr + 
                                             ".AudioExists") << std::endl;
  std::cout << "CCF Peak : " << 
               verifyInfo.value<Real>(srcDescr + "--x--" + snkDescr + 
                                      ".CorrVectorPeak") << std::endl;
  std::cout << "RMS Match : " << 
               verifyInfo.value<Real>(srcDescr + "--x--" + snkDescr + 
                                      ".RMSMatch") << std::endl;
  std::cout << "Gain Factor : " << 
               verifyInfo.value<Real>(srcDescr + "--x--" + snkDescr + 
                                      ".GainFactor") << std::endl;
  std::cout << "Gain in Decibels (dB) : " << 
               verifyInfo.value<Real>(srcDescr + "--x--" + snkDescr + 
                                      ".GainDecibels") << std::endl;
}

// AudioVerify Destructor for closure.
AudioVerify::~AudioVerify()
{
  // essentia::shutdown();
}
