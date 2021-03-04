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
 * File         : source/utils/getoptions.cpp.
 * Description  : Functions to store and map user inputs and options.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 28-11-2020.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : File header, description added.
 * 
 *******************************************************************************
 * 
 */


#include "cxxopts.hpp"
#include "getoptions.h"
#include "libav.h"

inputOpts getOptions(int argcount, char** argvals, std::string appl)
{
  int type = 0;
  std::string appname = "";
  std::string appdescr = "";
  
  if (appl == "ap-all")
  {
    appname = "audioprism"; 
    appdescr = "AudioPrism: Audio/Speech file data verification and diagnosis.";
  }
  else if (appl == "ap-v")
  {
    type = 1;
    appname = "audioprism-v";
    appdescr = "AudioPrism: Audio/Speech file data verification.";
  }
  else if (appl == "ap-d")
  {
    type = 2;
    appname = "audioprism-d";
    appdescr = "AudioPrism: Audio/Speech file data diagnosis.";
  }

  cxxopts::Options options(appname, appdescr);

  if (type == 0 || type == 1)
  {
    options.add_options()
        ("s,source", "Source input file", 
                     cxxopts::value<std::string>())
        ("t,sink", "Sink input file", 
                   cxxopts::value<std::string>())
        //("d,debug", "Enable debugging") // a bool parameter
        ("p,splitoutput", "Split the output to separate files", 
                          cxxopts::value<bool>()->default_value("false"))
        ("o,outfile", "Output file(s), separated by \',\' if multiple output "
                      "files (only if -p or --splitoutput is set) needed. The "
                      "order is - main, loader, attributes, verify, defects.", 
                      cxxopts::value<std::vector<std::string>>()->implicit_value(""))
        ("v,verbose", "Verbose output", 
                      cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Print usage")
    ;
  }
  else
  {
    options.add_options()
        ("s,source", "Source input file", 
                     cxxopts::value<std::string>())
        //("d,debug", "Enable debugging") // a bool parameter
        ("p,splitoutput", "Split the output to separate files", 
                          cxxopts::value<bool>()->default_value("false"))
        ("o,outfile", "Output file(s), separated by \',\' if multiple output "
                      "files (only if -p or --splitoutput is set) needed. The "
                      "order is - main, loader, attributes, verify, defects.", 
                      cxxopts::value<std::vector<std::string>>()->implicit_value(""))
        ("v,verbose", "Verbose output", 
                      cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Print usage")
    ;
  }

  auto result = options.parse(argcount, argvals);
  
  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    creditLibAV();
    exit(0);
  }
  
  std::string s_path = "";
  if (result.count("source"))
    s_path = result["source"].as<std::string>();
  else
    s_path = "N/A";
  
  std::string t_path = "";
  if (type == 0 || type == 1)
    if (result.count("sink"))
      t_path = result["sink"].as<std::string>();
    else
      t_path = "N/A";
  else
    t_path = "N/A";

  bool spval = false;
  if (result.count("splitoutput"))
    spval = result["splitoutput"].as<bool>();
  else
    spval = false;
  
  std::vector<std::string> o_files;
  if (result.count("outfile"))
    o_files = result["outfile"].as<std::vector<std::string>>();
  //else
  //  o_files.push_back("");
    

  bool con_out = false;
  if (result.count("verbose"))
    con_out = result["verbose"].as<bool>();
  else
    con_out = false;

  inputOpts in_opts(s_path, t_path, spval, o_files, con_out);
  
  //bool debug = result["debug"].as<bool>();
 
  return in_opts;
}
