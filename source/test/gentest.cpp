#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;


int main()
{
  map<string, float> energies;
  string key;
  vector<int> FreqBands {0, 50, 100, 150, 200, 300, 400, 510, 
                         630, 770, 920, 1080, 1270, 1480, 1720,
                         2000, 2320, 2700, 3150, 3700, 4400, 
                         5300, 6400, 7700, 9500, 12000, 15500, 
                         20500, 27000};
  vector<float> evals {0.00768436, 0.446009, 248.12, 505.001, 19609.6, 
                       35825.6, 211410, 54585.7, 8338.55, 10951.4, 116563, 
                       22817.2, 78838, 24598.1, 44082.6, 65265.6, 75600.8,
                       116484, 21149.7, 28.5852, 0, 0, 0, 0, 0, 0, 0, 0};
  vector<string> BandEnergies;

  for (unsigned int i=0; i < FreqBands.size() - 1; i++)
  {
    string currBandEnergy;
    string val(25, '\0');
    string key = to_string(FreqBands[i]) + "-" + to_string(FreqBands[i+1]);
    auto vals = snprintf(&val[0], val.size(), "%.20f", evals[i]);
    val.resize(vals);
    if (i != FreqBands.size() - 2)
      currBandEnergy = key + " : " + val + ", ";
    else
      currBandEnergy = key + " : " + val;
    BandEnergies.push_back(currBandEnergy);
  }

  for (auto element : BandEnergies)
    cout << element;
  
  cout << endl;

  return 0;
}