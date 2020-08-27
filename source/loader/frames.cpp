
#include "frames.h"

const char* FrameCount::name = "FrameCount";
const char* FrameCount::description = "Counts the number of frames in an input array";

void FrameCount::compute() {
    _count.get() = _array.get().size();
}
