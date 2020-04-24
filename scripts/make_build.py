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
        git_result = subprocess.run(['git', 'rev-parse', '--show-toplevel'], stdout=subprocess.PIPE, check=True)
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

os.makedirs(paths.meson_dir)
os.makedirs(paths.dependencies_dir)
subprocess.run(['conan', 'install', 
    '--build', 'missing', 
    '--install-folder', paths.dependencies_dir, 
    '-s', 'build_type=Debug', # TODO debug/release
    paths.source_dir])
os.environ['PKG_CONFIG_PATH'] = paths.dependencies_dir
boost_prefix_result = subprocess.run(['pkg-config', '--variable=prefix', 'boost'], stdout=subprocess.PIPE, check=True)
os.environ['BOOST_ROOT'] = get_first_line_of_subprocess_result(boost_prefix_result)

werror = 'true'
werror_environment = os.environ.get('FUBBLE_TREAT_WARNING_AS_ERROR')
if werror_environment == '0':
    werror = 'false'

# TODO support release build
subprocess.run(['meson',
    paths.source_dir, paths.meson_dir,
    f'--prefix={paths.prefix_dir}',
    '-Db_sanitize=address',
    '-Db_lundef=false',
    '-Dwarning_level=3',
    f'-Dwerror={werror}'
    ])

subprocess.run(['ninja',
    '-C', paths.meson_dir])

