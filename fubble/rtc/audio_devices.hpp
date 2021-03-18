#ifndef UUID_D55026B6_63E0_430F_BA8E_457E97F73B5A
#define UUID_D55026B6_63E0_430F_BA8E_457E97F73B5A

#include <string>
#include <vector>

namespace rtc {
class audio_devices {
public:
  virtual ~audio_devices() = default;
  virtual void enumerate() = 0;
  struct device {
    int index{};
    std::string name;
    bool operator==(const device &other) const {
      return other.index == index && other.name == name;
    }
  };
  using devices = std::vector<device>;
  virtual devices get_playout_devices() const = 0;
  virtual devices get_recording_devices() const = 0;
  virtual void set_recording_device(int id) = 0;
  virtual void set_output_device(int id) = 0;

  virtual void mute_speaker(const bool mute) = 0;
  virtual bool is_speaker_muted() = 0;
  virtual void mute_microphone(const bool mute) = 0;
  virtual bool is_microphone_muted() = 0;
};

} // namespace rtc

#endif
