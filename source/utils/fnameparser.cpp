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
 * File         : source/utils/fnameparser.cpp.
 * Description  : A file name parser from it's full path.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "fnameparser.h"


string getFileName(const string& s) 
{
  size_t i = s.rfind(SEP, s.length());

  if (i != string::npos) 
    return(s.substr(i + 1, s.length() - i));
  else
    return(s);
}


string getFileDescr(const string& s)
{
  const string fnwithextn = getFileName(s);
  size_t i = fnwithextn.rfind('.', fnwithextn.length());

  if (i != string::npos)
    return(fnwithextn.substr(0, i));
  else
    return(fnwithextn);
}

/*
int main(int argc, char** argv) 
{
   string path = argv[1];

   std::cout << "The file name is \"" << getFileName(path) << "\"\n";
   std::cout << "The file name (without extension) is \"" << getFileDescr(path) << "\"\n";

   return 0;
}
*/
