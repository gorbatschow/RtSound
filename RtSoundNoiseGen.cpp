#include "RtSoundNoiseGen.h"
#include <chrono>
#include <random>

RtSoundNoiseGen::RtSoundNoiseGen() {}

void RtSoundNoiseGen::beforeStartStream(const RtSoundSetup &setup) {}

void RtSoundNoiseGen::receiveStreamData(const RtStreamData &streamData) {
  std::default_random_engine gen;
  auto distr{std::bind(std::normal_distribution<float>{0.0f, 0.3f},
                       std::mt19937(std::random_device{}()))};
}
