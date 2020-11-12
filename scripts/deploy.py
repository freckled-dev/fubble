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

install_linux = os.path.join(script_dir, '..', 'install')
install_windows = os.path.join(script_dir, '..', 'install_windows')

matrix_user = os.environ.get('FUBBLE_TEMPORARY_ROOM_MATRIX_USER')
matrix_password = os.environ.get('FUBBLE_TEMPORARY_ROOM_MATRIX_PASSWORD')
matrix_device_id = os.environ.get('FUBBLE_TEMPORARY_ROOM_MATRIX_DEVICE_ID')
gitlab_registry_username = os.environ.get('CI_REGISTRY_USER')
gitlab_registry_password = os.environ.get('CI_REGISTRY_PASSWORD')

args = ['ansible-playbook']
args += [
    '-i', 'deploy/inventory/production.yml',
    '-e', 'fubble_binaries_dir=%s' % (install_linux),
    '-e', 'fubble_binaries_windows_dir="%s"' % (install_windows),
    '-e', 'fubble_enable_server=yes',
    '-e', 'fubble_enable_windows_client=yes',
    '-e', 'fubble_enable_appimage=yes',
    '-e', 'gitlab_registry_username="%s"' % gitlab_registry_username,
    '-e', 'gitlab_registry_password="%s"' % gitlab_registry_password
    ]
if matrix_user:
    print('using matrix_user:%s' % matrix_user)
    args += [
            '-e', 'fubble_temporary_room_matrix_user=%s' % matrix_user,
            '-e', 'fubble_temporary_room_matrix_password=%s' % matrix_password
            ]
    if matrix_device_id:
        args += [
                '-e', 'fubble_temporary_room_matrix_device_id=%s' % matrix_device_id
                ]
args += ['../deploy/site.yml']

subprocess.run(args, check=True)

