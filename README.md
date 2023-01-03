# RtSoundIO
The OOP-style wrapper for RtAudio

## Concept
The main idea is to implement observer pattern for audio stream related events, so this library contains a client base class `RtSoundClient` with relevant virtual methods. These events are the following:
- to receive audio devices list - `RtSoundClient::updateSoundDevices`
- to change audio stream configuraion - `RtSoundClient::configureStream`
- to receive audio stream configuration - `RtSoundClient::applyStreamConfig`
- to receive audio stream data - `RtSoundClient::streamDataReady`

## Usage
You can find a complete example here  ![RtSoundIODemo](https://github.com/gorbatschow/RtSoundIO).
In this example we want to make a signal (sine wave) generator and stream signal to sound card's output.
It could be someting like this:

```
class SineGenerator : public RtSoundClient {
  const int _channel{0};
  const float _frequency{1e3};
  const float _amplitude{0.5f};

  void configureStream(RtSoundSetup &setup) override {
    setup.setOutputEnabled(true);
    setup.setOutputChannels(1);
  }

  void streamDataReady(const RtSoundData &data) override {
    const auto dt = streamSetup().timeResolution();
    auto t{data.streamTime()};
    const auto buffer{data.outputBuffer(_channel)};
    for (int i = 0; i != data.framesN(); ++i) {
      buffer[i] = _amplitude * cos(2.0f * M_PI * _frequency * t);
      t += dt;
    }
  }
};
```
And we need to run it:
```
const auto gen{std::make_shared<SineGenerator>()};
auto io{RtSoundIO()};
io.addClient(gen);
io.startSoundStream();
std::this_thread::sleep_for(1s);
io.stopSoundStream();
```



