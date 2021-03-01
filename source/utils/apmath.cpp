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
 * File         : source/utils/apmath.cpp.
 * Description  : AudioPrism's utility math operation functions.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "apmath.h"

std::tuple <std::vector<Real>, std::vector<Real>> 
comp2padd(std::vector<Real> v1, std::vector<Real> v2)
{
  int v1Size = v1.size();
  int v2Size = v2.size();

  if (v2Size > v1Size)
  {
    v1.resize(v1.size() + (v2Size - v1Size), 0);
  }
  else if (v2Size < v1Size)
  {
    v2.resize(v2.size() + (v1Size - v2Size), 0);
  }

  return make_tuple(v1, v2);
}
