#!/usr/bin/env python3
import os
import sys
import glob
import shutil
import subprocess
import datetime
import io

# Fix encoding for Windows console
if sys.platform == 'win32':
    import codecs
    sys.stdout = codecs.getwriter('utf-8')(sys.stdout.buffer, 'strict')
    sys.stderr = codecs.getwriter('utf-8')(sys.stderr.buffer, 'strict')

# ============ Configuration ============

# Define toolchain configuration
# Keys correspond to directory names, values contain generator and build command
TOOL_CONFIG = {
    "armcc": {
        "generator": "MinGW Makefiles",
        "build_cmd": "mingw32-make"
    },
    "armclang": {
        "generator": "MinGW Makefiles",
        "build_cmd": "mingw32-make"
    },
    "arm-none-eabi-gcc": {
        "generator": "MinGW Makefiles",
        "build_cmd": "mingw32-make"
    }
}

def run_command(cmd, cwd):
    """Execute system command."""
    try:
        # Run command and capture output
        result = subprocess.run(
            cmd,
            cwd=cwd,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            encoding='utf-8', 
            errors='ignore'  # Prevent script crash due to encoding errors
        )
        return result.returncode, result.stdout
    except Exception as e:
        return -1, str(e)

def test_one(tool, soc, lib_dir):
    """Execute a single test case."""
    build_dir = "build_{}_{}".format(tool, soc)
    source_dir = os.path.abspath(os.path.join(lib_dir, tool))  # Get absolute path of source code
    
    print("=" * 60)
    print("Testing: {} - {}".format(tool, soc))
    print("=" * 60)
    
    cmake_file = os.path.join(lib_dir, tool, "{}.cmake".format(soc))
    if not os.path.exists(cmake_file):
        print("[FAILED] Test FAILED: {} - {}".format(tool, soc))
        print("Error: Cannot find {}\n".format(cmake_file))
        return "FAILED", "Cannot find {}".format(cmake_file)

    # Get configuration
    config = TOOL_CONFIG.get(tool)
    if not config:
        print("[FAILED] Test FAILED: {} - {}".format(tool, soc))
        print("Error: Configuration for tool {} is undefined\n".format(tool))
        return "FAILED", "Configuration for tool {} is undefined".format(tool)

    # Clean and create build directory
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)
    os.makedirs(build_dir)

    # 1. CMake Configuration Phase
    cmake_cmd = 'cmake -G "{}" -DSOC={} "{}"'.format(config["generator"], soc, source_dir)
    
    print("Configuring...")
    ret_code, output = run_command(cmake_cmd, cwd=build_dir)
    
    if ret_code != 0:
        print("[FAILED] Test FAILED: {} - {}".format(tool, soc))
        print("Error: CMake configuration error")
        print("{}\n".format(output[:500]))
        # Cleanup
        if os.path.exists(build_dir):
            try:
                shutil.rmtree(build_dir)
            except:
                pass
        return "FAILED", output

    # 2. Build Phase
    build_cmd = config["build_cmd"]
    print("Building...")
    ret_code, output = run_command(build_cmd, cwd=build_dir)

    # Clean build directory
    if os.path.exists(build_dir):
        try:
            shutil.rmtree(build_dir)
        except Exception as e:
            print("Warning: Failed to clean directory {}: {}".format(build_dir, e))

    if ret_code != 0:
        print("[FAILED] Test FAILED: {} - {}".format(tool, soc))
        print("Error: Compilation error")
        print("{}\n".format(output[:500]))
        return "FAILED", output
    else:
        print("[SUCCESS] Test SUCCESS: {} - {}\n".format(tool, soc))
        return "SUCCESS", None

def main():
    # Get the lib directory (parent of parent of this script)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    lib_dir = os.path.abspath(os.path.join(script_dir, "../../lib"))
    
    # Display start time
    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print("Test Started: {}\n".format(current_time))
    print("Library directory: {}\n".format(lib_dir))
    
    results = []
    
    # Iterate through tool directories
    for tool_name in TOOL_CONFIG.keys():
        tool_path = os.path.join(lib_dir, tool_name)
        if os.path.isdir(tool_path):
            # Scan for RTL*.cmake files
            cmake_files = glob.glob(os.path.join(tool_path, "RTL*.cmake"))
            
            for file_path in cmake_files:
                # Get filename (without extension), e.g., RTL8762D
                filename = os.path.splitext(os.path.basename(file_path))[0]
                soc_name = filename 
                
                status, error = test_one(tool_name, soc_name, lib_dir)
                results.append(("{} - {}".format(tool_name, soc_name), status, error))
        else:
            print("Skipped: Directory {} does not exist".format(tool_path))

    # Summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    failed_details = []
    for name, status, error in results:
        symbol = "[OK]" if status == "SUCCESS" else "[FAIL]"
        print("{} {}: {}".format(symbol, name, status))
        if status == "FAILED":
            failed_details.append((name, error))
    
    success_count = sum(1 for _, s, _ in results if s == "SUCCESS")
    print("\nTotal: {}/{} passed".format(success_count, len(results)))
    
    # Save detailed error log
    if failed_details:
        print("\n" + "=" * 60)
        print("FAILED BUILDS DETAILS")
        print("=" * 60)
        with open("test_errors.log", "w") as f:
            for name, error in failed_details:
                print("\n{}:".format(name))
                print(error[:300] if error else "No error message")
                f.write("\n{}\n{}\n{}\n".format("="*60, name, "="*60))
                f.write("{}\n".format(error))
    
    # ============ Final Overall Result ============
    print("\n" + "=" * 60)
    print("OVERALL RESULT")
    print("=" * 60)
    
    if len(results) == 0:
        print("Result: NO TESTS")
        sys.exit(1)
    elif failed_details:
        print("Result: FAILED")
        sys.exit(1)
    else:
        print("Result: SUCCESS")
        sys.exit(0)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nUser interrupted test")
        print("Result: INTERRUPTED")
        sys.exit(1)
