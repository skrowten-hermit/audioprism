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
 * File         : source/utils/output.cpp.
 * Description  : AudioPrism application output functions.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "apoutput.h"


void generateResults(Pool outputPool)
{
  std::vector<std::string> keys = outputPool.descriptorNames();
  std::map<std::string, std::vector<Real>> rvdescr = outputPool.getRealPool();
  std::map<std::string, Real> rsdescr = outputPool.getSingleRealPool();
  // std::map<std::string, std::vector<std::string>> svdescr = outputPool.getStringPool();
  std::map<std::string, std::vector<std::string>> svdescr = outputPool.getSingleVectorStringPool();
  std::map<std::string, std::string> ssdescr = outputPool.getSingleStringPool();
  
  std::cout << std::endl;
  std::cout << "Printing the results..." << std::endl;
  std::cout << std::endl;

  for(auto elem : rvdescr)
  {
    std::cout << elem.first << " --> " << elem.second << std::endl;
  }

  for(auto elem : rsdescr)
  {
    std::cout << elem.first << " --> " << elem.second << std::endl;
  }

  for(auto elem : svdescr)
  {
    std::cout << elem.first << " --> " << elem.second << std::endl;
  }

  for(auto elem : ssdescr)
  {
    std::cout << elem.first << " --> " << elem.second << std::endl;
  }

  for(auto elem : rvdescr)
  {
    std::cout << elem.first << " --> " << elem.second << std::endl;
  }
  
  std::cout << std::endl;
  std::cout << "The End of results..." << std::endl;
  std::cout << std::endl;
}
