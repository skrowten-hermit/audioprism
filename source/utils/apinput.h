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
 * File         : source/utils/input.h.
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



#ifndef INPUTOPTS_H
#define INPUTOPTS_H


#include "apstdh.hpp"
#include "constref.h"


typedef struct inConfigData
{
  std::string src, snk;
  std::vector<std::string> ofiles;
  std::string lofile, aofile, vofile, dofile, mofile;
} icData;

typedef struct inConfigFlags
{
  bool sout, lout, aout, vout, dout, mout, cout;
} icFlags;

typedef struct inConfProps
{ 
  icFlags icf;
  icData icd;
} inProps;


class inputOpts
{
  private:
    std::string srcpath = DEFAULT_INPUT;
    std::string snkpath = DEFAULT_INPUT;
    bool splitout = false;
    std::vector<std::string> outfiles;
    bool loaderout = false;
    bool attrout = false;
    bool verifyout = false;
    bool defectout = false;
    bool mainout = false;
    bool consoleout = false;
    std::string loutfile = DEFAULT_LOUTPUT;
    std::string aoutfile = DEFAULT_AOUTPUT;
    std::string voutfile = DEFAULT_VOUTPUT;
    std::string doutfile = DEFAULT_DOUTPUT;
    std::string moutfile = DEFAULT_OUTPUT;
  
  public:
    inputOpts(std::string spath, std::string tpath, bool split, 
              std::vector<std::string> files, bool scrout);
    inProps storeInConfig();
    void dispInputOpts();
};


#endif
