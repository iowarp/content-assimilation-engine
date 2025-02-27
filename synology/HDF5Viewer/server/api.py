from flask import Flask, request, jsonify
import h5py

app = Flask(__name__)

# Helper function to build HDF5 structure
def build_structure(group):
    result = {'name': group.name.split('/')[-1] or '/', 'type': 'group', 'path': group.name, 'children': []}
    for name, item in group.items():
        if isinstance(item, h5py.Group):
            result['children'].append(build_structure(item))
        elif isinstance(item, h5py.Dataset):
            result['children'].append({
                'name': name,
                'type': 'dataset',
                'path': item.name,
                'shape': item.shape,
                'dtype': str(item.dtype)
            })
    return result

# Endpoint to get file structure
@app.route('/api/get_structure')
def get_structure():
    file_path = request.args.get('file')
    try:
        with h5py.File(file_path, 'r') as f:
            structure = build_structure(f)
        return jsonify(structure)
    except Exception as e:
        return jsonify({'error': str(e)}), 500

# Endpoint to get dataset data
@app.route('/api/get_dataset')
def get_dataset():
    file_path = request.args.get('file')
    path = request.args.get('path')
    try:
        with h5py.File(file_path, 'r') as f:
            dataset = f[path]
            data = dataset[()]  # Load data (small datasets only for simplicity)
            return jsonify({
                'data': data.tolist(),
                'shape': dataset.shape,
                'dtype': str(dataset.dtype)
            })
    except Exception as e:
        return jsonify({'error': str(e)}), 500

# Endpoint to get attributes
@app.route('/api/get_attributes')
def get_attributes():
    file_path = request.args.get('file')
    path = request.args.get('path')
    try:
        with h5py.File(file_path, 'r') as f:
            obj = f[path]
            attributes = {key: obj.attrs[key].tolist() if hasattr(obj.attrs[key], 'tolist') else obj.attrs[key] for key in obj.attrs}
        return jsonify(attributes)
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=3000)
