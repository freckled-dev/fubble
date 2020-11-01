#!/usr/bin/env python3

import os
import sys
import subprocess
import platform
import shutil
import pathlib
from paths import Paths

class UnsupportedPlatform(Exception):
    pass

# https://stackoverflow.com/questions/1854/what-os-am-i-running-on
if platform.system() != "Windows":
    raise UnsupportedPlatform()

qt_version = "5.15.1"
qt_prefix_dir = "C:/Qt"
qt_platform = "desktop"
qt_system = "windows"
qt_target = "win64_msvc2019_64"
qt_target_folder = "msvc2019_64"

final_dir = os.path.join(qt_prefix_dir, qt_version, qt_target_folder)
qmake_path = os.path.join(final_dir, 'bin', 'qmake.exe')
if os.path.exists(qmake_path):
    print("skipping installation of qt, because '%s' does already exists" % qmake_path)
    sys.exit()
print("installation of qt, because '%s' does not exists" % qmake_path)

subprocess.run(['aqt', 'install', 
    '--outputdir', qt_prefix_dir,
    qt_version, qt_system, qt_platform, qt_target,
    '-m', 'qtcharts'],
        check=True)


