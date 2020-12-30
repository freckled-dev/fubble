#include "settings.hpp"
#include "fubble/utils/exception.hpp"
#include <boost/assert.hpp>
#include <boost/throw_exception.hpp>

struct invalid_audio_layer : utils::exception {};
using audio_layer_info =
    boost::error_info<struct invalid_audio_layer_tag, std::string>;

std::ostream &rtc::google::operator<<(std::ostream &out,
                                      settings::audio_layer print) {
  using audio_layer = settings::audio_layer;
  switch (print) {
  case audio_layer::default_:
    return out << "default";
  case audio_layer::windows_core:
    return out << "windows_core";
  case audio_layer::windows_core2:
    return out << "windows_core2";
  case audio_layer::linux_alsa:
    return out << "linux_alsa";
  case audio_layer::linux_pulse:
    return out << "linux_pulse";
  case audio_layer::android_java:
    return out << "android_java";
  case audio_layer::android_open_sles:
    return out << "android_open_sles";
  case audio_layer::android_java_input_and_open_sles_output:
    return out << "android_java_input_and_open_sles_output";
  case audio_layer::android_aaudio:
    return out << "android_aaudio";
  case audio_layer::android_java_input_and_aaudio_output:
    return out << "android_java_input_and_aaudio_output";
  case audio_layer::dummy:
    return out << "dummy";
  }
  BOOST_ASSERT(false);
  return out;
}

std::istream &rtc::google::operator>>(std::istream &in,
                                      settings::audio_layer &set) {
  using audio_layer = settings::audio_layer;
  std::string word;
  in >> word;
  if (word == "default")
    set = audio_layer::default_;
  else if (word == "windows_core")
    set = audio_layer::windows_core;
  else if (word == "windows_core2")
    set = audio_layer::windows_core2;
  else if (word == "linux_alsa")
    set = audio_layer::linux_alsa;
  else if (word == "linux_pulse")
    set = audio_layer::linux_pulse;
  else if (word == "android_java")
    set = audio_layer::android_java;
  else if (word == "android_open_sles")
    set = audio_layer::android_open_sles;
  else if (word == "android_java_input_and_open_sles_output")
    set = audio_layer::android_java_input_and_open_sles_output;
  else if (word == "android_aaudio")
    set = audio_layer::android_aaudio;
  else if (word == "android_java_input_and_aaudio_output")
    set = audio_layer::android_java_input_and_aaudio_output;
  else if (word == "dummy")
    set = audio_layer::dummy;
  else
    BOOST_THROW_EXCEPTION(invalid_audio_layer() << audio_layer_info(word));
  return in;
}
