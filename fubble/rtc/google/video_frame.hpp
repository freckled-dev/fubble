#ifndef UUID_7B29F782_81B8_4FCF_A893_156036A0EAAD
#define UUID_7B29F782_81B8_4FCF_A893_156036A0EAAD

#include <api/video/video_frame.h>
#include <boost/assert.hpp>
#include <fubble/rtc/video_frame.hpp>

namespace rtc {
namespace google {
class video_frame : public rtc::video_frame {
public:
  video_frame(const webrtc::VideoFrame &frame) : frame{frame} {
    auto frame_buffer = frame.video_frame_buffer();
    webrtc::VideoFrameBuffer::Type type = frame_buffer->type();
    (void) type;
    BOOST_ASSERT(type == webrtc::VideoFrameBuffer::Type::kI420);
  }

  int width() const override { return frame.width(); }
  int height() const override { return frame.height(); }

  void i420(buffer &data_y, int &stride_y, buffer &data_u, int &stride_u,
            buffer &data_v, int &stride_v) const override {
    // copy all data, because frame seems not to be thread safe. got problems on
    // windows when destructing frame on ui thread
    webrtc::VideoFrameBuffer *buffer = frame.video_frame_buffer().get();
    const webrtc::I420BufferInterface *buffer_accessor{};
    rtc::scoped_refptr<webrtc::I420BufferInterface> i420_reference;

    if (buffer->type() == webrtc::VideoFrameBuffer::Type::kI420) {
      buffer_accessor = buffer->GetI420();
    } else {
      i420_reference = buffer->ToI420();
      buffer_accessor = i420_reference.get();
    }
    BOOST_ASSERT(buffer_accessor);
    {
      stride_y = buffer_accessor->StrideY();
      auto size_y = stride_y * buffer_accessor->height();
      auto data_ptr_y = buffer_accessor->DataY();
      data_y.assign(data_ptr_y, data_ptr_y + size_y);
    }
    auto chroma_height = buffer_accessor->ChromaHeight();
    {
      stride_u = buffer_accessor->StrideU();
      auto size_u = stride_u * chroma_height;
      auto data_ptr_u = buffer_accessor->DataU();
      data_u.assign(data_ptr_u, data_ptr_u + size_u);
    }
    {
      stride_v = buffer_accessor->StrideV();
      auto size_v = stride_v * chroma_height;
      auto data_ptr_v = buffer_accessor->DataV();
      data_v.assign(data_ptr_v, data_ptr_v + size_v);
    }
  }

  const webrtc::VideoFrame &native() const { return frame; }

protected:
  const webrtc::VideoFrame &frame;
};
} // namespace google
} // namespace rtc

#endif
