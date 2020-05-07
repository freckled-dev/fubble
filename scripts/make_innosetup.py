#!/usr/bin/env python3

import os
import subprocess
import shutil
from paths import Paths
from distutils.dir_util import copy_tree

print('fun')

paths = Paths()

innosetup_build_dir = os.path.join(paths.build_dir, 'innosetup')
os.mkdir(innosetup_build_dir)

inno_file = os.path.join(paths.source_dir, 'scripts', 'innosetup', 'Fubble_Installer.iss')
shutil.copy2(inno_file, innosetup_build_dir)

updater_bin_dir = os.path.join(paths.source_dir, 'updater', 'bin', 'Release')
copy_tree(updater_bin_dir, innosetup_build_dir)

os.chdir(updater_bin_dir)
subprocess.run(['curl.exe', '-Lo', 'rclone.zip',
    'https://downloads.rclone.org/v1.51.0/rclone-v1.51.0-windows-amd64.zip'], 
    check=True)
subprocess.run(['7z.exe', 'e', 'rclone.zip'], check=True)

subprocess.run(['iscc', '/O' + paths.source_dir, 'Fubble_Installer.iss'], check=True)
