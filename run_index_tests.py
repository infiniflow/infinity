#!/usr/bin/env python3
"""
Index Test Runner for Infinity
Runs specific SLT test files for index functionality including BMP, HNSW, IVF, and secondary indexes
"""

import argparse
import os
import subprocess
import sys
import time
from pathlib import Path
from typing import List, Dict, Tuple


class IndexTestRunner:
    def __init__(self, sqllogictest_bin: str = "sqllogictest", test_dir: str = "test/sql"):
        self.sqllogictest_bin = sqllogictest_bin
        self.test_dir = test_dir
        self.results: Dict[str, Dict] = {}
        
    def check_sqllogictest_binary(self) -> bool:
        """Check if sqllogictest binary exists and is executable"""
        try:
            result = subprocess.run([self.sqllogictest_bin, "--version"], 
                                  capture_output=True, text=True, timeout=10)
            if result.returncode == 0:
                print(f"✓ Found sqllogictest binary: {self.sqllogictest_bin}")
                return True
        except (subprocess.TimeoutExpired, FileNotFoundError, subprocess.CalledProcessError):
            pass
        
        print(f"✗ sqllogictest binary not found or not executable: {self.sqllogictest_bin}")
        print("Please ensure sqllogictest is installed and in your PATH")
        return False
    
    def get_test_files(self) -> List[str]:
        """Get the list of test files to run"""
        test_files = [
            "test/sql/index/test_bmp_restore_empty.slt",
            "test/sql/index/test_bmp.slt",
            "test/sql/index/test_hnsw_restore_empty.slt",
            "test/sql/index/test_hnsw_restore.slt",
            "test/sql/index/test_hnsw.slt",
            "test/sql/index/test_index_comment_restore_empty.slt",
            "test/sql/index/test_index_comment_restore.slt",
            "test/sql/index/test_index_comment.slt",
            "test/sql/index/test_index.slt",
            "test/sql/index/test_ivf_restore_empty.slt",
            "test/sql/index/test_ivf_restore.slt",
            "test/sql/index/test_ivf.slt",
            "test/sql/index/test_secondary_index_restore_empty.slt",
            "test/sql/index/test_secondary_index_restore.slt",
            "test/sql/index/test_secondary_index.slt"
        ]
        
        # Check which files exist
        existing_files = []
        missing_files = []
        
        for test_file in test_files:
            if os.path.exists(test_file):
                existing_files.append(test_file)
            else:
                missing_files.append(test_file)
        
        if missing_files:
            print("⚠️  Warning: The following test files were not found:")
            for file in missing_files:
                print(f"   - {file}")
            print()
        
        return existing_files
    
    def run_single_test(self, test_file: str, verbose: bool = False) -> Tuple[bool, str, str]:
        """Run a single test file"""
        print(f"Running: {test_file}")
        
        try:
            # Run the test with timeout
            process = subprocess.run(
                [self.sqllogictest_bin, test_file],
                capture_output=True,
                text=True,
                timeout=300  # 5 minutes timeout
            )
            
            success = process.returncode == 0
            stdout = process.stdout
            stderr = process.stderr
            
            if verbose:
                if stdout:
                    print("STDOUT:")
                    print(stdout)
                if stderr:
                    print("STDERR:")
                    print(stderr)
            
            return success, stdout, stderr
            
        except subprocess.TimeoutExpired:
            return False, "", "Test timed out after 5 minutes"
        except Exception as e:
            return False, "", f"Error running test: {str(e)}"
    
    def run_tests(self, verbose: bool = False, parallel: bool = False) -> Dict[str, Dict]:
        """Run all the specified index tests"""
        print("=" * 80)
        print("INFINITY INDEX TEST RUNNER")
        print("=" * 80)
        
        # Check if sqllogictest binary is available
        if not self.check_sqllogictest_binary():
            return {}
        
        # Get test files
        test_files = self.get_test_files()
        if not test_files:
            print("❌ No test files found to run!")
            return {}
        
        print(f"\n📋 Found {len(test_files)} test files to run:")
        for test_file in test_files:
            print(f"   - {test_file}")
        print()
        
        # Run tests
        start_time = time.time()
        passed = 0
        failed = 0
        
        for i, test_file in enumerate(test_files, 1):
            print(f"[{i}/{len(test_files)}] ", end="")
            
            success, stdout, stderr = self.run_single_test(test_file, verbose)
            
            # Store results
            self.results[test_file] = {
                'success': success,
                'stdout': stdout,
                'stderr': stderr,
                'duration': time.time() - start_time
            }
            
            if success:
                print(f"✅ PASSED: {test_file}")
                passed += 1
            else:
                print(f"❌ FAILED: {test_file}")
                failed += 1
                if not verbose and stderr:
                    print(f"   Error: {stderr[:200]}...")
            
            print("-" * 60)
        
        total_time = time.time() - start_time
        
        # Print summary
        print("\n" + "=" * 80)
        print("TEST SUMMARY")
        print("=" * 80)
        print(f"Total tests: {len(test_files)}")
        print(f"Passed: {passed}")
        print(f"Failed: {failed}")
        print(f"Success rate: {(passed/len(test_files)*100):.1f}%")
        print(f"Total time: {total_time:.2f} seconds")
        print("=" * 80)
        
        # Print detailed results for failed tests
        if failed > 0:
            print("\nFAILED TESTS DETAILS:")
            print("-" * 40)
            for test_file, result in self.results.items():
                if not result['success']:
                    print(f"\n❌ {test_file}")
                    if result['stderr']:
                        print(f"Error: {result['stderr']}")
        
        return self.results
    
    def generate_report(self, output_file: str = None):
        """Generate a detailed test report"""
        if not self.results:
            print("No test results to report")
            return
        
        report_lines = [
            "# Infinity Index Test Report",
            f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}",
            "",
            "## Summary",
            f"- Total tests: {len(self.results)}",
            f"- Passed: {sum(1 for r in self.results.values() if r['success'])}",
            f"- Failed: {sum(1 for r in self.results.values() if not r['success'])}",
            "",
            "## Test Results",
            ""
        ]
        
        for test_file, result in self.results.items():
            status = "✅ PASS" if result['success'] else "❌ FAIL"
            report_lines.append(f"### {test_file}")
            report_lines.append(f"**Status:** {status}")
            report_lines.append(f"**Duration:** {result['duration']:.2f}s")
            
            if result['stderr']:
                report_lines.append("**Error:**")
                report_lines.append(f"```")
                report_lines.append(result['stderr'])
                report_lines.append("```")
            
            report_lines.append("")
        
        report_content = "\n".join(report_lines)
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report_content)
            print(f"📄 Report saved to: {output_file}")
        else:
            print(report_content)


def main():
    parser = argparse.ArgumentParser(
        description="Run Infinity index tests using SQL Logic Test",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python run_index_tests.py
  python run_index_tests.py --verbose
  python run_index_tests.py --sqllogictest /path/to/sqllogictest
  python run_index_tests.py --report report.md
        """
    )
    
    parser.add_argument(
        "--sqllogictest", "-s",
        default="sqllogictest",
        help="Path to sqllogictest binary (default: sqllogictest)"
    )
    
    parser.add_argument(
        "--test-dir", "-t",
        default="test/sql",
        help="Test directory path (default: test/sql)"
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Verbose output showing all stdout/stderr"
    )
    
    parser.add_argument(
        "--report", "-r",
        help="Generate detailed report to specified file"
    )
    
    parser.add_argument(
        "--parallel", "-p",
        action="store_true",
        help="Run tests in parallel (experimental)"
    )
    
    args = parser.parse_args()
    
    # Create test runner
    runner = IndexTestRunner(
        sqllogictest_bin=args.sqllogictest,
        test_dir=args.test_dir
    )
    
    # Run tests
    results = runner.run_tests(verbose=args.verbose, parallel=args.parallel)
    
    # Generate report if requested
    if args.report:
        runner.generate_report(args.report)
    
    # Exit with appropriate code
    failed_count = sum(1 for r in results.values() if not r['success'])
    sys.exit(failed_count)


if __name__ == "__main__":
    main() 