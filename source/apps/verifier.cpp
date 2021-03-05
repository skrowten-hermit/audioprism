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
 * File         : source/apps/verifier.cpp.
 * Description  : AudioPrism application for only audio verification.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "apstdh.hpp"
#include "apessh.hpp"

#include "libav.h"
#include "getoptions.h"
#include "fnameparser.h"
#include "apoutput.h"

#include "audioloader.h"
#include "audioattrs.h"
#include "audioverify.h"


int main(int argc, char** argv)
{
  std::string app = "ap-v"; // Application name shorthand.
  std::string srcDescr;
  std::string snkDescr;

  /* The following function processes, parses a set of user inputs and returns
     an object that enables to store them individually to be accessed later. */
  inputOpts inOpts = getOptions(argc, argv, app);
  creditLibAV();
  
  /* The following stores, processes and display each user input in and as a 
     struct data structure. */
  inProps inConfig = inOpts.storeInConfig();
  if (inConfig.icf.cout == true)
    inOpts.dispInputOpts();

  // Extract filenames from paths.
  srcDescr = getFileName(inConfig.icd.src);
  snkDescr = getFileName(inConfig.icd.snk);

  // Instantiate and initialize the essentia library interface.
  essentia::init();
  esstd::AlgorithmFactory& stdAF = esstd::AlgorithmFactory::instance();

  // Load audio streams to buffers for processing.
  AudioLoad srcLoader(stdAF, inConfig.icd.src, inConfig.icd.lofile, srcDescr,
                      inConfig.icf.lout, inConfig.icf.cout);
  AudioLoad snkLoader(stdAF, inConfig.icd.snk, inConfig.icd.lofile, snkDescr,
                      inConfig.icf.lout, inConfig.icf.cout);
  std::vector<Real> sourceBuffer = srcLoader.MonoBuffer;
  std::vector<Real> sinkBuffer = snkLoader.MonoBuffer;
  Pool srcLPool = srcLoader.loadPool;
  Pool snkLPool = snkLoader.loadPool;

  // Extract audio attributes from the buffers.
  AudioAttrs srcAttrs(stdAF, sourceBuffer, srcLPool, srcDescr, 
                      inConfig.icf.aout, inConfig.icf.cout);
  AudioAttrs snkAttrs(stdAF, sinkBuffer, snkLPool, snkDescr, inConfig.icf.aout, 
                      inConfig.icf.cout);
  Pool sourceAttrs = srcAttrs.audioAttrs;
  std::string srcdescr = srcAttrs.audioAttrs.value<std::string>("Description");
  Real srcsamplerate = srcAttrs.audioAttrs.value<Real>("SampleRate");
  sourceAttrs.set("Description", srcdescr);
  sourceAttrs.set("SampleRate", srcsamplerate);
  Pool sinkAttrs = snkAttrs.audioAttrs;
  std::string snkdescr = snkAttrs.audioAttrs.value<std::string>("Description");
  Real snksamplerate = snkAttrs.audioAttrs.value<Real>("SampleRate");
  sinkAttrs.set("Description", snkdescr);
  sinkAttrs.set("SampleRate", snksamplerate);

  // Verify the audio by comparing to the source buffer.
  AudioVerify validate(stdAF, sourceBuffer, sinkBuffer, sourceAttrs, sinkAttrs, 
                       inConfig.icd.vofile, inConfig.icf.vout, 
                       inConfig.icf.cout);

  Pool srcAttrsOut = srcAttrs.StoreAttrs(srcdescr, inConfig.icf.aout);
  Pool snkAttrsOut = snkAttrs.StoreAttrs(snkdescr, inConfig.icf.aout);
  Pool verifierOut = validate.SetVerifyData(srcdescr, snkdescr, inConfig.icf.vout);

  Pool gatherData;

  gatherData.merge(srcAttrsOut);
  gatherData.merge(snkAttrsOut);
  gatherData.merge(verifierOut);

  generateResults(gatherData);

  essentia::shutdown();

  return 0;
}
