#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
#!/usr/bin/env python3

"""
Total test script - Calls all module tests
Runs all test modules and generates a comprehensive report
"""

import os
import sys
import subprocess
import datetime
import io
import codecs
from pathlib import Path

# Fix encoding for Windows console
if sys.platform == 'win32':
    sys.stdout = codecs.getwriter('utf-8')(sys.stdout.buffer, 'strict')
    sys.stderr = codecs.getwriter('utf-8')(sys.stderr.buffer, 'strict')

class TestRunner:
    def __init__(self):
        self.script_dir = os.path.dirname(os.path.abspath(__file__))
        self.results = {}
        self.start_time = None
        self.end_time = None
        
    def run_test_module(self, module_name, script_path):
        """Run a single test module."""
        print("\n" + "="*70)
        print("Running: {}".format(module_name))
        print("Script: {}".format(script_path))
        print("="*70 + "\n")
        
        if not os.path.exists(script_path):
            print("[ERROR] Test script not found: {}\n".format(script_path))
            return {
                "status": "NOT_FOUND",
                "return_code": -1,
                "output": "Test script not found: {}".format(script_path)
            }
        
        try:
            # Run the test script with real-time output
            process = subprocess.Popen(
                [sys.executable, script_path],
                cwd=self.script_dir,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                encoding='utf-8',
                errors='ignore',
                bufsize=1  # Line buffered
            )
            
            output_lines = []
            # Read output in real-time
            for line in process.stdout:
                print(line, end='')  # Print immediately
                output_lines.append(line)
            
            # Wait for process to complete
            return_code = process.wait()
            output = ''.join(output_lines)
            
            return {
                "status": "SUCCESS" if return_code == 0 else "FAILED",
                "return_code": return_code,
                "output": output,
                "error": ""
            }
            
        except subprocess.TimeoutExpired:
            print("[ERROR] Test timeout (1 hour exceeded)\n")
            process.kill()
            return {
                "status": "TIMEOUT",
                "return_code": -1,
                "output": "Test timeout"
            }
        except Exception as e:
            print("[ERROR] {}\n".format(str(e)))
            return {
                "status": "ERROR",
                "return_code": -1,
                "output": str(e)
            }
    
    def run_all_tests(self):
        """Run all configured test modules."""
        self.start_time = datetime.datetime.now()
        
        # Define test modules
        test_modules = [
            {
                "name": "GUI Library Build Test",
                "script": "test_gui_lib_build.py",
                "description": "Test GUI library compilation with different toolchains"
            },
            {
                "name": "GUI Widgets Test",
                "script": "test_widgets.py",
                "description": "Test GUI widgets functionality"
            }
        ]
        
        print("\n" + "="*70)
        print("HoneyGUI - Comprehensive Test Suite")
        print("="*70)
        print("Start Time: {}\n".format(self.start_time.strftime('%Y-%m-%d %H:%M:%S')))
        
        # Run each test module
        for module in test_modules:
            module_name = module["name"]
            script_path = os.path.join(self.script_dir, module["script"])
            
            result = self.run_test_module(module_name, script_path)
            self.results[module_name] = result
        
        self.end_time = datetime.datetime.now()
        self.print_summary()
    
    def print_summary(self):
        """Print comprehensive test summary."""
        print("\n" + "="*70)
        print("TEST SUMMARY")
        print("="*70 + "\n")
        
        # Summary table
        print("{:<40} {:<15} {:<10}".format("Module Name", "Status", "Return Code"))
        print("-" * 70)
        
        passed_count = 0
        failed_count = 0
        error_count = 0
        
        for module_name, result in self.results.items():
            status = result["status"]
            return_code = result["return_code"]
            
            # Count results
            if status == "SUCCESS":
                passed_count += 1
                symbol = "[OK]"
            elif status == "FAILED":
                failed_count += 1
                symbol = "[FAIL]"
            else:
                error_count += 1
                symbol = "[WARN]"
            
            print("{:<40} {} {:<13} {:<10}".format(module_name, symbol, status, return_code))
        
        print("-" * 70)
        print("\nTotal Modules: {}".format(len(self.results)))
        print("Passed: {}".format(passed_count))
        print("Failed: {}".format(failed_count))
        print("Errors: {}".format(error_count))
        
        # Time information
        duration = self.end_time - self.start_time
        print("\nStart Time: {}".format(self.start_time.strftime('%Y-%m-%d %H:%M:%S')))
        print("End Time: {}".format(self.end_time.strftime('%Y-%m-%d %H:%M:%S')))
        print("Duration: {}".format(duration))
        
        # Final result
        print("\n" + "="*70)
        if failed_count == 0 and error_count == 0:
            print("FINAL RESULT: [OK] ALL TESTS PASSED")
            print("="*70)
            return 0
        else:
            print("FINAL RESULT: [FAIL] SOME TESTS FAILED")
            print("="*70)
            return 1
    
    def save_report(self, filename="test_report.txt"):
        """Save test report to file."""
        report_path = os.path.join(self.script_dir, filename)
        
        with open(report_path, "w", encoding='utf-8') as f:
            f.write("="*70 + "\n")
            f.write("HoneyGUI - Test Report\n")
            f.write("="*70 + "\n\n")
            
            f.write("Start Time: {}\n".format(self.start_time.strftime('%Y-%m-%d %H:%M:%S')))
            f.write("End Time: {}\n".format(self.end_time.strftime('%Y-%m-%d %H:%M:%S')))
            f.write("Duration: {}\n\n".format(self.end_time - self.start_time))
            
            f.write("="*70 + "\n")
            f.write("DETAILED RESULTS\n")
            f.write("="*70 + "\n\n")
            
            for module_name, result in self.results.items():
                f.write("\nModule: {}\n".format(module_name))
                f.write("Status: {}\n".format(result['status']))
                f.write("Return Code: {}\n".format(result['return_code']))
                f.write("-"*70 + "\n")
                f.write("Output:\n")
                f.write(result.get("output", "No output") + "\n")
                if result.get("error"):
                    f.write("\nError:\n")
                    f.write(result["error"] + "\n")
                f.write("="*70 + "\n")
        
        print("\nTest report saved to: {}".format(report_path))

def main():
    runner = TestRunner()
    
    try:
        runner.run_all_tests()
        runner.save_report()
        
        # Return appropriate exit code
        if runner.results:
            failed = sum(1 for r in runner.results.values() if r["status"] != "SUCCESS")
            sys.exit(1 if failed > 0 else 0)
        else:
            sys.exit(1)
            
    except KeyboardInterrupt:
        print("\n\nUser interrupted test suite")
        sys.exit(1)
    except Exception as e:
        print("\n\nFatal error: {}".format(str(e)))
        sys.exit(1)

if __name__ == "__main__":
    main()
