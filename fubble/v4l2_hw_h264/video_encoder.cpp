#include "video_encoder.hpp"
#include "reader.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <common_video/h264/h264_bitstream_parser.h>
#include <common_video/h264/h264_common.h>
#include <fubble/rtc/logger.hpp>
#include <modules/video_coding/include/video_codec_interface.h>
#include <modules/video_coding/include/video_error_codes.h>
#include <system_wrappers/include/clock.h>

using namespace fubble::v4l2_hw_h264;

namespace {
class video_encoder_impl : public video_encoder {
public:
  webrtc::EncodedImageCallback *callback{};
  webrtc::Clock *const clock_{webrtc::Clock::GetRealTimeClock()};
  std::unique_ptr<reader> reader_;
  boost::signals2::scoped_connection on_data_connection;
  bool got_first_iframe{};

  video_encoder_impl(std::unique_ptr<reader> reader_move)
      : reader_{std::move(reader_move)} {
    BOOST_ASSERT(reader_);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
    on_data_connection = reader_->on_data.connect(
        [this](auto data, auto size) { on_data(data, size); });
  }

  ~video_encoder_impl() {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", this:" << this;
  }

  void SetFecControllerOverride(
      webrtc::FecControllerOverride *fec_controller_override) override {
    (void)fec_controller_override;
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  }

  int InitEncode(const webrtc::VideoCodec *codec_settings,
                 const VideoEncoder::Settings &settings) override {
    (void)settings;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", codec_settings->width: " << codec_settings->width;
    try {
      reader_->initialise(codec_settings->startBitrate);
    } catch (const boost::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__ << ", " << boost::diagnostic_information(error);
      return WEBRTC_VIDEO_CODEC_ERROR;
    }
    return WEBRTC_VIDEO_CODEC_OK;
  }

  bool is_h264_i_frame(const std::uint8_t *packet) {
    // https://stackoverflow.com/questions/1957427/detect-mpeg4-h264-i-frame-idr-in-rtp-stream
    int RTPHeaderBytes = 4;

    int fragment_type = packet[RTPHeaderBytes + 0] & 0x1F;
    int nal_type = packet[RTPHeaderBytes + 1] & 0x1F;
    int start_bit = packet[RTPHeaderBytes + 1] & 0x80;

    bool result{};
    if (((fragment_type == 28 || fragment_type == 29) && nal_type == 5 &&
         start_bit == 128) ||
        fragment_type == 5) {
      result = true;
    }

#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << std::hex << ", 0x" << int(packet[0]) << ", 0x"
        << int(packet[1]) << ", 0x" << int(packet[2]) << ", 0x"
        << int(packet[3]) << ", 0x" << int(packet[4]) << ", 0x"
        << int(packet[5]) << ", 0x" << int(packet[6]) << ", 0x"
        << int(packet[7]) << ", 0x" << int(packet[8]) << ", 0x"
        << int(packet[9]) << std::dec << ", result: " << result;
#endif

    // hack! the above code didn't get the iframe. check what the next line is
    // actually doiing!
    // checkout https://www.cardinalpeak.com/the-h-264-sequence-parameter-set
    // and https://en.wikipedia.org/wiki/Network_Abstraction_Layer
    if (int(packet[4]) == 0x27) {
#if 0
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << " hack";
#endif
      return true;
    }

    return result;
  }

  void on_data(const void *data, int size) {
#if 0
    BOOST_LOG_SEV(logger, logging::severity::trace)
        << __FUNCTION__ << ", size:" << size;
#endif
    // TODO here we r using a deprecated method
    webrtc::EncodedImage encoded{
        static_cast<std::uint8_t *>(const_cast<void *>(data)),
        static_cast<std::size_t>(size), static_cast<std::size_t>(size)};
    webrtc::CodecSpecificInfo info;
    webrtc::CodecSpecificInfoH264 codec_info;
    codec_info.packetization_mode =
        webrtc::H264PacketizationMode::NonInterleaved;
    codec_info.temporal_idx = webrtc::kNoTemporalIdx;
    auto frameType = webrtc::VideoFrameType::kVideoFrameDelta;
    auto is_intra_frame = is_h264_i_frame(static_cast<const uint8_t *>(data));
    if (is_intra_frame) {
      if (!got_first_iframe) {
        BOOST_LOG_SEV(logger, logging::severity::debug)
            << __FUNCTION__ << ", got_first_iframe = true";
        got_first_iframe = true;
      }
#if 0
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << ", idr_frame detected";
#endif
      frameType = webrtc::VideoFrameType::kVideoFrameKey;
    }
#if 0 // no need to wait for first iframe no more. has been necessairy on rpi
    if (!got_first_iframe) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << " !got_first_iframe";
      return;
    }
#endif
    codec_info.idr_frame = encoded._frameType == frameType;
    codec_info.base_layer_sync = false;

    h264_bitstream_parser_.ParseBitstream(rtc::ArrayView<const std::uint8_t>(
        static_cast<const std::uint8_t *>(data), size));
    encoded.qp_ = h264_bitstream_parser_.GetLastSliceQp().value_or(-1);

    const std::vector<webrtc::H264::NaluIndex> nalu_indexes =
        webrtc::H264::FindNaluIndices(static_cast<const std::uint8_t *>(data),
                                      size);

    if (nalu_indexes.empty()) {
      // could not find the nal unit in the buffer, so do nothing.
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << "NAL unit length is zero! "
          << "Frame length : " << size;
      return;
    };

    int64_t capture_time_ms = clock_->TimeInMilliseconds();
    // int64_t ntp_capture_time_ms = clock_->CurrentNtpInMilliseconds();

    encoded.SetTimestamp(capture_time_ms);
    encoded.SetSpatialIndex(0);
    encoded._frameType = frameType;
    // encoded.ntp_time_ms_ = ntp_capture_time_ms;
    // encoded.capture_time_ms_ = capture_time_ms;
    // encoded._encodedWidth = config_.width;
    // encoded._encodedHeight = config_.height;

    info.codecSpecific.H264 = codec_info;
    info.codecType = webrtc::kVideoCodecH264;
    // info.end_of_picture = true;
    if (!callback) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__ << ", callback not set";
      return;
    }
    webrtc::EncodedImageCallback::Result result =
        callback->OnEncodedImage(encoded, &info);
    if (result.error ==
        webrtc::EncodedImageCallback::Result::ERROR_SEND_FAILED) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__
          << ", error in passing EncodedImage, ERROR_SEND_FAILED";
      // consequences?
    }
  }

  int32_t RegisterEncodeCompleteCallback(
      webrtc::EncodedImageCallback *callback_) override {
    callback = callback_;
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    return WEBRTC_VIDEO_CODEC_OK;
  }

  int32_t Release() override {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    reader_.reset();
    return WEBRTC_VIDEO_CODEC_OK;
  }

  int32_t
  Encode(const webrtc::VideoFrame &frame,
         const std::vector<webrtc::VideoFrameType> *frame_types) override {
    // this function gets called with width=1 if the remote client demands an
    // iframe
    (void)frame;
    (void)frame_types;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", frame.width: " << frame.width();
    if (!reader_)
      return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
    reader_->mainloop();
    if (frame_types != nullptr) {
      // We only support a single stream.
      RTC_DCHECK_EQ(frame_types->size(), static_cast<size_t>(1));
      // Skip frame?
      if ((*frame_types)[0] == webrtc::VideoFrameType::kEmptyFrame) {
        return WEBRTC_VIDEO_CODEC_OK;
      }
      // Force key frame?
      if ((*frame_types)[0] == webrtc::VideoFrameType::kVideoFrameKey)
        reader_->trigger_i_frame();
    }
    return WEBRTC_VIDEO_CODEC_OK;
  }

  void SetRates(const RateControlParameters &parameters) override {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", framerate_fps: " << parameters.framerate_fps
        << ", bandwidth_allocation: " << parameters.bandwidth_allocation.kbps()
        << ", bitrate: " << parameters.bitrate.ToString()
        << ", target_bitrate: " << parameters.target_bitrate.ToString();
    if (!reader_) {
      BOOST_LOG_SEV(logger, logging::severity::error)
          << __FUNCTION__ << ", encoder is not initialised";
      // BOOST_ASSERT(false);
      return;
    }
    reader_->set_bitrate(parameters.target_bitrate.get_sum_bps());
  }

  EncoderInfo GetEncoderInfo() const override {
    auto info = webrtc::VideoEncoder::GetEncoderInfo();
    // webrtc::VideoEncoder::EncoderInfo info;
    info.has_internal_source = true;
    info.is_hardware_accelerated = true;
    // info.has_trusted_rate_controller = true; // TODO push to settings!
    info.implementation_name = "v4l2_hw_h264";
    // info.scaling_settings = webrtc::VideoEncoder::ScalingSettings::kOff;
    // TODO don't use fixed values
    info.resolution_bitrate_limits = {VideoEncoder::ResolutionBitrateLimits(
        1280 * 720, 25'000, 25'000, 25'000'000)};
    // https://webrtcglossary.com/simulcast/
    // info.supports_simulcast = false; // not needed. `false` is default
    BOOST_LOG_SEV(const_cast<video_encoder_impl *>(this)->logger,
                  logging::severity::debug)
        << __FUNCTION__ << ", info: " << info.ToString();
    return info;
  }

private:
  rtc::logger logger{"video_encoder_impl"};
  webrtc::H264BitstreamParser h264_bitstream_parser_;
};
} // namespace

std::unique_ptr<video_encoder>
video_encoder::create(std::unique_ptr<reader> reader_move) {
  return std::make_unique<video_encoder_impl>(std::move(reader_move));
}
