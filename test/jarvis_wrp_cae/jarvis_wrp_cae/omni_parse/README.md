# OMNI Parse Jarvis Package

This package provides a Jarvis interface for running OMNI Content Assimilation Engine jobs. OMNI processes data files according to YAML job specifications and supports MPI scaling for efficient data processing.

## Overview

The `omni_parse` package allows you to:
- Execute OMNI jobs through Jarvis pipeline management
- Support interception through the `mod_env` environment
- Integrate OMNI processing with other CAE components
- Manage OMNI job configurations through Jarvis

## Package Configuration

### Required Parameters

- **omni_yaml**: Path to the OMNI YAML configuration file (required)

### Optional Parameters

- **wrp_path**: Path to the wrp executable (auto-detected if not specified)
- **cwd**: Working directory to run omni from (defaults to build directory)

## Usage Examples

### Basic Usage

```bash
# Configure and run an OMNI job
jarvis ppl create test_omni
jarvis ppl append omni_parse omni_yaml=/path/to/job.yaml
jarvis ppl run
```

### With Custom wrp Path

```bash
jarvis ppl append omni_parse omni_yaml=/path/to/job.yaml wrp_path=/custom/path/to/wrp
```

### With Custom Working Directory

```bash
jarvis ppl append omni_parse omni_yaml=/path/to/job.yaml cwd=/custom/working/dir
```

## Pipeline Script Example

```yaml
name: omni_demo
env: hermes
pkgs:
  - pkg_type: omni_parse
    pkg_name: omni_parse
    omni_yaml: ${HOME}/omni/config/demo_job.yaml
    sleep: 1
```

## Pipeline Test Example

```yaml
config:
  name: omni_scaling_test
  env: hermes
  pkgs:
    - pkg_type: omni_parse
      pkg_name: omni_parse
      omni_yaml: ${HOME}/omni/config/demo_job.yaml
vars:
  omni_parse.omni_yaml: 
    - ${HOME}/omni/config/quick_test.yaml
    - ${HOME}/omni/config/demo_job.yaml
loop:
  - [omni_parse.omni_yaml]
repeat: 3
output: "${SHARED_DIR}/omni_results"
```

## Integration with Interceptors

The `omni_parse` package uses `mod_env` for environment variables, making it compatible with CAE interceptors:

```yaml
name: omni_with_interception
env: hermes
pkgs:
  - pkg_type: cae_adapter
    pkg_name: cae_adapter
    posix: true
  - pkg_type: omni_parse
    pkg_name: omni_parse
    omni_yaml: ${HOME}/omni/config/demo_job.yaml
```

## Auto-Detection Features

The package automatically detects:
- **wrp executable**: Searches in `build/bin/`, `bin/`, current directory, and PATH
- **Working directory**: Defaults to `build/` directory if it exists
- **YAML file validation**: Ensures the OMNI configuration file exists and is readable

## Error Handling

The package provides comprehensive error checking:
- Validates OMNI YAML file existence
- Verifies wrp executable path and permissions
- Checks working directory accessibility
- Reports execution status and exit codes

## Output and Statistics

The package collects:
- Exit code from the OMNI execution
- Runtime statistics
- Stdout/stderr if configured
- Execution status reporting

## Common OMNI YAML Configurations

The package works with any valid OMNI YAML configuration. See the main OMNI documentation for YAML format details and example configurations in `omni/config/`.

## Troubleshooting

### wrp executable not found
- Ensure the CAE project is built (`make` or `ninja`)
- Check that `build/bin/wrp` exists
- Specify explicit `wrp_path` if in non-standard location

### YAML file not found
- Use absolute paths for `omni_yaml` parameter
- Verify file permissions are readable
- Check that the YAML syntax is valid

### Permission denied
- Ensure wrp executable has execute permissions
- Check working directory write permissions
- Verify data file access permissions 