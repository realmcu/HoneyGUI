#!/usr/bin/env python3
import os
import re
import subprocess

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
        return
    
    print(f"Found {len(widgets)} demos in Kconfig.gui\n")
    
    os.chdir("../../win32_sim")
    results = []
    
    for config_name, display_name in widgets:
        print("=" * 60)
        print(f"Testing: {display_name}")
        print(f"Config: {config_name}")
        print("=" * 60)
        
        # Create .config with all features enabled
        with open(".config", "w") as f:
            f.write("CONFIG_REALTEK_HONEYGUI=y\n")
            f.write(f"CONFIG_{config_name}=y\n")
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
            print(f"✓ Build SUCCESS: {display_name}\n")
            results.append((display_name, "SUCCESS", None))
        else:
            print(f"✗ Build FAILED: {display_name}")
            error_msg = result.stderr if result.stderr else result.stdout
            print(f"Error: {error_msg[:500]}\n")
            results.append((display_name, "FAILED", error_msg))
    
    os.chdir("../..")
    
    # Summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    failed_details = []
    for name, status, error in results:
        symbol = "✓" if status == "SUCCESS" else "✗"
        print(f"{symbol} {name}: {status}")
        if status == "FAILED":
            failed_details.append((name, error))
    
    success_count = sum(1 for _, s, _ in results if s == "SUCCESS")
    print(f"\nTotal: {success_count}/{len(results)} passed")
    
    # Save detailed error log
    if failed_details:
        print("\n" + "=" * 60)
        print("FAILED BUILDS DETAILS")
        print("=" * 60)
        with open("build_errors.log", "w") as f:
            for name, error in failed_details:
                print(f"\n{name}:")
                print(error[:300] if error else "No error message")
                f.write(f"\n{'='*60}\n{name}\n{'='*60}\n{error}\n")

if __name__ == "__main__":
    main()
