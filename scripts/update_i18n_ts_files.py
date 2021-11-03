#!/usr/bin/env python3

import pathlib
import subprocess
import os
import distro

script_dir = pathlib.Path(__file__).parent.resolve()
client_dir = os.path.join(script_dir, '..', 'fubble', 'client')
i18n_dir = os.path.join(script_dir, '..', 'fubble', 'app', 'i18n');
de_ts = os.path.join(i18n_dir, 'qml_de.ts')
en_ts = os.path.join(i18n_dir, 'qml_en.ts')

if distro.id() == "linuxmint" or distro.id() == "ubuntu":
    subprocess.run([
        'lupdate', client_dir,
        '-ts', de_ts,
        '-ts', en_ts,
        ], check=True)
else:
    subprocess.run([
        'lupdate-qt5', client_dir,
        '-ts', de_ts,
        '-ts', en_ts,
        ], check=True)

