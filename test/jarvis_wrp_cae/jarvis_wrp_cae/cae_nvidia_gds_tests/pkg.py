"""
This module provides classes and methods to launch the HermesGDSTests application.
HermesGDSTests is ....
"""
from jarvis_cd.basic.pkg import Application
from jarvis_util import *


class CaeNvidiaGdsTests(Application):
    """
    This class provides methods to launch the CAE NVIDIA GDS tests application.
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
                'name': 'test_file',
                'choices': ['nvidia_gds_basic',
                            'nvidia_gds_simple'],
                'msg': '',
                'type': str,
                'default': None,
            },
            {
                'name': 'test_case',
                'msg': 'Specify exact test case to run',
                'type': str,
                'default': None,
            },
            {
                'name': 'hermes',
                'msg': 'Whether or not to use Hermes',
                'type': bool,
                'default': False,
            },
            {
                'name': 'size',
                'msg': 'The size of the test to run',
                'choices': [None, 'small', 'large'],
                'type': str,
                'default': None,
            },
        ]

    def _configure(self, **kwargs):
        """
        Converts the Jarvis configuration to application-specific configuration.
        E.g., OrangeFS produces an orangefs.xml file.

        :param kwargs: Configuration parameters for this pkg.
        :return: None
        """
        pass

    def start(self):
        """
        Launch an application. E.g., OrangeFS will launch the servers, clients,
        and metadata services on all necessary pkgs.

        :return: None
        """
        Mkdir("/tmp/test_hermes")
        test_fun = getattr(self, f'test_{self.config["test_file"]}')
        test_fun()

    def test_nvidia_gds_basic(self):
        cmd = 'gds_adapter_basic_test'
        # if self.config['hermes']:
        #     cmd = f'hermes_{cmd}'
        if self.config['test_case']:
            cmd = (f'{cmd} {self.config["test_case"]}')
            cmd = (f'{cmd} BatchedWriteTemporalFixed,BatchedReadSequentialTemporalFixed')

        # else:
        #     gds_cmd = [cmd]
        #     if self.config['size'] == 'small':
        #         gds_cmd.append('~[request_size=range-small]')
        #     elif self.config['size'] == 'large':
        #         gds_cmd.append('~[request_size=range-large]')
        #     gds_cmd.append('--reporter compact -d yes')
        #     cmd = ' '.join(gds_cmd)
        node = Exec(cmd,
                    LocalExecInfo(env=self.mod_env,
                                  do_dbg=self.config['do_dbg'],
                                  dbg_port=self.config['dbg_port']))
                                #   pipe_stdout=self.config['stdout'],
                                #   pipe_stderr=self.config['stderr']))
        return node.exit_code

    def test_nvidia_gds_simple(self):
        cmd = 'gds_adapter_simple_test'
        if self.config['hermes']:
            cmd = f'hermes_{cmd}'
        if self.config['test_case']:
            cmd = f'{cmd} {self.config["test_case"]}'
        # else:
        #     gds_cmd = [cmd]
        #     if self.config['size'] == 'small':
        #         gds_cmd.append('~[request_size=range-small]')
        #     elif self.config['size'] == 'large':
        #         gds_cmd.append('~[request_size=range-large]')
        #     gds_cmd.append('--reporter compact -d yes')
        #     cmd = ' '.join(gds_cmd)
        node = Exec(cmd,
                    LocalExecInfo(env=self.mod_env,
                                  do_dbg=self.config['do_dbg'],
                                  dbg_port=self.config['dbg_port']))
                                #   pipe_stdout=self.config['stdout'],
                                #   pipe_stderr=self.config['stderr']
        return node.exit_code

    def stop(self):
        """
        Stop a running application. E.g., OrangeFS will terminate the servers,
        clients, and metadata services.

        :return: None
        """
        pass

    def kill(self):
        """
        Kill a running application. E.g., OrangeFS will terminate the servers,
        clients, and metadata services.

        :return: None
        """
        Kill('gds_adapter_basic_test', LocalExecInfo(env=self.env))

    def clean(self):
        """
        Destroy all data for an application. E.g., OrangeFS will delete all
        metadata and data directories in addition to the orangefs.xml file.

        :return: None
        """
        pass
