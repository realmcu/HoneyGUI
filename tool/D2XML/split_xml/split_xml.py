import os
import re

# ============================================================================
# SCRIPT DESCRIPTION
#   Name: split_screens.py
#   Function: 1. Parses 'template.xml' and splits each <SCREEN> block into a
#                separate XML file.
#             2. Creates a single 'view_descriptor.xml' file containing a
#                summary of all found screens.
#   Requirements: Python 3.x
#   Usage:
#     1. Place this script in the same directory as your '__template.xml' file.
#     2. Open a terminal/CMD in that directory and run: python split_screens.py
# ============================================================================

# --- 1. Configuration ---
INPUT_FILENAME = "__template.xml"
DESCRIPTOR_FILENAME = "view_descriptor.xml"  # [NEW] Name for the new descriptor file
XML_HEADER = '<?xml version="1.0" encoding="UTF-8"?>\n'

def sanitize_filename(name: str) -> str:
    """
    Removes characters that are illegal in Windows filenames.
    """
    return re.sub(r'[\\/*?:"<>|]', "", name).strip()

def cleanup_xml_files():
    """
    Delete all XML files in the current directory except '__template.xml'
    """
    print("[INFO] Cleaning up existing XML files (except '__template.xml')...")
    files_deleted = 0
    
    for filename in os.listdir('.'):
        if filename.endswith('.xml') and filename != INPUT_FILENAME:
            try:
                os.remove(filename)
                print(f"[CLEANUP] Removed: {filename}")
                files_deleted += 1
            except Exception as e:
                print(f"[WARNING] Could not remove {filename}: {e}")
    
    if files_deleted > 0:
        print(f"[INFO] Cleanup completed. Removed {files_deleted} XML file(s).")
    else:
        print("[INFO] No XML files found to remove (except '__template.xml').")
    print()

def split_screens():
    """
    Main function to read, parse, and generate all required XML files.
    """
    # [NEW] Clean up existing XML files before starting
    cleanup_xml_files()
    
    if not os.path.exists(INPUT_FILENAME):
        print(f"[ERROR] File not found: '{INPUT_FILENAME}'")
        print("Please make sure 'template.xml' is in the same folder as this script.")
        input("Press Enter to exit...")
        return

    print(f"[INFO] Starting to parse file: '{INPUT_FILENAME}'")
    print()

    # [NEW] List to store entries for view_descriptor.xml
    descriptor_entries = []

    try:
        with open(INPUT_FILENAME, 'r', encoding='utf-8') as f:
            content = f.read()

        # Split the file by the opening <SCREEN tag.
        screen_blocks = content.split('<SCREEN')
        
        files_created = 0

        for i, block_content in enumerate(screen_blocks):
            if i == 0:  # Skip the part before the first <SCREEN>
                continue

            full_block = '<SCREEN' + block_content

            try:
                end_of_open_tag = full_block.find('>') + 1
                start_of_children = full_block.find('<', end_of_open_tag)
                
                screen_name = full_block[end_of_open_tag:start_of_children].strip()

                if not screen_name:
                    print(f"[WARNING] Found a SCREEN block (number {i}) with no name. Skipping.")
                    continue
                
                # --- Task 1: Create individual screen XML file ---
                sanitized_name = sanitize_filename(screen_name)
                output_filename = f"{sanitized_name}.xml"

                print(f"[ACTION] Original Name: '{screen_name}' -> Creating File: '{output_filename}'")
                
                with open(output_filename, 'w', encoding='utf-8') as out_f:
                    out_f.write(XML_HEADER)
                    out_f.write(full_block.strip())
                
                files_created += 1

                # --- [NEW] Task 2: Prepare the entry for view_descriptor.xml ---
                # Based on the user's requested format.
                # Indentation is added for readability in the final XML.
                view_entry = f'  <VIEW\n        keep = "0">{screen_name}\n  </VIEW>'
                descriptor_entries.append(view_entry)

            except Exception as e:
                print(f"[ERROR] Could not process block number {i}. Error: {e}")

        # --- [NEW] After the loop, create the view_descriptor.xml file ---
        if descriptor_entries:
            print()
            print(f"[ACTION] Creating summary file: '{DESCRIPTOR_FILENAME}'")
            
            # Join all entries with a newline
            all_views_content = "\n".join(descriptor_entries)
            
            # Wrap the content in a single root element to create a valid XML file
            final_descriptor_content = (
                f"{XML_HEADER}"
                f"<ViewDescriptors>\n"
                f"{all_views_content}\n"
                f"</ViewDescriptors>\n"
            )
            
            with open(DESCRIPTOR_FILENAME, 'w', encoding='utf-8') as desc_f:
                desc_f.write(final_descriptor_content)
            
            print(f"[SUCCESS] Summary file '{DESCRIPTOR_FILENAME}' created.")

        print()
        if files_created > 0:
            print(f"[SUCCESS] Script finished processing. Created {files_created} screen file(s).")
        else:
            print("[INFO] Script finished, but no valid SCREEN blocks were found to create files.")

    except Exception as e:
        print(f"[FATAL ERROR] An unexpected error occurred: {e}")
    
    input("Press Enter to exit...")

if __name__ == "__main__":
    split_screens()