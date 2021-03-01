#include "apstdh.hpp"
#include "apinput.h"
#include "getoptions.h"

int main(int argc, char** argv)
{
  std::string app = "ap-all";

  // The following function processes, parses a set of user inputs and returns
  // an object that enables to store them individually to be accessed later.
  inputOpts inOpts = getOptions(argc, argv, app);
  
  // The following stores each user input in and as a struct data structure.
  inProps inConfig = inOpts.storeInConfig();

  // Display the values individually.
  inOpts.dispInputOpts();

  std::cout << "Split Output Files (From Struct) : " << inConfig.icf.sout << std::endl;
  std::cout << "Loader Output File (From Struct) : " << inConfig.icf.lout << std::endl;
  std::cout << "Verifier Output File (From Struct) : " << inConfig.icf.vout << std::endl;
  std::cout << "Defects Output File (From Struct) : " << inConfig.icf.dout << std::endl;
  std::cout << "Main Output File (From Struct) : " << inConfig.icf.mout << std::endl;
  std::cout << "Console Output Enabled (From Struct) : " << inConfig.icf.cout << std::endl;
  std::cout << "Source File Path (From Struct) : " << inConfig.icd.src << std::endl;
  std::cout << "Sink File Path (From Struct) : " << inConfig.icd.snk << std::endl;
  std::cout << "List of Files (From Struct) : ";
  std::vector<std::string> rfiles = inConfig.icd.ofiles;
  for (unsigned int i=0; i < rfiles.size(); i++)
  {
    std::cout << rfiles.at(i); 
    if (i < rfiles.size() - 1)
      std::cout << ",";
  }
  std::cout << std::endl;
  std::cout << "Loader Output Filename (From Struct) : " << inConfig.icd.lofile << std::endl;
  std::cout << "Verifier Output Filename (From Struct) : " << inConfig.icd.vofile << std::endl;
  std::cout << "Defect Output Filename (From Struct) : " << inConfig.icd.dofile << std::endl;
  std::cout << "Main Output Filename (From Struct) : " << inConfig.icd.mofile << std::endl;
   
  return 0;
}
