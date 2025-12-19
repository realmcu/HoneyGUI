#!/usr/bin/env python3
import os
import re
import subprocess
import sys
import codecs

# Fix encoding for Windows console
if sys.platform == 'win32':
    sys.stdout = codecs.getwriter('utf-8')(sys.stdout.buffer, 'strict')
    sys.stderr = codecs.getwriter('utf-8')(sys.stderr.buffer, 'strict')

def parse_kconfig():
    """Parse Kconfig.gui to extract widget configurations"""
    widgets = []
    with open("../../Kconfig.gui", "r") as f:
        content = f.read()
    
    # Find all config entries in the choice block
    choice_match = re.search(r'choice\s+prompt "Select HoneyGUI Demo".*?endchoice', content, re.DOTALL)
    if not choice_match:
        return widgets
    
    choice_block = choice_match.group(0)
    
    # Extract config entries
    config_pattern = r'config\s+(REALTEK_BUILD_\w+)\s+bool\s+"([^"]+)"'
    for match in re.finditer(config_pattern, choice_block):
        widgets.append((match.group(1), match.group(2)))
    
    return widgets

def main():
    widgets = parse_kconfig()
    
    if not widgets:
        print("No widgets found in Kconfig.gui")
        print("\n" + "=" * 60)
        print("OVERALL RESULT")
        print("=" * 60)
        print("Result: NO TESTS")
        return
    
    print("Found {} demos in Kconfig.gui\n".format(len(widgets)))
    
    os.chdir("../../win32_sim")
    results = []
    
    for config_name, display_name in widgets:
        print("=" * 60)
        print("Testing: {}".format(display_name))
        print("Config: {}".format(config_name))
        print("=" * 60)
        
        # Create .config with all features enabled
        with open(".config", "w") as f:
            f.write("CONFIG_REALTEK_HONEYGUI=y\n")
            f.write("CONFIG_{}=y\n".format(config_name))
            f.write("CONFIG_REALTEK_BUILD_PINYIN=y\n")
            f.write("CONFIG_REALTEK_BUILD_LETTER_SHELL=y\n")
            f.write("CONFIG_REALTEK_BUILD_GUI_BOX2D=y\n")
            f.write("CONFIG_REALTEK_BUILD_LITE3D=y\n")
        
        # Clean
        subprocess.run(["scons", "-c"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        
        # Build
        print("Building...")
        result = subprocess.run(["scons", "-j8"], capture_output=True, text=True)
        
        if result.returncode == 0:
            print("[SUCCESS] Build SUCCESS: {}\n".format(display_name))
            results.append((display_name, "SUCCESS", None))
        else:
            print("[FAILED] Build FAILED: {}".format(display_name))
            error_msg = result.stderr if result.stderr else result.stdout
            print("Error: {}\n".format(error_msg[:500]))
            results.append((display_name, "FAILED", error_msg))
    
    os.chdir("../..")
    
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
        with open("build_errors.log", "w") as f:
            for name, error in failed_details:
                print("\n{}:".format(name))
                print(error[:300] if error else "No error message")
                f.write("\n{}\n{}\n{}\n".format("="*60, name, "="*60))
                f.write("{}\n".format(error))
    
    # ============ Final Overall Result ============
    print("\n" + "=" * 60)
    print("OVERALL RESULT")
    print("=" * 60)
    
    if failed_details:
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
