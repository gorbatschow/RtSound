# RtSoundIO
OOP-style wrapper for RtAudio

## Concept & usage
The main idea is to implement observer pattern for audio stream related events, so this library contains a client base class `RtSoundClient` with relevant virtual methods. These events are the following:
- to receive audio devices list - `RtSoundClient::updateSoundDevices`
- to change audio stream configuraion - `RtSoundClient::configureStream`
- to receive audio stream configuration - `RtSoundClient::applyStreamConfig`
- to receive audio stream data - `RtSoundClient::streamDataReady`

To control the sound stream (start, stop, etc) `RtSoundIO` is used.
