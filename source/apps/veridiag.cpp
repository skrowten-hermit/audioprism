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
 * File         : source/apps/veridiag.cpp.
 * Description  : AudioPrism application for audio verification and diagnostics.
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
#include "audiodefects.h"


int main(int argc, char** argv)
{
  std::string app = "ap-all"; // Application name shorthand.
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
  std::vector<Real> sourceBuffer = srcLoader.GetBuffer();
  std::vector<Real> sinkBuffer = snkLoader.GetBuffer();
  Pool srcLPool = srcLoader.GetLoaderData();
  Pool snkLPool = snkLoader.GetLoaderData();
  if (inConfig.icf.lout == true)
  {
    srcLoader.WriteToFile();
    snkLoader.WriteToFile();
  }

  // Extract audio attributes from the buffers.
  AudioAttrs srcAttrs(stdAF, sourceBuffer, srcLPool, srcDescr, 
                      inConfig.icf.aout, inConfig.icf.cout);
  AudioAttrs snkAttrs(stdAF, sinkBuffer, snkLPool, snkDescr, inConfig.icf.aout,
                      inConfig.icf.cout);
  Pool sourceAttrs = srcAttrs.GetAttrData();
  Pool sinkAttrs = snkAttrs.GetAttrData();
  if (inConfig.icf.aout == true)
  {
    srcAttrs.WriteToFile();
    snkAttrs.WriteToFile();
  }

  // Look for and identify potential defects in the audio buffers.
  AudioBug srcDiagnose(stdAF, sourceBuffer, sourceAttrs, srcDescr, 
                       inConfig.icf.dout, inConfig.icf.cout);
  AudioBug snkDiagnose(stdAF, sinkBuffer, sinkAttrs, snkDescr, 
                       inConfig.icf.dout, inConfig.icf.cout);
  Pool sourceBugs = srcDiagnose.GetBugsData();
  Pool sinkBugs = snkDiagnose.GetBugsData();
  if (inConfig.icf.dout == true)
  {
    srcDiagnose.WriteToFile();
    snkDiagnose.WriteToFile();
  }

  // Verify the audio by comparing to the source buffer.
  AudioVerify validate(stdAF, sourceBuffer, sinkBuffer, 
                       sourceAttrs, sinkAttrs, srcDescr, snkDescr, 
                       inConfig.icd.vofile, inConfig.icf.vout, 
                       inConfig.icf.cout);
  Pool validateAudio = validate.GetValidationData();
  if (inConfig.icf.vout == true)
    validate.WriteToFile();

  // Collect all the computed parameters in a data structure.
  Pool gatherData;
  gatherData.merge(sourceAttrs);
  gatherData.merge(sinkAttrs);
  gatherData.merge(sourceBugs);
  gatherData.merge(sinkBugs);
  gatherData.merge(validateAudio);

  // Generate and display the final result.
  generateResults(gatherData);

  // Teardown the essentia library interface.
  essentia::shutdown();

  return 0;
}
