#include <Python.h>
#include <essentia/algorithmfactory.h>
#include "frames.h"

static PyObject* Essentia__myModule = NULL;

extern "C" void init_essentia_framecount() {
  Essentia__myModule = Py_InitModule3("_essentia_framecount", NULL,
                                                "A cool essentia extension");

  if (Essentia__myModule == NULL) {
    return;
  }

  essentia::standard::AlgorithmFactory::Registrar<FrameCount> regFrameCount;
}

