#!/usr/bin/env python3
"""
Simple test runner for snapshot restart tests
"""

import sys
import os
import subprocess
import time

def run_snapshot_test():
    """Run the snapshot restart test"""
    
    # Add the current directory to Python path
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    
    # Import the test
    from test_snapshot import TestSnapshot
    from infinity_runner import InfinityRunner
    
    # Create infinity runner
    infinity_path = "cmake-build-debug/src/infinity"  # Adjust path as needed
    config_path = "test/data/config/restart_test/test_snapshot/1.toml"
    runner = InfinityRunner(infinity_path, config_path)
    
    # Create test instance
    test = TestSnapshot()
    
    print("Running snapshot restart tests...")
    
    try:
        # Run basic restart test
        print("\n1. Testing basic snapshot restart...")
        test.test_snapshot_basic_restart(runner)
        print("✓ Basic snapshot restart test passed")
        
        # Run concurrent operations test
        print("\n2. Testing snapshot during operations...")
        test.test_snapshot_during_operations(runner)
        print("✓ Snapshot during operations test passed")
        
        # Run index test
        print("\n3. Testing snapshot with indexes...")
        test.test_snapshot_with_indexes(runner)
        print("✓ Snapshot with indexes test passed")
        
        # Run error recovery test
        print("\n4. Testing snapshot error recovery...")
        test.test_snapshot_error_recovery(runner)
        print("✓ Snapshot error recovery test passed")
        
        # Run large data test (optional - takes longer)
        print("\n5. Testing snapshot with large data...")
        test.test_snapshot_large_data(runner)
        print("✓ Snapshot with large data test passed")
        
        print("\n🎉 All snapshot restart tests passed!")
        
    except Exception as e:
        print(f"\n❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit_code = run_snapshot_test()
    sys.exit(exit_code) 