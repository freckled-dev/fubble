## install greamer

```bash
sudo apt-get install -y gstreamer1.0-tools gstreamer1.0-nice gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-plugins-good libgstreamer1.0-dev  libglib2.0-dev libgstreamer-plugins-good1.0-dev libgstreamer-plugins-bad1.0-dev
# for gstreamer openh264 codec
sudo apt install ubuntu-restricted-extras
```

## rpi v4l2, gstreamer and h264 issue

https://stackoverflow.com/questions/25941171/how-to-get-gstreamer1-0-working-with-v4l2-raspicam-driver
file `/etc/modprobe.d/bcm_v4l2_workaround.conf` content
```
# https://www.raspberrypi.org/forums/viewtopic.php?f=43&t=87430
# workaround to keep gstreamer happy with camera resolution reporting
options bcm2835-v4l2 gst_v4l2src_is_broken=1
# in /etc/modprobe.d/bcm_v4l2_workaround.conf
```

## enable v4l2 at rpi

gst-launch-1.0 -v udpsrc port=1234 ! gdpdepay ! rtph264depay ! h264parse ! avdec_h264 ! fpsdisplaysink sync=false

## allow to record video

```bash
sudo raspi-config
# enable it in there
usermod -a -G video mlanner
```

## gstreamer testing

https://raspberrypi.stackexchange.com/questions/26675/modern-way-to-stream-h-264-from-the-raspberry-cam
https://raspberrypi.stackexchange.com/questions/7446/how-can-i-stream-h-264-video-from-the-raspberry-pi-camera-module-via-a-web-serve/63649#63649

### sender

```bash
# don't do >=60 fps because the gstreamer sink seems to v-sync!
gst-launch-1.0 -e v4l2src do-timestamp=true ! video/x-h264,width=1280,height=720,framerate=50/1 ! h264parse ! rtph264pay config-interval=1 ! gdppay ! tcpserversink host=192.168.0.18 port=5000
# or use "udpsink host=192.168.0.164 port=1234"

# not recommended no more. use v4l2
raspivid -t 0 -h 720 -w 1080 -fps 50 -b 10000000 -o - | gst-launch-1.0 fdsrc ! h264parse !  rtph264pay config-interval=1 pt=96 ! gdppay ! tcpserversink host=192.168.0.18 port=5000
```

### receiver
```bash
# when sometimes grey and the pipelines reinits, is the cpu on limit!

gst-launch-1.0 tcpclientsrc host=192.168.0.18 port=5000  ! gdpdepay !  rtph264depay !  vaapih264dec low-latency=true ! fpsdisplaysink sync=false
# use avdec_h264 for slower software decoder. has higher latency
# instead of fpsdisplaysink you may want to use "videoconvert !s autovideosink"
# use "udpsrc port=1234" if the sender uses udp
```


# video4linux2 control

you can control v4l2 while reading it

```bash
# list options
v4l2-ctl --list-ctrls
# set brightness
v4l2-ctl --set-ctrl brightness=80
# flip the recording
v4l2-ctl --set-ctrl vertical_flip=1
# set bitrate to 5mbit (h264)
v4l2-ctl --set-ctrl video_bitrate=5000000
# fix sometimes grey picture - because of gst pipeline reinit?
v4l2-ctl --set-ctrl exposure_dynamic_framerate=1
```

# webrtc gstreamer resources
https://blog.nirbheek.in/2018/02/gstreamer-webrtc.html

# gpu encoded webrtc using gstreamer (python)
https://cloud.google.com/solutions/gpu-accelerated-streaming-using-webrtc

