#!/bin/bash

# This script installs and h5py and flask for HDF5Viewer Synology app.
# Usage:
#
# 0. Install Python3.9 using Package Manager.
# 1. Upload this script to /volume1/homes/demouser.
# 2. Go to Control Panel > Task Manager.
# 3. Add Manual script:
#   bash /volume1/homes/demouser/install.sh >& /volume1/homes/demouser/out.txt

ls /usr/local/bin
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
/usr/local/bin/python3.9 get-pip.py
/var/services/homes/demouser/.local/bin/pip3 install h5py flask


