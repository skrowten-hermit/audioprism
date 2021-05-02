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
 * File         : source/utils/boxheader.cpp.
 * Description  : Function to print a header.
 * Created On   : 25-03-2021.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 25-03-2021.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "boxheader.h"

// Create a box with a passed string at the center.
void boxheader(std::string header)
{
  int height = 5;
  int width = 80;
  int it, in, diff;
  int len = header.length();
  int fh, sh;

  it = width;
  while (it --> 0) putchar('*'); putchar('\n');

  in = width - 2;
  diff = in - len;
  fh = diff/2;
  sh = diff - fh;
  putchar('*'); 
  while(in --> 0) putchar(' ');
  putchar('*'); putchar('\n');
  putchar('*');
  while(fh --> 0) putchar(' ');
  std::cout << header;
  while(sh --> 0) putchar(' ');
  putchar('*'); putchar('\n');
  putchar('*'); 
  in = width - 2;
  while(in --> 0) putchar(' ');
  putchar('*'); putchar('\n');

  it = width;
  while (it --> 0) putchar('*'); putchar('\n');
}
