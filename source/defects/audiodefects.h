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
 * File         : source/defects/audiodefects.h.
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



#ifndef AUDIOBUG_H
#define AUDIOBUG_H


#include "apstdh.hpp"
#include "apessh.hpp"
#include "constref.h"


class AudioBug
{
  private:
    std::string outputFile = DEFAULT_DOUTPUT;
    int saveOutput = 0;

    esstd::AlgorithmFactory& AF;

    esstd::Algorithm* fc;
    esstd::Algorithm* cd;
    esstd::Algorithm* gd;
    esstd::Algorithm* nb;
    esstd::Algorithm* Output;
    
    std::vector<Real> audioSignalVector;
    std::vector<Real> clickStarts, clickEnds;
    std::vector<Real> gapStarts, gapEnds;
    std::vector<Real> nbIndexes;
    
    int numClicks = 0;
    int numGaps = 0;
    int numNBSamples = 0;

    int clickExists = 0;
    int gapsExists = 0;
    int nbExists = 0;

    Real SampleRate;

  public:
    Pool defectsData;
    
    // Initialization and de-initialization of the defects class.
    AudioBug(esstd::AlgorithmFactory& saf, std::vector<Real> audioSigVector, 
             Pool audioAttrs, int saveData);
    ~AudioBug();

    // Functions for processing data of the defects class.
    int DetectClickPop();
    int DetectDiscontinuity();
    int DetectNoiseBurst();
    
    void DetectWhiteNoise();
    void DetectBGNoise();
    void DetectSilence();
    void DetectFade();
    void DetectChop();
    void DetectSideTone();
    void DetectHum();
    void DetectEcho();
    void DetectMetallicAudio();
    
    // Functions for storing the defects data in a pool data structure.
    Pool SetDefectInfo();
    Pool SetDefectInfo(std::string description, int split);
    
};


#endif
