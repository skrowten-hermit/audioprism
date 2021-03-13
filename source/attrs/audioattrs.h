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
 * File         : source/attrs/audioattrs.h.
 * Description  : Functions to extract audio stream attributes/information.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */



#ifndef AUDIOATTRS_H
#define AUDIOATTRS_H


#include "apstdh.hpp"
#include "apessh.hpp"
#include "constref.h"


class AudioAttrs
{
  private:
    std::string outputFile = DEFAULT_AOUTPUT;
    int saveOutput = 0;
    bool consoleOut = false;

    esstd::AlgorithmFactory& AF;

    esstd::Algorithm* acfAlgo;
    esstd::Algorithm* fftAlgo;
    esstd::Algorithm* fbandsAlgo;
    esstd::Algorithm* fcAlgo;
    esstd::Algorithm* psdAlgo;
    esstd::Algorithm* rmsAlgo;
    esstd::Algorithm* snrAlgo;
    esstd::Algorithm* loudAlgo;
    esstd::Algorithm* meanAlgo;
    esstd::Algorithm* Output;

    std::vector<Real> signalVector, signalVectorWODC , signalAutoCorr;
    std::vector<Real> signalFFT, signalFreqBandEnergy, signalPSD;
    std::vector<int> FreqBands {0, 50, 100, 150, 200, 300, 400, 510, 
                                630, 770, 920, 1080, 1270, 1480, 1720,
                                2000, 2320, 2700, 3150, 3700, 4400, 
                                5300, 6400, 7700, 9500, 12000, 15500, 
                                20500, 27000};
    std::vector<std::string> FreqBandEnergy; 
    
    long unsigned int sigACFSize = 0, sigPSDSize = 0, sigFFTSize = 0;
    long unsigned int numFreqBands = 0;
    
    Real SampleRate;
    Real Channels;
    Real BitRate;
    Real SampleSize;
    std::string md5sum;
    std::string Codec;
    
    Real signalF0, signalRMS, signalSNR, signalLoudness;
    Real signalDCOffset, signalRMSWODC;

    std::string fileTag;

    // Functions for processing data of the attribute class.
    std::vector<Real> CalcACFSeq(); // Get the auto-correlation function
                                    // sequence of the signal.
    Real GetF0Freq(); // Get the fundamental frequency F0.
    std::vector<Real> GetFreqComponents(); // Get the corresponding 
                                           // energies of the above 
                                           // frequency bands.
    std::vector<Real> CalcPSD(); // Get the Power Spectral Density - 
                                 // to be derived from ACF.
    Real CalcRMS(); // Get the Root Mean Square of the audio signal.
    Real CalcSNR(); // Get the signal-to-Noise Ratio of the audio signal.
    Real CalcLoudness(); // Get the loudness of the audio signal.
    std::vector<Real> MinusDCComp(); // Remove DC component from the signal.
    
    // Functions for storing the attributes data in a pool data structure.
    Pool StoreAttrs();  // For internal storage.
    Pool attrsData;
    
  public:
    // Initialization and de-initialization of the attribute class.
    AudioAttrs(esstd::AlgorithmFactory& saf, 
               std::vector<Real> audioSigVector, Pool attrPool, 
               std::string description, int saveData, std::string outFile, 
               bool conOut);
    ~AudioAttrs();

    // Get a non-modifiable data for processing.
    const Pool &GetAttrData() const { return attrsData; }

    // Function to display calculated parameters on console.
    void WriteToFile(); // For external storage.

    // Function to display calculated parameters on console.
    void projectData();
    void printPool();
    
};


#endif
