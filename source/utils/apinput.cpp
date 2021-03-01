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
 * File         : source/utils/input.cpp.
 * Description  : AudioPrism application input functions.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "apinput.h"

inputOpts::inputOpts(std::string spath, std::string tpath, bool split, 
		                 std::vector<std::string> files, bool scrout) : 
	                   srcpath(spath), snkpath(tpath), splitout(split), 
		                 outfiles(files), consoleout(scrout)
{
  int numfiles = files.size();
  
  if (numfiles >= 1)
  {
    if (files[0] != "")
    {
      mainout = true;
      moutfile = files[0];
    }
    else
    {
      mainout = false;
      moutfile = DEFAULT_OUTPUT;
    }

    if (split == true)
    {
      if (numfiles >= 2)
      {
        if (files[1] != "")
        {
          loaderout = true;
	        loutfile = files[1];
        }
        else
        {
          loaderout = false;
          loutfile = DEFAULT_LOUTPUT;
        }

        if (numfiles >= 3)
        {
          if (files[2] != "")
          {
            attrout = true;
	          aoutfile = files[2];
	        }
          else
          {
            attrout = false;
	          aoutfile = DEFAULT_AOUTPUT;
          }

          if (numfiles >= 4)
          {
            if (files[3] != "")
            {
              verifyout = true;
              voutfile = files[3];
            }
            else
            {
              verifyout = false;
              voutfile = DEFAULT_VOUTPUT;
            }

            if (numfiles == 5)
            {
              if (files[4] != "")
              {  
                defectout = true;
                doutfile = files[4];
              }
              else
              {
                defectout = false;
                doutfile = DEFAULT_DOUTPUT;
              }
            }
          }
        }
      }
    }
    else
    {
      loaderout = false;
      attrout = false;
      verifyout = false;
      defectout = false;

      loutfile = "";
      aoutfile = "";
      voutfile = "";
      doutfile = "";
    }
  }
  else
  {
    moutfile = "";
  }
}

inProps inputOpts::storeInConfig()
{
  inProps ic = {splitout, loaderout, attrout, verifyout, defectout, mainout, 
                consoleout, srcpath, snkpath, outfiles, loutfile, aoutfile, 
                voutfile, doutfile, moutfile};

  return ic;
}


void inputOpts::dispInputOpts()
{
  std::cout << "Source File Path : " << srcpath << std::endl;
  std::cout << "Sink File Path : " << snkpath << std::endl;
  std::cout << "Split Output Files : " << splitout << std::endl;
  std::cout << "Loader Output File : " << loaderout << std::endl;
  std::cout << "Attributes Output File : " << attrout << std::endl;
  std::cout << "Verifier Output File : " << verifyout << std::endl;
  std::cout << "Defects Output File : " << defectout << std::endl;
  std::cout << "Main Output File : " << mainout << std::endl;
  std::cout << "Console Output Enabled : " << consoleout << std::endl;
  std::cout << "Loader Output Filename : " << loutfile << std::endl;
  std::cout << "Attributes Output Filename : " << aoutfile << std::endl;
  std::cout << "Verifier Output Filename : " << voutfile << std::endl;
  std::cout << "Defect Output Filename : " << doutfile << std::endl;
  std::cout << "Main Output Filename : " << moutfile << std::endl;
}
