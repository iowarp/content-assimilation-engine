"""
This module provides classes and methods to inject the HermesMpiio interceptor.
HermesMpiio intercepts the MPI I/O calls used by a native MPI program and
routes it to Hermes.
"""
from jarvis_cd.basic.pkg import Interceptor
from jarvis_util import *


class CaeAdapter(Interceptor):
    """
    This class provides methods to inject the CAE adapter interceptor.
    """
    def _init(self):
        """
        Initialize paths
        """
        pass

    def _configure_menu(self):
        """
        Create a CLI menu for the configurator method.
        For thorough documentation of these parameters, view:
        https://github.com/scs-lab/jarvis-util/wiki/3.-Argument-Parsing

        :return: List(dict)
        """
        return [
            {
                'name': 'mpi',
                'msg': 'Intercept MPI-IO',
                'type': bool,
                'default': False
            },
            {
                'name': 'posix',
                'msg': 'Intercept POSIX',
                'type': bool,
                'default': False
            },
            {
                'name': 'stdio',
                'msg': 'Intercept STDIO',
                'type': bool,
                'default': False
            },
            {
                'name': 'vfd',
                'msg': 'Intercept HDF5 I/O',
                'type': bool,
                'default': False
            },
            {
                'name': 'nvidia_gds',
                'msg': 'Intercept NVIDIA GDS I/O',
                'type': bool,
                'default': False
            },
        ]

    def _configure(self, **kwargs):
        """
        Converts the Jarvis configuration to application-specific configuration.
        E.g., OrangeFS produces an orangefs.xml file.

        :param kwargs: Configuration parameters for this pkg.
        :return: None
        """
        has_one = False
        if self.config['mpi']:
            self.env['HERMES_MPIIO'] = self.find_library('hermes_mpiio')
            if self.env['HERMES_MPIIO'] is None:
                raise Exception('Could not find hermes_mpi')
            self.env['HERMES_ROOT'] = str(pathlib.Path(self.env['HERMES_MPIIO']).parent.parent)
            print(f'Found libhermes_mpiio.so at {self.env["HERMES_MPIIO"]}')
            has_one = True
        if self.config['posix']:
            self.env['HERMES_POSIX'] = self.find_library('hermes_posix')
            if self.env['HERMES_POSIX'] is None:
                raise Exception('Could not find hermes_posix')
            self.env['HERMES_ROOT'] = str(pathlib.Path(self.env['HERMES_POSIX']).parent.parent)
            print(f'Found libhermes_posix.so at {self.env["HERMES_POSIX"]}')
            has_one = True
        if self.config['stdio']:
            self.env['HERMES_STDIO'] = self.find_library('hermes_stdio')
            if self.env['HERMES_STDIO'] is None:
                raise Exception('Could not find hermes_posix')
            self.env['HERMES_ROOT'] = str(pathlib.Path(self.env['HERMES_STDIO']).parent.parent)
            print(f'Found libhermes_stdio.so at {self.env["HERMES_STDIO"]}')
            has_one = True
        if self.config['vfd']:
            self.env['HERMES_VFD'] = self.find_library('hdf5_hermes_vfd')
            if self.env['HERMES_VFD'] is None:
                raise Exception('Could not find hdf5_hermes_vfd')
            self.env['HERMES_ROOT'] = str(pathlib.Path(self.env['HERMES_VFD']).parent.parent)
            print(f'Found libhdf5_hermes_vfd.so at {self.env["HERMES_VFD"]}')
            has_one = True
        if self.config['nvidia_gds']:
            self.env['HERMES_NVIDIA_GDS'] = self.find_library('hermes_nvidia_gds')
            if self.env['HERMES_NVIDIA_GDS'] is None:
                raise Exception('Could not find hermes_nvidia_gds')
            self.env['HERMES_ROOT'] = str(pathlib.Path(self.env['HERMES_NVIDIA_GDS']).parent.parent)
            print(f'Found libhermes_nvidia_gds.so at {self.env["HERMES_NVIDIA_GDS"]}')
            has_one = True
        if not has_one:
            raise Exception('Hermes API not selected')

    def modify_env(self):
        """
        Modify the jarvis environment.

        :return: None
        """
        if self.config['mpi']:
            self.append_env('LD_PRELOAD', self.env['HERMES_MPIIO'])
        if self.config['posix']:
            self.append_env('LD_PRELOAD', self.env['HERMES_POSIX'])
        if self.config['stdio']:
            self.append_env('LD_PRELOAD', self.env['HERMES_STDIO'])
        if self.config['vfd']:
            plugin_path_parent = (
                str(pathlib.Path(self.env['HERMES_VFD']).parent))
            self.setenv('HDF5_PLUGIN_PATH', plugin_path_parent)
            self.setenv('HDF5_DRIVER', 'hdf5_hermes_vfd')
        if self.config['nvidia_gds']:
            self.append_env('LD_PRELOAD', self.env['HERMES_NVIDIA_GDS'])
