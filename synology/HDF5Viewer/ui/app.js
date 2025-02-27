Ext.define('HDF5Viewer.Application', {
    extend: 'SYNO.SDS.AppWindow',
    title: 'HDF5 Viewer',
    width: 800,
    height: 600,

    constructor: function(config) {
        this.currentFile = null;
        this.callParent([config]);
        this.initUI();
    },

    initUI: function() {
        this.add({
            layout: 'border',
            tbar: [{
                text: 'Open File',
                handler: this.openFile,
                scope: this
            }],
            items: [{
                region: 'west',
                xtype: 'treepanel',
                width: 300,
                title: 'HDF5 Structure',
                store: Ext.create('Ext.data.TreeStore', {
                    root: {
                        text: 'No file loaded',
                        expanded: true
                    }
                }),
                listeners: {
                    itemclick: this.onItemClick,
                    scope: this
                }
            }, {
                region: 'center',
                xtype: 'tabpanel',
                itemId: 'contentPanel',
                items: []
            }]
        });
    },

    openFile: function() {
        var filePicker = new SYNO.SDS.FilePicker({
            owner: this,
            callback: this.onFileSelected,
            scope: this
        });
        filePicker.show();
    },

    onFileSelected: function(file) {
        if (file) {
            this.currentFile = file.path;
            Ext.Ajax.request({
                url: 'http://' + window.location.hostname + ':3000/api/get_structure',
                params: { file: file.path },
                success: function(response) {
                    var data = Ext.decode(response.responseText);
                    this.down('treepanel').getStore().setRoot(data);
                },
                failure: function(response) {
                    Ext.Msg.alert('Error', 'Failed to load file: ' + response.statusText);
                },
                scope: this
            });
        }
    },

    onItemClick: function(view, record) {
        if (record.get('type') === 'dataset') {
            var path = record.get('path');
            var file = this.currentFile;
            var contentPanel = this.down('#contentPanel');
            contentPanel.removeAll();

            // Fetch dataset data
            Ext.Ajax.request({
                url: 'http://' + window.location.hostname + ':3000/api/get_dataset',
                params: { file: file, path: path },
                success: function(response) {
                    var result = Ext.decode(response.responseText);
                    if (result.error) {
                        contentPanel.add({ title: 'Data', html: 'Error: ' + result.error });
                        return;
                    }
                    var store = Ext.create('Ext.data.Store', {
                        fields: [{ name: 'value', type: 'auto' }],
                        data: result.data.map((val, idx) => ({ value: val }))
                    });
                    contentPanel.add({
                        title: 'Data',
                        xtype: 'grid',
                        store: store,
                        columns: [{ text: 'Value', dataIndex: 'value', flex: 1 }]
                    });
                },
                scope: this
            });

            // Fetch attributes
            Ext.Ajax.request({
                url: 'http://' + window.location.hostname + ':3000/api/get_attributes',
                params: { file: file, path: path },
                success: function(response) {
                    var attrs = Ext.decode(response.responseText);
                    contentPanel.add({
                        title: 'Attributes',
                        xtype: 'propertygrid',
                        source: attrs
                    });
                },
                scope: this
            });
        }
    }
});

// Launch the application
Ext.onReady(function() {
    new HDF5Viewer.Application();
});
