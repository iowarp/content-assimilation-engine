"""
This module provides classes and methods to launch the OMNI Content Assimilation Engine.
OMNI processes data files according to YAML job specifications and supports MPI scaling.
"""
from jarvis_cd.basic.pkg import Application
from jarvis_util import *
import os
import pathlib


class OmniParse(Application):
    """
    This class provides methods to launch the OMNI Content Assimilation Engine application.
    """
    def _init(self):
        """
        Initialize paths
        """
        self.omni_yaml_file = None
        self.wrp_executable = None

    def _configure_menu(self):
        """
        Create a CLI menu for the configurator method.
        For thorough documentation of these parameters, view:
        https://github.com/scs-lab/jarvis-util/wiki/3.-Argument-Parsing

        :return: List(dict)
        """
        return [
            {
                'name': 'omni_yaml',
                'msg': 'Path to the OMNI YAML configuration file',
                'type': str,
                'default': None,
                'required': True,
            },
            {
                'name': 'wrp_path',
                'msg': 'Path to the wrp executable (auto-detected if not specified)',
                'type': str,
                'default': None,
            },
            {
                'name': 'cwd',
                'msg': 'Working directory to run omni from (default: build directory)',
                'type': str,
                'default': None,
            },
        ]

    def _configure(self, **kwargs):
        """
        Converts the Jarvis configuration to application-specific configuration.
        Validates the OMNI YAML file and wrp executable paths.

        :param kwargs: Configuration parameters for this pkg.
        :return: None
        """
        # Validate omni_yaml file
        if not self.config['omni_yaml']:
            raise Exception('omni_yaml parameter is required')
        
        omni_yaml_path = os.path.expandvars(self.config['omni_yaml'])
        if not os.path.isabs(omni_yaml_path):
            omni_yaml_path = os.path.abspath(omni_yaml_path)
            
        if not os.path.exists(omni_yaml_path):
            raise Exception(f'OMNI YAML file not found: {omni_yaml_path}')
        
        self.config['omni_yaml'] = omni_yaml_path
        
        # Find wrp executable
        if self.config['wrp_path']:
            wrp_path = os.path.expandvars(self.config['wrp_path'])
            if not os.path.isabs(wrp_path):
                wrp_path = os.path.abspath(wrp_path)
        else:
            # Auto-detect wrp executable
            # Look in common locations: build/bin/, bin/, current directory
            possible_paths = [
                'build/bin/wrp',
                'bin/wrp',
                './wrp',
                'wrp'
            ]
            wrp_path = None
            for path in possible_paths:
                abs_path = os.path.abspath(path)
                if os.path.exists(abs_path) and os.access(abs_path, os.X_OK):
                    wrp_path = abs_path
                    break
            
            if not wrp_path:
                raise Exception('Could not find wrp executable. Please specify wrp_path or ensure it is in build/bin/ or PATH')
        
        if not os.path.exists(wrp_path):
            raise Exception(f'wrp executable not found: {wrp_path}')
        
        if not os.access(wrp_path, os.X_OK):
            raise Exception(f'wrp executable is not executable: {wrp_path}')
            
        self.config['wrp_path'] = wrp_path
        
        # Set working directory
        if self.config['cwd']:
            cwd = os.path.expandvars(self.config['cwd'])
            if not os.path.isabs(cwd):
                cwd = os.path.abspath(cwd)
        else:
            # Default to build directory if it exists, otherwise current directory
            if os.path.exists('build'):
                cwd = os.path.abspath('build')
            else:
                cwd = os.getcwd()
                
        if not os.path.exists(cwd):
            raise Exception(f'Working directory not found: {cwd}')
            
        self.config['cwd'] = cwd
        
        print(f'OMNI Configuration:')
        print(f'  YAML file: {self.config["omni_yaml"]}')
        print(f'  wrp executable: {self.config["wrp_path"]}')
        print(f'  Working directory: {self.config["cwd"]}')

    def start(self):
        """
        Launch the OMNI Content Assimilation Engine with the specified YAML configuration.

        :return: None
        """
        # Construct the command to run wrp with the YAML file
        cmd = f'{self.config["wrp_path"]} {self.config["omni_yaml"]}'
        if self.jarvis.hostfile.path:
            cmd += f' {self.jarvis.hostfile.path}'
        
        print(f'Executing OMNI job: {cmd}')
        print(f'Working directory: {self.config["cwd"]}')
        
        # Execute using LocalExecInfo with mod_env for interception support
        node = Exec(cmd,
                    LocalExecInfo(env=self.mod_env,
                                  cwd=self.config['cwd'],
                                  do_dbg=self.config['do_dbg'],
                                  dbg_port=self.config['dbg_port'],
                                  pipe_stdout=self.config.get('stdout'),
                                  pipe_stderr=self.config.get('stderr'),
                                  collect_output=True))
        
        # Store the exit code for potential use in statistics
        self.exit_code = node.exit_code
        if node.exit_code != 0:
            print(f'OMNI job failed with exit code: {node.exit_code}')
            if hasattr(node, 'stdout') and node.stdout:
                print(f'STDOUT: {node.stdout}')
            if hasattr(node, 'stderr') and node.stderr:
                print(f'STDERR: {node.stderr}')
        else:
            print('OMNI job completed successfully')
        
        return node.exit_code

    def stop(self):
        """
        Stop a running application. OMNI is typically a short-running job,
        so this is usually not needed.

        :return: None
        """
        pass

    def clean(self):
        """
        Clean up any temporary files or data created by the OMNI job.

        :return: None
        """
        # OMNI jobs may create output files, but we don't want to automatically
        # delete them as they may be important results. Users should clean
        # manually if needed.
        pass

    def _get_stat(self, stat_dict):
        """
        Get statistics from the OMNI execution.

        :param stat_dict: A dictionary of statistics.
        :return: None
        """
        if hasattr(self, 'exit_code'):
            stat_dict[f'{self.pkg_id}.exit_code'] = self.exit_code
        if hasattr(self, 'start_time'):
            stat_dict[f'{self.pkg_id}.runtime'] = self.start_time 