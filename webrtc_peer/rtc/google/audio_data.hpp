#ifndef UUID_DEA725DC_62AE_4208_BCCA_0D803C8A1267
#define UUID_DEA725DC_62AE_4208_BCCA_0D803C8A1267

namespace rtc::google {
struct audio_data {
  const void *audio_data;
  int bits_per_sample;
  int sample_rate;
  int number_of_channels;
  int number_of_frames;
};
} // namespace rtc::google

#endif
