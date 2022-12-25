#pragma once
#include "RtSoundSetup.h"
#include "RtSoundStream.h"
#include <memory>
#include <mutex>
#include <vector>
class RtSoundClient;

class RtSoundProvider {
public:
  RtSoundProvider();
  ~RtSoundProvider() {}

  void addClient(std::weak_ptr<RtSoundClient> client);

protected:
  void checkClients();
  void orderClients();
  void notifyConfigureStream(RtSoundSetup &setup);
  void notifyBeforeStartStream(const RtSoundSetup &setup);
  void notifyReceiveStreamData(const RtStreamData &data);

private:
  std::vector<std::weak_ptr<RtSoundClient>> _clients;
};
