"""
This module provides classes and methods to configure and run CAE.
"""
from jarvis_cd.basic.pkg import Service
from jarvis_util import *


class CaeRun(Service):
    """
    This class provides methods to configure and run CAE.
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
                'name': 'include',
                'msg': 'Specify paths to include',
                'type': list,
                'default': [],
                'args': [
                    {
                        'name': 'path',
                        'msg': 'The path to be included',
                        'type': str
                    },
                ],
                'aliases': ['i']
            },
            {
                'name': 'exclude',
                'msg': 'Specify paths to exclude',
                'type': list,
                'default': [],
                'args': [
                    {
                        'name': 'path',
                        'msg': 'The path to be excluded',
                        'type': str
                    },
                ],
                'aliases': ['e']
            },
            {
                'name': 'adapter_mode',
                'msg': 'The adapter mode to use for CAE',
                'type': str,
                'default': 'default',
                'choices': ['default', 'scratch', 'bypass'],
            },
            {
                'name': 'flush_mode',
                'msg': 'The flushing mode to use for adapters',
                'type': str,
                'default': 'async',
                'choices': ['sync', 'async'],
            },
            {
                'name': 'page_size',
                'msg': 'The page size to use for adapters',
                'type': str,
                'default': '1m',
            }
        ]

    def _configure(self, **kwargs):
        """
        Converts the Jarvis configuration to application-specific configuration.

        :param kwargs: Configuration parameters for this pkg.
        :return: None
        """
        # Begin making CAE client config
        cae_client = {
            'path_inclusions': [''],
            'path_exclusions': ['/'],
            'file_page_size': self.config['page_size']
        }

        # Set flushing mode
        if self.config['flush_mode'] == 'async':
            cae_client['flushing_mode'] = 'kAsync'
        elif self.config['flush_mode'] == 'sync':
            cae_client['flushing_mode'] = 'kSync'

        # Add path inclusions/exclusions
        if self.config['include'] is not None:
            cae_client['path_inclusions'] += self.config['include']
        if self.config['exclude'] is not None:
            cae_client['path_exclusions'] += self.config['exclude']

        # Set adapter mode
        if self.config['adapter_mode'] == 'default':
            adapter_mode = 'kDefault'
        elif self.config['adapter_mode'] == 'scratch':
            adapter_mode = 'kScratch'
        elif self.config['adapter_mode'] == 'bypass':
            adapter_mode = 'kBypass'
        cae_client['base_adapter_mode'] = adapter_mode

        # Save CAE client configuration
        cae_client_yaml = f'{self.shared_dir}/cae_client.yaml'
        YamlFile(cae_client_yaml).save(cae_client)
        self.env['IOWARP_CAE_CONF'] = cae_client_yaml

    def start(self):
        """
        Nothing to start - this package only generates configuration
        """
        pass

    def stop(self):
        """
        Nothing to stop - this package only generates configuration
        """
        pass

    def kill(self):
       pass

    def clean(self):
        """
        Clean up configuration files
        """
        pass
    
    def status(self):
        """
        Check whether or not an application is running. E.g., are OrangeFS
        servers running?

        :return: True or false
        """
        return True