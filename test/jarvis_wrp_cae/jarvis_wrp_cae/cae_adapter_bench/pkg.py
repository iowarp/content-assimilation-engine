"""
This module provides classes and methods to launch the HermesApiBench application.
HermesApiBench is ....
"""
from jarvis_cd.basic.pkg import Application
from jarvis_util import *


class CaeAdapterBench(Application):
    """
    This class provides methods to launch the CAE adapter bench application.
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
                'name': 'mode',
                'msg': 'The benchmark to run',
                'type': str,
                'default': None,
                'choices': ['putget', 'pputget', 'create_bkt',
                            'get_bkt', 'del_bkt'],
            },
            {
                'name': 'blobs_per_rank',
                'msg': 'The number of blobs to create per-rank',
                'type': str,
                'default': '1',
            },
            {
                'name': 'bkts_per_rank',
                'msg': 'The number of buckets to create per-rank',
                'type': str,
                'default': '1',
            },
            {
                'name': 'blob_size',
                'msg': 'The size of a single blob',
                'type': str,
                'default': '1m',
            },
            {
                'name': 'part_size',
                'msg': 'The size of a partial op on a blob',
                'type': str,
                'default': '4k',
            },
            {
                'name': 'blobs_per_bkt',
                'msg': 'The number of blobs to create in a single bucket',
                'type': str,
                'default': '1',
            },
            {
                'name': 'nprocs',
                'msg': 'The number of processes to spawn',
                'type': int,
                'default': 1,
            },
            {
                'name': 'ppn',
                'msg': 'The number of processes per node',
                'type': int,
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
        mode = self.config['mode']
        cmd = [f'cae_adapter_bench {mode}']
        if mode == 'putget':
            cmd += [
                self.config['blob_size'],
                self.config['blobs_per_rank']
            ]
        elif mode == 'pputget':
            cmd += [
                self.config['blob_size'],
                self.config['part_size'],
                self.config['blobs_per_rank']
            ]
        elif mode == 'create_bkt':
            cmd += [
                self.config['bkts_per_rank']
            ]
        elif mode == 'get_bkt':
            cmd += [
                self.config['bkts_per_rank']
            ]
        elif mode == 'del_bkt':
            cmd += [
                self.config['bkts_per_rank'],
                self.config['blobs_per_bkt'],
            ]
        cmd = ' '.join(cmd)
        Exec(cmd, MpiExecInfo(nprocs=self.config['nprocs'],
                              env=self.env,
                              hosts=self.jarvis.hostfile,
                              ppn=self.config['ppn'],
                              do_dbg=self.config['do_dbg'],
                              dbg_port=self.config['dbg_port']))

    def stop(self):
        """
        Stop a running application. E.g., OrangeFS will terminate the servers,
        clients, and metadata services.

        :return: None
        """
        pass

    def clean(self):
        """
        Destroy all data for an application. E.g., OrangeFS will delete all
        metadata and data directories in addition to the orangefs.xml file.

        :return: None
        """
        pass
