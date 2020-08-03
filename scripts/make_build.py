#!/usr/bin/env python3

import os
import subprocess
import shutil
from paths import Paths
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--skip_remove', help='don\'t remove the build directory',
        action="store_true")
parser.add_argument('--skip_install', help='don\'t run conan install',
        action="store_true")
parser.add_argument('--skip_install_update', help='don\'t run conan install --update',
        action="store_true")
parser.add_argument('--skip_build', help='don\'t run conan build',
        action="store_true")
parser.add_argument('--skip_package', help='don\'t run conan package',
        action="store_true")
parser.add_argument('--profile', help='the conan profile to use', default='default')
parser.add_argument('--use_asan', help='use address sanitizer \'memory,undefined\'', action="store_true")
parser.add_argument('--treat_warnings_as_errors', help='warnings shall get handled as errors', action="store_true")
args = parser.parse_args()

paths = Paths()

if not args.skip_remove:
    try:
        shutil.rmtree(paths.build_dir)
    except:
        print("Could not delete the build_dir:'%s'. Ignoring." % (paths.build_dir))

if not args.skip_install:
    conan_remotes = [
            ('bincrafters', 'https://api.bintray.com/conan/bincrafters/public-conan'),
            ('inexorgame', 'https://api.bintray.com/conan/inexorgame/inexor-conan'),
            ('google_webrtc', 'https://api.bintray.com/conan/freckled/google-webrtc'),
            ('rectangle_bin_pack', 'https://api.bintray.com/conan/freckled/RectangleBinPack')
            ]
    for remote_name, remote_url in conan_remotes:
        subprocess.run(['conan', 'remote', 'add', '-f', remote_name, remote_url],
                check=True)

    install_args = ['conan', 'install',
        '--build', 'missing',
        '--install-folder', paths.dependencies_dir,
        '--profile', args.profile]
    if not args.skip_install_update:
        install_args += ['--update'] # Check updates exist from upstream remotes
    if args.treat_warnings_as_errors:
        install_args += ['-o', 'fubble:treat_warnings_as_errors=True']
    if args.use_asan:
        install_args += ['-o', 'fubble:sanatize=True']
    subprocess.run(install_args + [paths.source_dir],
        check=False) # may fail, due to instability of bintray

if not args.skip_build:
    subprocess.run(['conan', 'build',
       paths.source_dir,
        # '--build', 'missing',
        '--build-folder', paths.build_dir,
        '--install-folder', paths.dependencies_dir,
        '--package-folder', paths.prefix_dir,
        ], check=True)

if not args.skip_package:
    subprocess.run(['conan', 'package',
        paths.source_dir,
        '--build-folder', paths.build_dir,
        '--install-folder', paths.dependencies_dir,
        '--package-folder', paths.prefix_dir,
        ], check=True)

