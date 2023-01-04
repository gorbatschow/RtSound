#include "RtSoundNoiseGen.h"
#include <chrono>
#include <random>

void RtSoundNoiseGen::applyStreamConfig(const RtSoundSetup &setup) {}

void RtSoundNoiseGen::generate(float *buffer, int nFrames, double t) {
  const float A{float(_amplitude.load()) * _da};
  for (int i = 0; i != nFrames; ++i) {
    std::normal_distribution<float> val(0.0f, A);
    buffer[i] = val(_rgen);
  }
}
