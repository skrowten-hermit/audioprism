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
  std::string srcDescr, srcFileName;
  std::string snkDescr, snkFileName;

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
  srcFileName = getFileName(inConfig.icd.src);
  srcDescr = getFileDescr(inConfig.icd.src);
  snkFileName = getFileName(inConfig.icd.snk);
  snkDescr = getFileDescr(inConfig.icd.snk);

  // Instantiate and initialize the essentia library interface.
  essentia::init();
  esstd::AlgorithmFactory& stdAF = esstd::AlgorithmFactory::instance();

  // Load audio streams to buffers for processing.
  AudioLoad srcLoader(stdAF, inConfig.icd.src, srcDescr, inConfig.icf.lout, 
                      inConfig.icd.lofile, inConfig.icf.cout);
  AudioLoad snkLoader(stdAF, inConfig.icd.snk, snkDescr, inConfig.icf.lout, 
                      inConfig.icd.lofile, inConfig.icf.cout);
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
                      inConfig.icf.aout, inConfig.icd.aofile, 
                      inConfig.icf.cout);
  AudioAttrs snkAttrs(stdAF, sinkBuffer, snkLPool, snkDescr, 
                      inConfig.icf.aout, inConfig.icd.aofile, 
                      inConfig.icf.cout);
  Pool sourceAttrs = srcAttrs.GetAttrData();
  Pool sinkAttrs = snkAttrs.GetAttrData();
  if (inConfig.icf.aout == true)
  {
    srcAttrs.WriteToFile();
    snkAttrs.WriteToFile();
  }

  // Verify the audio by comparing to the source buffer.
  AudioVerify validate(stdAF, sourceBuffer, sinkBuffer, 
                       sourceAttrs, sinkAttrs, srcDescr, snkDescr, 
                       inConfig.icf.vout, inConfig.icd.vofile, 
                       inConfig.icf.cout);
  Pool validateAudio = validate.GetValidationData();
  if (inConfig.icf.vout == true)
    validate.WriteToFile();

  // Collect all the computed parameters in a data structure.
  Pool gatherData;
  gatherData.merge(sourceAttrs);
  gatherData.merge(sinkAttrs);
  gatherData.merge(validateAudio);

  // Generate and display the final result.
  generateResults(gatherData);

  // Generate the output result file.
  if (inConfig.icf.mout == true)
  {
    std::cout << "-------- writing results to file " << inConfig.icd.mofile 
              << " --------" << std::endl;
              
    esstd::Algorithm* Output;
    Output = stdAF.create("YamlOutput", "filename", inConfig.icd.mofile);
    Output->input("pool").set(gatherData);
    Output->compute();
    delete Output;
    
    std::cout << "Output written successfully...." << std::endl;
  }

  // Teardown the essentia library interface.
  essentia::shutdown();

  return 0;
}
