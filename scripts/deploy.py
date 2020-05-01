#!/usr/bin/env python3

import os
import subprocess
import shutil
from paths import Paths

paths = Paths()

deploy_dir = os.path.join(paths.build_dir, 'deploy')
deploy_source_dir = os.path.join(paths.source_dir, 'deploy', 'development')

try:
    shutil.rmtree(deploy_dir)
except:
    print("Could not delete the deploy_dir:'%s'. Ignoring." % (deploy_dir))
shutil.copytree(deploy_source_dir, deploy_dir)
shutil.copytree(paths.prefix_dir, os.path.join(deploy_dir, 'install'))
shutil.copytree(paths.landing_dist_dir, os.path.join(deploy_dir, 'landing_dist'))

os.chdir(deploy_dir)
subprocess.run(['docker-compose', 'build'],
        check=True)


