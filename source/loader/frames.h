#ifndef FRAMES_H
#define FRAMES_H

#include <essentia/algorithm.h>

using essentia::standard::Algorithm;
using essentia::standard::Input;
using essentia::standard::Output;
using essentia::Real;

class FrameCount : public Algorithm {

    private:
        Input<std::vector<Real> > _array;
        Output<Real> _count;
    public:
        FrameCount() {
            declareInput(_array, "array", "an input array");
            declareOutput(_count, "count", "the number of elements in the input array");
        }

    void compute();
    void declareParameters() {}

    static const char* name;
    static const char* description;

};


#endif /* FRAMES_H */
