#include <iostream>
#include <cmath>
#include <complex>
#include <algorithm>
#include "credits/libav.h"
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include "standard/monomixer.h"
#include "standard/crosscorrelation.h"
#include "stats/variance.h"
#include <essentia/scheduler/network.h>

#define DEFAULT_INPUT "../../lib/essentia/test/audio/recorded/cat_purrrr.wav"
#define DEFAULT_OUTPUT "results.yaml"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

/*template<class T> 
struct tType;*/

//template <typename T> tType(T);

int main(int argc, char* argv[])
{
  // Initialize I/O variables.
  string audioFileSource = DEFAULT_INPUT;
  string outputFileName = "results.yaml";
  //int outfile = 0;
  
  cout << "Arguments : " << argc;

  if (argc < 2)
  {
    cout << "Error: incorrect number of arguments." << endl;
    cout << "Usage: " << argv[0] << " audio_input yaml_output[OPTIONAL]" << endl;
    creditLibAV();
    cout << "Using the default file : " << audioFileSource << endl;
  }
  else if (argc == 3)
  {
    audioFileSource = argv[1];
    outputFileName = argv[2];
  }
  else
  {
    audioFileSource = argv[1];
  }
  
  // Initialize audio data variables.
  vector<Real> monoVectorSource;
  vector<StereoSample> audioBufferSource;
  Real sourceSampleRate;
  int sourceChannels;
  int sourceBitRate;
  string sourcemd5sum;
  string sourceCodec;
  
  Pool lpool, dpool, pool;

  // Register the algorithm in the factory(ies).
  essentia::init();

  AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
  Algorithm* sourceLoader =  factory.create("AudioLoader",
                                            "filename", audioFileSource,
                                            "computeMD5", true);
  Algorithm* sourceMono = factory.create("MonoMixer",
                                         "type", "left");

  sourceLoader->output("audio").set(audioBufferSource);
  sourceLoader->output("sampleRate").set(sourceSampleRate);
  sourceLoader->output("numberChannels").set(sourceChannels);
  sourceLoader->output("md5").set(sourcemd5sum);
  sourceLoader->output("codec").set(sourceCodec);
  sourceLoader->output("bit_rate").set(sourceBitRate);
  
  sourceLoader->compute();
  
  //lpool.add("source.Data", audioBufferSource);
  lpool.add("source.Channels", sourceChannels);
  lpool.add("source.md5sum", sourcemd5sum);
  lpool.add("source.SampleRate", sourceSampleRate);
  lpool.add("source.BitRate", sourceBitRate);
  lpool.add("source.Codec", sourceCodec);

  sourceMono->input("audio").set(audioBufferSource);
  sourceMono->input("numberChannels").set(sourceChannels);
  sourceMono->output("audio").set(monoVectorSource);
  
  sourceMono->compute();

  lpool.add("source.MonoData", monoVectorSource);

  int sampleSize;
  int frameSize = 2048;
  int hopSize = 1024;
  sampleSize = monoVectorSource.size();
  
  vector<Real> CDframe, DDframe, GDframe, HDframe, NDframe;
  vector<Real> clickStartvec, clickEndvec;
  vector<Real> clickStarts, clickEnds;
  vector<Real> disLocvec, disAmplvec;
  vector<Real> gapStartvec, gapEndvec;
  vector<Real> gapStarts, gapEnds, gapLengths;
  vector<Real> humStartvec, humEndvec, humFreqvec, humSalvec;
  vector<Real> noiseburstIdxvec, noiseburstIdxs;
  TNT::Array2D<Real> outMatrix;

  Algorithm* fcC = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
  Algorithm* fcD = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
  Algorithm* fcG = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
  Algorithm* fcN = factory.create("FrameCutter",
                                  "frameSize", frameSize,
                                  "hopSize", hopSize);
 
  Algorithm* cdProb = factory.create("ClickDetector",
                                     //"detectionThreshold", 50,
                                     "frameSize", frameSize,
                                     "hopSize", hopSize,
                                     "sampleRate", sourceSampleRate);

  fcC->input("signal").set(monoVectorSource);
  fcC->output("frame").set(CDframe);
  
  cdProb->input("frame").set(CDframe); 
  cdProb->output("starts").set(clickStartvec);
  cdProb->output("ends").set(clickEndvec);

  while (true)
  {
    fcC->compute();

    if (!CDframe.size())
    {
      break;
    }
    
    //win->compute();
    cdProb->compute();
    
    clickStarts.insert(clickStarts.end(), clickStartvec.begin(), clickStartvec.end());
    clickEnds.insert(clickEnds.end(), clickEndvec.begin(), clickEndvec.end());
  }

  dpool.add("ClickDetector.Start", clickStarts);
  dpool.add("ClickDetector.End", clickEnds);

  Algorithm* ddProb = factory.create("DiscontinuityDetector",
                                     "frameSize", frameSize,
				     "hopSize", hopSize);

  fcD->input("signal").set(monoVectorSource);
  fcD->output("frame").set(DDframe);
  
  ddProb->input("frame").set(DDframe); 
  ddProb->output("discontinuityLocations").set(disLocvec);
  ddProb->output("discontinuityAmplitudes").set(disAmplvec);

  while (true)
  {
    fcD->compute();

    if (!DDframe.size())
    {
      break;
    }
    
    //win->compute();
    ddProb->compute();

    dpool.add("DiscontinuityDetector.Locations", disLocvec);
    dpool.add("DiscontinuityDetector.Amplitudes", disAmplvec);
  }

  Algorithm* gdProb = factory.create("GapsDetector",
                                     "frameSize", frameSize,
                                     "hopSize", hopSize,
                                     "sampleRate", sourceSampleRate);

  fcG->input("signal").set(monoVectorSource);
  fcG->output("frame").set(GDframe);
  
  gdProb->input("frame").set(GDframe); 
  gdProb->output("starts").set(gapStartvec);
  gdProb->output("ends").set(gapEndvec);

  while (true)
  {
    fcG->compute();

    if (!GDframe.size())
    {
      break;
    }
    
    //win->compute();
    gdProb->compute();

    gapStarts.insert(gapStarts.end(), gapStartvec.begin(), gapStartvec.end());
    gapEnds.insert(gapEnds.end(), gapEndvec.begin(), gapEndvec.end());
  }

  dpool.add("GapsDetector.Start", gapStarts);
  dpool.add("GapsDetector.End", gapEnds);

  Algorithm* hdProb = factory.create("HumDetector",
                                     "frameSize", frameSize,
                                     "hopSize", hopSize,
                                     "sampleRate", sourceSampleRate);

  hdProb->input("signal").set(monoVectorSource); 
  hdProb->output("frequencies").set(humFreqvec);
  hdProb->output("saliences").set(humSalvec);
  hdProb->output("starts").set(humStartvec);
  hdProb->output("ends").set(humEndvec);
  hdProb->output("r").set(outMatrix);

  hdProb->compute();

  dpool.add("HumDetector.Frequencies", humFreqvec);
  dpool.add("HumDetector.Saliences", humSalvec);
  dpool.add("HumDetector.Starts", humStartvec);
  dpool.add("HumDetector.Ends", humEndvec);

  Algorithm* ndProb = factory.create("NoiseBurstDetector");

  fcN->input("signal").set(monoVectorSource);
  fcN->output("frame").set(NDframe);
  
  ndProb->input("frame").set(NDframe); 
  ndProb->output("indexes").set(noiseburstIdxvec);

  while (true)
  {
    fcN->compute();

    if (!NDframe.size())
    {
      break;
    }
    
    //win->compute();
    //iseburstIdxvec
    ndProb->compute();

    noiseburstIdxs.insert(noiseburstIdxs.end(), noiseburstIdxvec.begin(), noiseburstIdxvec.end());
  }

  dpool.add("NoiseBurstDetector.Indexes", noiseburstIdxs);

  pool.merge(lpool);
  pool.merge(dpool);

  vector<string> keys = pool.descriptorNames();
  map<string, vector<Real>> rvdescr = pool.getRealPool();
  map<string, Real> rsdescr = pool.getSingleRealPool();
  map<string, vector<string>> svdescr = pool.getStringPool();
  map<string, string> ssdescr = pool.getSingleStringPool();

  // cout << rvdescr << endl;
  // cout << rsdescr << endl;
  // cout << svdescr << endl; 
  // cout << ssdescr << endl;

  for(auto elem : rvdescr)
  {
    cout << elem.first << " --> " << elem.second << endl;
  }

  for(auto elem : rsdescr)
  {
    cout << elem.first << " --> " << elem.second << endl;
  }

  for(auto elem : svdescr)
  {
    cout << elem.first << " --> " << elem.second << endl;
  }

  for(auto elem : ssdescr)
  {
    cout << elem.first << " --> " << elem.second << endl;
  }

  for(auto elem : rvdescr)
  {
    cout << elem.first << " --> " << elem.second << endl;
  }

  for(vector<string>::iterator i = begin(keys); i != end(keys); ++i)
  {
    cout << *i << endl;
    //cout << *i << " --> " << pool.value<string>(*i) << endl;
    //cout << *i << " --> " << pool.value(*i) << endl;
    //cout << *i << " --> " << (string)pool.value(*i) << endl;
    //cout << *i << " --> " << pool.value<string>(*i) << endl;
  }

  /* std::map<std::string, std::vector<std::string>> keyvals;
  std::map<std::string, std::vector<Real>> keyvalr;
  //map<string, vector>::iterator i;

  keyvals = pool.getStringPool();
  keyvalr = pool.getRealPool();

  for(map<string, vector<string>>::iterator i = keyvals.begin(); i != keyvals.end(); ++i)
  {
    cout << i->first << " --> " << i->second << endl;
  }

  for(map<string, vector<string>>::iterator i = keyvalr.begin(); i != keyvalr.end(); ++i)
  {
    cout << i->first << " --> " << i->second << endl;
  } */

  cout << "Size of mono audio data of source : " << sampleSize << endl;
  //cout << "FFT output : " << fftSource << endl;
  cout << "Size of ClickStarts vector : " << clickStarts.size() << endl;
  cout << "Size of ClickEnds vector : " << clickEnds.size() << endl;
  cout << "Size of GapStarts vector : " << gapStarts.size() << endl;
  cout << "Size of GapEnds vector : " << gapEnds.size() << endl;
  cout << "Size of NoiseBurst indexes : " << noiseburstIdxs.size() << endl;
  
  // write results to file
  cout << "-------- writing results to file " << outputFileName << " --------" << endl;
    
  standard::Algorithm* output = standard::AlgorithmFactory::create("YamlOutput",
                                                                   "filename", outputFileName);
    
  output->input("pool").set(pool);
  output->compute();
    
  delete output;

  essentia::shutdown();

  return 0;
}
