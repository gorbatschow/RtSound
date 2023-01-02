#pragma once
#include "RtSoundBaseGen.h"
#include <atomic>

class RtSoundToneGen : public RtSoundBaseGen {
public:
  RtSoundToneGen(int priority = 0);
  ~RtSoundToneGen() override;

  inline void setAmplitude(int percent) { _amplitude.exchange(percent); }
  inline int amplitude() { return _amplitude.load(); }

  inline void setFrequency(int percent) { _frequency.exchange(percent); }
  inline int frequency() const { return _frequency.load(); }

private:
  void applyStreamConfig(const RtSoundSetup &setup) override final;
  void generate(float *buffer, int nFrames, double t) override final;

  float _dt{}, _df{}, _fs{}, _nwin{};
  float _da{1.0f / 100.0f};
  std::atomic_int _amplitude{1};
  std::atomic_int _frequency{1};
};
