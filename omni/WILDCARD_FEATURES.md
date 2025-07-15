# Wildcard and Directory Support in OMNI

This document describes the new wildcard and directory functionality added to the OMNI Content Assimilation Engine.

## Overview

The OMNI parser now supports:
- **Wildcard patterns**: Use `*`, `?`, and `[pattern]` to match multiple files
- **Directory paths**: Specify a directory to process all files within it
- **Asynchronous processing**: Multiple files are processed concurrently when possible

## New Features

### 1. Wildcard Pattern Support

You can now use standard shell wildcards in your YAML configuration:

```yaml
data:
- path: ../data/*.csv  # Process all CSV files
  offset: 0
  size: 1000
  description:
    - csv
    - wildcard_test
```

Supported wildcards:
- `*` - Matches any sequence of characters
- `?` - Matches any single character  
- `[abc]` - Matches any character in the set
- `[a-z]` - Matches any character in the range

### 2. Directory Support

You can specify a directory to process all files within it:

```yaml
data:
- path: ../data/  # Process all files in the data directory
  offset: 0
  size: 500
  description:
    - directory
    - test
```

### 3. Asynchronous Processing

When multiple files are found for a single data entry, they are processed asynchronously:

- **Single file**: Synchronous processing (original behavior)
- **Multiple files**: Asynchronous processing using `std::async`

## Implementation Details

### File Pattern Expansion

The `ExpandFilePattern()` function handles:
1. **Wildcard detection**: Checks for `*`, `?`, `[` characters
2. **Glob expansion**: Uses POSIX `glob()` function for pattern matching
3. **Directory scanning**: Uses `std::filesystem` for directory iteration
4. **File validation**: Only processes regular files (not directories or symlinks)
5. **Sorting**: Files are sorted for consistent ordering

### Data Structure Changes

The `OmniJobConfig::DataEntry` structure was updated:
- **Before**: `std::string path` (single file)
- **After**: `std::vector<std::string> paths` (multiple files)

### Processing Logic

The processing loop now:
1. **Expands patterns**: Converts wildcards/directories to file lists
2. **Validates files**: Ensures all files exist and are accessible
3. **Chooses processing mode**: 
   - Single file → Synchronous processing
   - Multiple files → Asynchronous processing
4. **Executes MPI commands**: Each file gets its own MPI process

## Usage Examples

### Example 1: Process all CSV files
```yaml
name: csv_processing
max_scale: 8
data:
- path: ../data/*.csv
  offset: 0
  size: 1000
  description: ["csv", "batch"]
```

### Example 2: Process all files in a directory
```yaml
name: directory_processing
max_scale: 4
data:
- path: ../data/
  offset: 0
  size: 500
  description: ["directory", "all_files"]
```

### Example 3: Process specific file pattern
```yaml
name: pattern_processing
max_scale: 6
data:
- path: ../data/A46_xx.*
  offset: 0
  size: 2000
  description: ["pattern", "specific"]
```

## Error Handling

The implementation includes robust error handling:

- **Empty patterns**: Warning and skip
- **No matches**: Warning but continue processing
- **Invalid paths**: Warning and skip
- **Permission errors**: Warning and skip
- **Glob errors**: Detailed error messages with error codes

## Performance Considerations

### Asynchronous Processing
- Uses `std::async` with `std::launch::async` policy
- Each file gets its own thread for MPI command execution
- All threads are waited for completion before proceeding

### Memory Usage
- File lists are stored in memory during processing
- Large directories may consume significant memory
- Consider using more specific patterns for very large directories

### Scalability
- Each file gets its own MPI process allocation
- Total MPI processes = files × processes_per_file
- Monitor system resources when processing many files

## Testing

Use the provided test files to verify functionality:

```bash
# From build directory
./bin/wrp ../omni/config/wildcard_test.yaml
bash ../omni/config/test_wildcards.sh
```

## Migration Guide

### Existing YAML Files
Existing YAML files will continue to work without changes. The parser automatically:
1. Expands single file paths to single-element vectors
2. Uses synchronous processing for single files
3. Maintains backward compatibility

### Recommended Updates
For better performance with multiple files:
1. Use wildcards instead of listing files individually
2. Group related files in single entries
3. Use appropriate `max_scale` values for your system

## Troubleshooting

### Common Issues

1. **"No files match pattern"**
   - Check file permissions
   - Verify pattern syntax
   - Ensure files exist in specified location

2. **"Path is neither a file nor directory"**
   - Check file/directory existence
   - Verify path syntax
   - Check permissions

3. **High memory usage**
   - Use more specific patterns
   - Process files in smaller batches
   - Monitor system resources

4. **MPI process limits**
   - Reduce `max_scale` values
   - Use fewer files per entry
   - Check system MPI limits

## Future Enhancements

Potential improvements for future versions:
- **Recursive directory scanning**: Process subdirectories
- **File filtering**: Exclude specific file types
- **Batch size limits**: Limit concurrent processing
- **Progress reporting**: Show processing progress
- **Error recovery**: Retry failed files 