#include "RtSoundBaseGen.h"
#include <cstring>
#include <mutex>

RtSoundBaseGen::RtSoundBaseGen(int priority) : RtSoundClient(priority) {}

void RtSoundBaseGen::streamDataReady(const RtSoundData &data) {
  assert(_inputChannel >= 0);
  assert(_outputChannel >= 0);

  if (_inputChanged && data.inputsN() > 0) {
    std::lock_guard lock(data.mutex);
    data.nullifyInputBuffer();
    _inputChanged = false;
  }
  if (_inputEnabled && data.inputsN() > 0 && data.inputsN() > _inputChannel) {
    std::lock_guard lock(data.mutex);
    generate(data.inputBuffer(_inputChannel), data.framesN(),
             data.streamTime());
  }

  if (_outputChanged && data.outputsN() > 0) {
    std::lock_guard lock(data.mutex);
    data.nullifyOutputBuffer();
    _outputChanged = false;
  }

  if (_outputEnabled && data.outputsN() > 0 &&
      data.outputsN() > _outputChannel) {
    std::lock_guard lock(data.mutex);
    generate(data.outputBuffer(_outputChannel), data.framesN(),
             data.streamTime());
  }
}
