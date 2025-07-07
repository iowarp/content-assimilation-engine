"""
This module provides classes and methods to inject the CAE adapter interceptor.
CAE adapter intercepts the MPI I/O calls used by a native MPI program and
routes it to CAE.
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
            self.env['CAE_MPIIO'] = self.find_library('cae_mpiio')
            if self.env['CAE_MPIIO'] is None:
                raise Exception('Could not find cae_mpiio')
            self.env['CAE_ROOT'] = str(pathlib.Path(self.env['CAE_MPIIO']).parent.parent)
            print(f'Found libcae_mpiio.so at {self.env["CAE_MPIIO"]}')
            has_one = True
        if self.config['posix']:
            self.env['CAE_POSIX'] = self.find_library('cae_posix')
            if self.env['CAE_POSIX'] is None:
                raise Exception('Could not find cae_posix')
            self.env['CAE_ROOT'] = str(pathlib.Path(self.env['CAE_POSIX']).parent.parent)
            print(f'Found libcae_posix.so at {self.env["CAE_POSIX"]}')
            has_one = True
        if self.config['stdio']:
            self.env['CAE_STDIO'] = self.find_library('cae_stdio')
            if self.env['CAE_STDIO'] is None:
                raise Exception('Could not find cae_posix')
            self.env['CAE_ROOT'] = str(pathlib.Path(self.env['CAE_STDIO']).parent.parent)
            print(f'Found libcae_stdio.so at {self.env["CAE_STDIO"]}')
            has_one = True
        if self.config['vfd']:
            self.env['CAE_VFD'] = self.find_library('hdf5_cae_vfd')
            if self.env['CAE_VFD'] is None:
                raise Exception('Could not find hdf5_cae_vfd')
            self.env['CAE_ROOT'] = str(pathlib.Path(self.env['CAE_VFD']).parent.parent)
            print(f'Found libhdf5_cae_vfd.so at {self.env["CAE_VFD"]}')
            has_one = True
        if self.config['nvidia_gds']:
            self.env['CAE_NVIDIA_GDS'] = self.find_library('cae_nvidia_gds')
            if self.env['CAE_NVIDIA_GDS'] is None:
                raise Exception('Could not find cae_nvidia_gds')
            self.env['CAE_ROOT'] = str(pathlib.Path(self.env['CAE_NVIDIA_GDS']).parent.parent)
            print(f'Found libcae_nvidia_gds.so at {self.env["CAE_NVIDIA_GDS"]}')
            has_one = True
        if not has_one:
            raise Exception('CAE API not selected')

    def modify_env(self):
        """
        Modify the jarvis environment.

        :return: None
        """
        if self.config['mpi']:
            self.append_env('LD_PRELOAD', self.env['CAE_MPIIO'])
        if self.config['posix']:
            self.append_env('LD_PRELOAD', self.env['CAE_POSIX'])
        if self.config['stdio']:
            self.append_env('LD_PRELOAD', self.env['CAE_STDIO'])
        if self.config['vfd']:
            plugin_path_parent = (
                str(pathlib.Path(self.env['CAE_VFD']).parent))
            self.setenv('HDF5_PLUGIN_PATH', plugin_path_parent)
            self.setenv('HDF5_DRIVER', 'hdf5_cae_vfd')
        if self.config['nvidia_gds']:
            self.append_env('LD_PRELOAD', self.env['CAE_NVIDIA_GDS'])
