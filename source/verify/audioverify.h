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

#include "credits/libav.h"
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>
#include "standard/crosscorrelation.h"
#include "standard/autocorrelation.h"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

#ifndef AUDIOVERIFY_H
#define AUDIOVERIFY_H

class AudioVerify
{
  private:
    typedef const vector<Real> sourceSignal;
    typedef const vector<Real> sinkSignal;
    
    typedef vector<Real> sourceAutoCorrSeq;
    typedef vector<Real> sinkAutoCorrSeq;
    
    typedef vector<Real> audioCrossCorrSeq;
    Real audioSimilarityIndex;
    Real audioDelay;
    
    int audioExists = 0;
    Real gainFactor = 0.0;
    
    Real sourceRMS = 0.0;
    Real sinkRMS = 0.0;
    Real RMSMatch = 0.0;
    
  public:
    AudioVerify(vector<Real> source, vector<Real> sink);
    typedef vector<Real> CalcACFSeq(vector<Real> signalVector);
    typedef vector<Real> CalcCCFSeq(vector<Real> signalVector);
    Real CalcDelay(vector<Real> signalCCFSeq);
    int AudioExists(vector<Real> source, vector<Real> sink);
    Real CalcRMS(vector<Real> source);
    void SetProperties(vector<Real> source, vector<Real> sink);
};

#endif
