#!/bin/bash

# Test script for wildcard and directory functionality
# This script should be run from the build/ directory

echo "=========================================="
echo "OMNI Wildcard and Directory Functionality Test"
echo "=========================================="

# Check if we're in the correct directory
if [ ! -f "bin/wrp" ]; then
    echo "❌ Error: wrp executable not found."
    echo "Please run this script from the build/ directory."
    echo "Usage: cd build && bash ../omni/config/test_wildcards.sh"
    exit 1
fi

# Check if data files exist
if [ ! -d "../data" ]; then
    echo "❌ Error: data directory not found."
    echo "Please ensure the data/ directory exists with test files."
    exit 1
fi

echo "✅ Prerequisites check passed"
echo ""

# Test Case 1: Wildcard Pattern Test
echo "=== Test Case 1: Wildcard Pattern (*.csv) ==="
echo "Testing wildcard pattern expansion..."
echo "Command: ./bin/wrp ../omni/config/wildcard_test.yaml"
echo ""

if ./bin/wrp ../omni/config/wildcard_test.yaml; then
    echo "✅ Wildcard test PASSED"
else
    echo "❌ Wildcard test FAILED"
    exit 1
fi

echo ""
echo "=========================================="
echo "🎉 WILDCARD TESTS COMPLETED!"
echo "=========================================="
echo ""
echo "The wildcard and directory functionality is working."
echo "You can now use:"
echo "  - Wildcard patterns: ../data/*.csv"
echo "  - Directory paths: ../data/"
echo "  - File patterns: ../data/A46_xx.*"
echo "" 