
#include <iostream>
#include <essentia/algorithmfactory.h>
#include <essentia/algorithm.h>

#include "frames.h"

using namespace std;

int main(int argc, char **argv) {
  string filename = argv[1];
  cout << "processing file " << filename << endl;

  essentia::init();
  essentia::standard::AlgorithmFactory::Registrar<FrameCount> regFrameCount;


  essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();

  essentia::standard::Algorithm* audio         = factory.create("EasyLoader",
                                            "filename", filename,
                                            "sampleRate", 44100,
                                            "downmix", "mix");

  essentia::standard::Algorithm *count = factory.create("FrameCount");
  //essentia::standard::Algorithm *count = new FrameCount();

  std::vector<essentia::Real> audioBuffer;
  audio->output("audio").set(audioBuffer);
  count->input("array").set(audioBuffer);

  essentia::Real size;
  count->output("count").set(size);

  audio->compute();

  cout << "Count of item is " << size << endl;
}
