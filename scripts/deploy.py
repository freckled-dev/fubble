#!/usr/bin/env python3

import os
import subprocess
import shutil
import pathlib
from paths import Paths

paths = Paths()

script_dir = pathlib.Path(__file__).parent.resolve()
deploy_dir = os.path.join(script_dir, 'deploy')
os.chdir(script_dir)
install_windows = os.path.join(script_dir, '..', 'install_windows')

subprocess.run(['ansible-playbook', 
    '-i', 'deploy/inventory/production.yml',
    '-e', 'fubble_binaries_dir=%s' % (paths.prefix_dir),
    '-e', 'fubble_binaries_windows_dir="%s"' % (install_windows),
    'deploy/site.yml'],
        check=True)

