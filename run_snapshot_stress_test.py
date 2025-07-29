#!/usr/bin/env python3
"""
Stress test script for snapshot functionality.
Runs the specified test 100 times and stops if it fails.
"""

import subprocess
import sys
import time
import os

def run_test(test_command, max_iterations=100):
    """
    Run the test multiple times and stop if it fails.
    
    Args:
        test_command: The pytest command to run
        max_iterations: Maximum number of iterations (default: 100)
    """
    print(f"Starting stress test: {test_command}")
    print(f"Will run up to {max_iterations} times, stopping on first failure")
    print("=" * 60)
    
    success_count = 0
    failure_count = 0
    
    for iteration in range(1, max_iterations + 1):
        print(f"\n--- Iteration {iteration}/{max_iterations} ---")
        print(f"Time: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        
        try:
            # Run the test command
            result = subprocess.run(
                test_command,
                shell=True,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout per test
            )
            
            if result.returncode == 0:
                success_count += 1
                print(f"✅ PASSED (Success: {success_count}, Failures: {failure_count})")
            else:
                failure_count += 1
                print(f"❌ FAILED (Success: {success_count}, Failures: {failure_count})")
                print("\n--- Test Output ---")
                print(result.stdout)
                print("\n--- Error Output ---")
                print(result.stderr)
                print("\n--- Stopping on first failure ---")
                break
                
        except subprocess.TimeoutExpired:
            failure_count += 1
            print(f"⏰ TIMEOUT (Success: {success_count}, Failures: {failure_count})")
            print("Test timed out after 5 minutes")
            break
        except Exception as e:
            failure_count += 1
            print(f"💥 ERROR (Success: {success_count}, Failures: {failure_count})")
            print(f"Exception: {e}")
            break
    
    # Print final summary
    print("\n" + "=" * 60)
    print("STRESS TEST SUMMARY")
    print("=" * 60)
    print(f"Total iterations: {iteration}")
    print(f"Successful runs: {success_count}")
    print(f"Failed runs: {failure_count}")
    print(f"Success rate: {(success_count/iteration)*100:.1f}%")
    
    if failure_count == 0:
        print("\n🎉 All tests passed! The snapshot functionality is stable.")
        return True
    else:
        print(f"\n⚠️  Test failed after {success_count} successful runs.")
        return False

def main():
    """Main function to run the stress test."""
    # The test command to run
    test_command = "python -m pytest python/test_pysdk/test_table_snapshot.py::TestSnapshot::test_snapshot_large_table -s -v"
    
    # Check if we're in the right directory
    if not os.path.exists("python/test_pysdk/test_table_snapshot.py"):
        print("❌ Error: test_table_snapshot.py not found!")
        print("Please run this script from the infinity project root directory.")
        sys.exit(1)
    
    # Run the stress test
    success = run_test(test_command, max_iterations=100)
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main() 