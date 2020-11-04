#!/usr/bin/env python3

import pathlib
import subprocess
import os

script_dir = pathlib.Path(__file__).parent.resolve()
client_dir = os.path.join(script_dir, '..', 'client')
de_ts = os.path.join(script_dir, '..', 'client', 'app', 'i18n', 'qml_de.ts')

subprocess.run([
    'lupdate-qt5', client_dir, '-ts', de_ts
    ], check=True)

