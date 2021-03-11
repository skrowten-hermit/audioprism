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
 * File         : source/verify/audioverify.h.
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



#ifndef AUDIOVERIFY_H
#define AUDIOVERIFY_H


#include "apstdh.hpp"
#include "apessh.hpp"
#include "constref.h"


class AudioVerify
{
  private:
    std::string outputFile = DEFAULT_VOUTPUT;
    int saveOutput = 0;
    bool consoleOut = false;

    esstd::AlgorithmFactory& AF;

    esstd::Algorithm* ccfAlgo;
    esstd::Algorithm* sourceVarAlgo;
    esstd::Algorithm* sinkVarAlgo;
    esstd::Algorithm* Output;

    std::vector<Real> sourceSignal;
    std::vector<Real> sinkSignal;
    std::vector<Real> corrVector;

    std::vector<Real> audioCrossCorrSeq;
    
    long unsigned int sourceSize = 0, sinkSize = 0;
    long unsigned int xCorrVecSize = 0, xCorrSize = 0;
    Real corrVectorPeak;
    
    Real sourceVar, sinkVar;
    Real sourceStdDev, sinkStdDev;
    Real xcorrNormFactor;
    
    Real sourceRMS = 0.0;
    Real sinkRMS = 0.0;
    Real RMSMatch = 0.0;
    Real gainFactor = 0.0; 
    Real gainDB = 0.0;
    
    Real audioSimilarityIndex;
    Real audioDelay;

    Pool verifyData;

    std::string srcDescr, snkDescr;
    std::string similarity, audioExists;

    // Functions for processing data of the verifier class.
    std::vector<Real> CalcCCFSeq();
    Real GetCorrPeak();
    Real CalcRMSMatch();
    std::string AudioExists();
    Real CalcDelay();
    
    // Functions for storing the attributes data in a pool data structure.
    Pool SetVerifyData();  // For internal storage.
    Pool verifyInfo;
    
  public:
    // Initialization and de-initialization of the verifier class.
    AudioVerify(esstd::AlgorithmFactory& saf, 
                std::vector<Real> source, std::vector<Real> sink, 
                Pool srcAttrs, Pool snkAttrs, 
                std::string srcdesc, std::string snkdesc, 
                int saveData, std::string outFile, 
                bool conOut);
    ~AudioVerify();

    // Get a non-modifiable data for processing.
    const Pool &GetValidationData() const { return verifyInfo; }
    
    // Function for storing the attributes data in a file.
    void WriteToFile(); // For external storage.

    // Function to display calculated parameters on console.
    void projectData();
    void printPool();

};


#endif
