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
 * File         : source/apps/diagnostics.cpp.
 * Description  : AudioPrism application for only audio diagnostics.
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
#include "audiodefects.h"


int main(int argc, char** argv)
{
  std::string app = "ap-d"; // Application name shorthand.
  std::string srcDescr;

  /* The following function processes, parses a set of user inputs and returns
     an object that enables to store them individually to be accessed later. */
  inputOpts inOpts = getOptions(argc, argv, app);
  creditLibAV();
  
  /* The following stores, processes and display each user input in and as a 
     struct data structure. */
  inProps inConfig = inOpts.storeInConfig();

  if (inConfig.icf.cout == true)
    inOpts.dispInputOpts();

  srcDescr = getFileName(inConfig.icd.src);

  // Instantiate and initialize the essentia library interface.
  essentia::init();
  esstd::AlgorithmFactory& stdAF = esstd::AlgorithmFactory::instance();

  // Load audio stream to a buffer for processing.
  AudioLoad srcLoader(stdAF, inConfig.icd.src, inConfig.icd.lofile, srcDescr,
                      inConfig.icf.lout, inConfig.icf.cout);
  std::vector<Real> sourceBuffer = srcLoader.MonoBuffer;
  Pool srcLPool = srcLoader.loadPool;

  // Extract audio attributes from the stream.
  AudioAttrs srcAttrs(stdAF, sourceBuffer, srcLPool, srcDescr, 
                      inConfig.icf.aout, inConfig.icf.cout);
  Pool sourceAttrs = srcAttrs.audioAttrs;
  std::string adescr = srcAttrs.audioAttrs.value<std::string>("Description");
  Real samplerate = srcAttrs.audioAttrs.value<Real>("SampleRate");
  sourceAttrs.set("Description", adescr);
  sourceAttrs.set("SampleRate", samplerate);

  // Look for and identify potential defects in the audio stream.
  AudioBug srcDiagnose(stdAF, sourceBuffer, sourceAttrs, inConfig.icf.dout,
                       inConfig.icf.cout);
  Pool sourceBugs = srcDiagnose.defectsData;

  // Collect all the computed parameters in a data structure.
  Pool gatherData;
  gatherData.merge(sourceAttrs);
  gatherData.merge(sourceBugs);

  // Generate and display the final result.
  generateResults(gatherData);

  // Teardown the essentia library interface.
  essentia::shutdown();

  return 0;
}
