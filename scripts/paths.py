import os
import subprocess
import pathlib

def get_environment_variable_or(variable, or_):
    result = os.environ.get(variable)
    if result:
        return result
    return or_

def get_first_line_of_subprocess_result(result):
    return result.stdout.decode('utf-8').partition('\n')[0]

class Paths:
    def __init__(self):
        script_dir = pathlib.Path(__file__).parent

        oldpwd = os.getcwd()
        os.chdir(script_dir)
        git_result = subprocess.run(['git', 'rev-parse', '--show-toplevel'],
            stdout=subprocess.PIPE, check=True)
        os.chdir(oldpwd)
        self.source_dir = get_first_line_of_subprocess_result(git_result)
        self.build_dir = os.path.join(self.source_dir, '..', 'fubble_build')
        self.build_dir = get_environment_variable_or('FUBBLE_BUILD_DIR', self.build_dir)
        self.meson_dir = os.path.join(self.build_dir, 'meson')
        self.dependencies_dir = os.path.join(self.build_dir, 'dependencies')
        self.prefix_dir = os.path.join(self.build_dir, 'install')
        self.prefix_dir = get_environment_variable_or('FUBBLE_PREFIX_DIR', self.prefix_dir)
        self.landing_dist_dir = os.path.join(self.source_dir, 'web', 'landing', 'dist');

