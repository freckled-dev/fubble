# v4l2loopback

source: https://github.com/umlaeute/v4l2loopback

```bash
# preconditions
sudo dnf install kernel-devel
# build
git clone https://github.com/umlaeute/v4l2loopback.git
cd v4l2loopback
make
sudo make install
# load
# gstreamer can't allocate unless `max_buffers` is set.
# https://github.com/umlaeute/v4l2loopback/issues/166#issuecomment-465960256
sudo modprobe v4l2loopback max_buffers=2
```

send videos to fake device

- ffmpeg: https://github.com/umlaeute/v4l2loopback/wiki/Ffmpeg
- gstreamer: webrtc could not read it https://github.com/umlaeute/v4l2loopback/wiki/Gstreamer

```bash
# produce a video
# sintel https://durian.blender.org/download/
curl -LO http://peach.themazzone.com/durian/movies/sintel-1280-surround.mp4
// TODO more movie links. maybe with torrent: https://www.maketecheasier.com/how-to-download-torrents-from-the-command-line-in-ubuntu/
ffmpeg -re -i ./sintel-1280-surround.mp4 -f v4l2 /dev/video0

# consume it by gstreamer
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! autovideosink
```

