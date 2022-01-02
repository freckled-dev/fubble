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
parser.add_argument('--skip_install_webrtc', help='don\'t run `conan install google-webrtc`',
        action="store_true")
parser.add_argument('--skip_install_update', help='don\'t run `conan install --update`',
        action="store_true")
parser.add_argument('--skip_build', help='don\'t run conan build',
        action="store_true")
parser.add_argument('--skip_package', help='don\'t run conan package',
        action="store_true")
parser.add_argument('--skip_qt', help='don\'t pull qt',
        action="store_true")
parser.add_argument('--skip_export', help='don\'t export the package to conan',
        action="store_true")
parser.add_argument('--profile_host', help='the conan profile:host to use', default=None)
parser.add_argument('--profile_build', help='the conan profile:build to use', default=None)
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

def conan_profile_command_args():
    result = []
    if args.profile_build:
        result += ['--profile:build', args.profile_build]
    if args.profile_host:
        result += ['--profile:host', args.profile_host]
    return result

if not args.skip_install:
    print('installing dependencies')
    if not args.skip_install_webrtc:
        # webrtc remote is very buggy, checkout https://gitlab.com/groups/gitlab-org/-/epics/6816 and
        # remove the following hack as soon as gitlab-conan is production ready
        subprocess.run(['conan', 'remote', 'add', '-f', 
            'gitlab_google_webrtc', 'https://gitlab.com/api/v4/projects/19162728/packages/conan'],
            check=True)
        subprocess.run(['conan', 'install', 'google-webrtc/94@acof/stable',
            '--remote', 'gitlab_google_webrtc',
            ] + conan_profile_command_args(), check=True)
        subprocess.run(['conan', 'remote', 'remove', 'gitlab_google_webrtc'], check=True)

    # when the hack is fixed, just add it as a remote here
    conan_remotes = [
            #('gitlab_google_webrtc', 'https://gitlab.com/api/v4/projects/19162728/packages/conan'),
            ]
    for remote_name, remote_url in conan_remotes:
        subprocess.run(['conan', 'remote', 'add', '-f', remote_name, remote_url],
                check=True)

    # https://github.com/conan-io/conan/issues/8964
    install_args = ['conan', 'install',
        '--build', 'missing',
        # TODO do an issue, when install-folder and build-folder differ, it can't find the `conan_meson_native.ini` file
        # maybe issue does already exist: https://github.com/conan-io/conan/issues/8964
        '--install-folder', paths.build_dir,
        ]
    install_args += conan_profile_command_args()
    if platform.system() != "Windows":
        install_args += ['-o', 'fubble:qt_install={}'.format(qt_install)]
    if not args.skip_install_update:
        install_args += ['--update'] # Check updates exist from upstream remotes
    if args.treat_warnings_as_errors:
        install_args += ['-o', 'fubble:treat_warnings_as_errors=True']
    if args.use_asan:
        install_args += ['-o', 'fubble:sanatize=True']
    install_args += [paths.source_dir]
    #print(f'running: {install_args}')
    subprocess.run(install_args,
        check=True
        )

if not args.skip_build:
    print('calling `conan build`')
    subprocess.run(['conan', 'build',
       paths.source_dir,
        # '--build', 'missing',
        '--source-folder', paths.source_dir,
        '--install-folder', paths.build_dir,
        '--build-folder', paths.build_dir,
        '--package-folder', paths.prefix_dir,
        ], check=True)

if not args.skip_package:
    subprocess.run(['conan', 'package',
        paths.source_dir,
        '--source-folder', paths.source_dir,
        '--install-folder', paths.build_dir,
        '--build-folder', paths.build_dir,
        '--package-folder', paths.prefix_dir,
        ], check=True)

if not args.skip_export and not args.skip_package:
    subprocess.run(['conan', 'export-pkg',
        paths.source_dir,
        '-f',
        '--package-folder', paths.prefix_dir,
        ], check=True)

