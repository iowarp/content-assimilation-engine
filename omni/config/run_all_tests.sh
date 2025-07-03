#!/bin/bash

# OMNI Test Suite - Run all test cases
# This script should be run from the build/ directory

echo "=========================================="
echo "OMNI Content Assimilation Engine Test Suite"
echo "=========================================="

# Check if we're in the correct directory
if [ ! -f "bin/wrp" ]; then
    echo "❌ Error: wrp executable not found."
    echo "Please run this script from the build/ directory."
    echo "Usage: cd build && bash ../omni/config/run_all_tests.sh"
    exit 1
fi

# Check if data files exist
if [ ! -f "../data/A46_xx.csv" ]; then
    echo "❌ Error: Test data files not found."
    echo "Please ensure the data/ directory exists with test files."
    exit 1
fi

echo "✅ Prerequisites check passed"
echo ""

# Test Case 1: Quick Validation Test
echo "=== Test Case 1: Quick Validation ==="
echo "Testing basic functionality with minimal data..."
echo "Command: ./bin/wrp ../omni/config/quick_test.yaml"
echo ""

if ./bin/wrp ../omni/config/quick_test.yaml; then
    echo "✅ Quick test PASSED"
else
    echo "❌ Quick test FAILED"
    exit 1
fi

echo ""
echo "=== Test Case 2: Multi-File Demonstration ==="
echo "Testing multi-file processing with different formats..."
echo "Command: ./bin/wrp ../omni/config/demo_job.yaml"
echo ""

if ./bin/wrp ../omni/config/demo_job.yaml; then
    echo "✅ Demo job PASSED"
else
    echo "❌ Demo job FAILED"
    exit 1
fi

echo ""
echo "=========================================="
echo "🎉 ALL TESTS PASSED SUCCESSFULLY!"
echo "=========================================="
echo ""
echo "Your OMNI installation is working correctly."
echo "You can now:"
echo "  1. Create custom jobs using config/example_job.yaml as a template"
echo "  2. Process your own data files"
echo "  3. Explore the scaling features with larger files"
echo ""
echo "For more information, see the README.md file." 