#!/usr/bin/env python3

from paths import Paths
import os
import subprocess

subprocess.run(['conan', 'remote', 'add', '-f', 'fubble',
    'https://api.bintray.com/conan/freckled/fubble'], check=False)
paths = Paths()
api_key = os.environ.get('FUBBLE_CONAN_API_KEY')
if api_key:
    subprocess.run(['conan', 'user', '-p', api_key, '--remote', 'fubble', 'freckled'],
        check=True)
subprocess.run(['conan', 'upload', '-c', '--remote', 'fubble', 'fubble'])

