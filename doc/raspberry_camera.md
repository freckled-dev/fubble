# general

hw encoder does not support more than 1080p!

# test camera using rpi tools

## video

use `raspivid`. Examples:

```bash
raspivid -t 10000 -w 1280 -h 720 -fps 60 -sh 100 -pts fun.pts -b 25000000 -o fun.h264
# `-t` for time
# `-sh` sharpness
# `-pts` timestamps for `mkvmerge`
# `-b` bitrate
mkvmerge fun.h264 -o fun.mkv --timecodes 0:fun.pts
```

### high-res images

```bash
raspistill -o fun.jpg
```

# zoominging in or cropped image issue

- [an issue about it](https://github.com/showmewebcam/showmewebcam/issues/9)
- [official documentation about cropped image](https://picamera.readthedocs.io/en/release-1.13/fov.html#sensor-modes). It's not possible to force a sensor-mode with v4l2

