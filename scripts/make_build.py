#!/usr/bin/env python3

import os
import platform
import subprocess
import shutil
from paths import Paths
from pathlib import Path
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
parser.add_argument('--skip_qt', help='don\'t pull qt',
        action="store_true")
parser.add_argument('--profile', help='the conan profile to use', default='default')
parser.add_argument('--use_asan', help='use address sanitizer \'memory,undefined\'', action="store_true")
parser.add_argument('--treat_warnings_as_errors', help='warnings shall get handled as errors', action="store_true")
args = parser.parse_args()

paths = Paths()

if not args.skip_remove:
    try:
        shutil.rmtree(paths.build_dir)
        print("removing the build_dir:'%s'." % (paths.build_dir))
    except:
        print("Could not delete the build_dir:'%s'. Ignoring." % (paths.build_dir))

Path(paths.build_dir).mkdir(parents=True, exist_ok=True)

# qt
qt_version = "5.15.1"
qt_platform = "desktop"
qt_system = ""
qt_target = ""
if platform.system() == "Darwin":
    qt_system = "mac"
    qt_target = "clang_64"
if platform.system() == "Linux":
    qt_system = "linux"
    qt_target = "gcc_64"
# qt_system = "windows"
# qt_target = "win64_msvc2019_64"
qt_install = os.path.join(paths.qt_dir, qt_version, qt_target)
if not args.skip_qt and platform.system() != "Windows":
    Path(paths.qt_dir).mkdir(parents=True, exist_ok=True)
    subprocess.run(['aqt', 'install',
        '--outputdir', paths.qt_dir,
        qt_version, qt_system, qt_platform, qt_target,
        '-m', 'qtcharts'],
            check=True)
    qt_pkgconfig_dir = os.path.join(qt_install, 'lib', 'pkgconfig')
    if platform.system() == "Linux":
        subprocess.run("sed -i 's+/home/qt/work/install+{}+g' {}/*".format(qt_install, qt_pkgconfig_dir), shell=True, check=True)
    if platform.system() == "Darwin":
        subprocess.run("sed -i '' 's+/Users/qt/work/install+{}+g' {}/*".format(qt_install, qt_pkgconfig_dir), shell=True, check=True)

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
    subprocess.run(['conan', 'remote', 'add', '--insert', '0', '-f',
        'fubble_dependencies', 'https://api.bintray.com/conan/freckled/fubble_dependencies'],
        check=True)

    install_args = ['conan', 'install',
        '--build', 'missing',
        '--install-folder', paths.dependencies_dir,
        '--profile', args.profile]
    if platform.system() != "Windows":
        install_args += ['-o', 'fubble:qt_install={}'.format(qt_install)]
    if not args.skip_install_update:
        install_args += ['--update'] # Check updates exist from upstream remotes
    if args.treat_warnings_as_errors:
        install_args += ['-o', 'fubble:treat_warnings_as_errors=True']
    if args.use_asan:
        install_args += ['-o', 'fubble:sanatize=True']
    subprocess.run(install_args + [paths.source_dir],
        check=False # may fail, due to instability of bintray
        )

if not args.skip_build:
    print('calling `conan build`')
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

