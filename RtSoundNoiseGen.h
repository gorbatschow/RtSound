#pragma once
#include "RtSoundClient.h"

class RtSoundNoiseGen : public RtSoundClient {

public:
  RtSoundNoiseGen();

protected:
  virtual void beforeStartStream(const RtSoundSetup &setup) override;
  virtual void receiveStreamData(const RtStreamData &streamData) override;
};
