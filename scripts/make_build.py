#!/usr/bin/env python3

import os
import subprocess
import shutil
from paths import Paths

paths = Paths()
try:
    shutil.rmtree(paths.build_dir)
except:
    print("Could not delete the build_dir:'%s'. Ignoring." % (paths.build_dir))

subprocess.run(['conan', 'install',
    '--build', 'missing',
    '--install-folder', paths.dependencies_dir,
    paths.source_dir
    ], check=True)

werror = 'true'
werror_environment = os.environ.get('FUBBLE_TREAT_WARNING_AS_ERROR')
if werror_environment == '0':
    werror = 'false'

subprocess.run(['conan', 'build',
    paths.source_dir,
    # '--build', 'missing',
    '--build-folder', paths.build_dir,
    '--install-folder', paths.dependencies_dir,
    '--package-folder', paths.prefix_dir
    ], check=True)

subprocess.run(['conan', 'package',
    paths.source_dir,
    '--build-folder', paths.build_dir,
    '--install-folder', paths.dependencies_dir,
    '--package-folder', paths.prefix_dir
    ], check=True)

