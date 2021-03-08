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
 * File         : source/loader/audioloader.h.
 * Description  : Functions to load and extract audio streams from a audio file.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */



#ifndef AUDIOLOAD_H
#define AUDIOLOAD_H


#include "apstdh.hpp"
#include "apessh.hpp"
#include "constref.h"


class AudioLoad
{
  private:
    std::string audioFile = DEFAULT_INPUT;
    std::string outputFile = DEFAULT_LOUTPUT;
    int saveOutput = 0;
    bool consoleOut = false;

    esstd::AlgorithmFactory& AF;

    esstd::Algorithm* Loader;
    esstd::Algorithm* MonoData;
    esstd::Algorithm* Output;

    std::vector<Real> MonoBuffer;
    std::vector<Real> LeftBuffer, RightBuffer;
    std::vector<StereoSample> AudioBuffer, StereoBuffer;

    long unsigned int BSize = 0; 
    long unsigned int MBSize = 0;
    long unsigned int ABSize = 0;
    long unsigned int SBSize = 0;
    
    Real SampleRate;
    Real Channels;
    Real BitRate;
    std::string md5sum;
    std::string Codec;

    Pool audioData, audioInfo;
    
    std::string fileTag;

    // Functions for processing data of the loader class.
    std::vector<StereoSample> GetAudioData();
    std::vector<Real> GetMonoData();
    std::vector<Real> GetLeftChannel();
    std::vector<Real> GetRightChannel();
    
    // Functions for storing the attributes data in a pool data structure.
    Pool StoreAudio();  // For internal storage.
  
  public:
    // Initialization and de-initialization of the loader class.
    AudioLoad(esstd::AlgorithmFactory& saf, std::string inFile, 
              std::string description, int saveData, std::string outFile,
              bool conOut);
    ~AudioLoad();

    // Get a non-modifiable buffer & data for processing.
    const std::vector<Real> &GetBuffer() const { return MonoBuffer; }
    const Pool &GetLoaderData() const { return audioInfo; }
    
    // Function for storing the attributes data in a file.
    void WriteToFile(); // For external storage.

    // Function to display calculated parameters on console.
    void projectData();
    void printPool();

};


#endif
