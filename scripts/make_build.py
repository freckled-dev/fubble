#!/usr/bin/env python3

import os
import subprocess
import shutil

def get_first_line_of_subprocess_result(result):
    return result.stdout.decode('utf-8').partition('\n')[0]

def get_environment_variable_or(variable, or_):
    result = os.environ.get(variable)
    if result:
        return result
    return or_

class Paths:
    def __init__(self):
        script_dir = os.path.realpath(__file__)
        git_result = subprocess.run(['git', 'rev-parse', '--show-toplevel'], 
            stdout=subprocess.PIPE, check=True)
        self.source_dir = get_first_line_of_subprocess_result(git_result)
        self.build_dir = os.path.join(self.source_dir, '..', 'fubble_build')
        self.build_dir = get_environment_variable_or('FUBBLE_BUILD_DIR', self.build_dir)
        self.meson_dir = os.path.join(self.build_dir, 'meson')
        self.dependencies_dir = os.path.join(self.build_dir, 'dependencies')
        self.prefix_dir = os.path.join(self.build_dir, 'install')
        self.prefix_dir = get_environment_variable_or('FUBBLE_PREFIX_DIR', self.prefix_dir)

paths = Paths()
try:
    shutil.rmtree(paths.build_dir)
except:
    print("Could not delete the build_dir:'%s'" % (paths.build_dir))

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

