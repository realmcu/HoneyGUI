import pandas as pd
import re
import os
import glob

# Specify the parent directory of the log files (two levels up)
directory_path = os.path.abspath(os.path.join(os.path.dirname(__file__), './../../'))

# Define the prefix
file_prefix = 'auto_test'
print(f"Looking for files with prefix '{file_prefix}' in directory: {directory_path}")

# List files in the target directory
all_files_in_dir = os.listdir(directory_path)
print(f"Files in directory {directory_path}: {all_files_in_dir}")

# Find files matching the prefix
matching_files = glob.glob(os.path.join(directory_path, f"{file_prefix}*.txt"))
print(f"Matching files: {matching_files}")

if not matching_files:
    raise FileNotFoundError(f"No files found with prefix {file_prefix} in directory {directory_path}")

# Select the first matching file
log_file_path = matching_files[0]
output_excel_path = os.path.join(os.getcwd(), 'realgui', 'test', 'widget_size.xlsx')

print(f"Using log file: {log_file_path}")
print(f"Output will be saved to: {output_excel_path}")

# Define a regular expression to match log entries
log_pattern = re.compile(r'name:\s+(\w+)\s+size:\s+(\d+)')

# Initialize an empty list to store extracted data
data = []

# Flags to determine when to start and stop processing
process_data = False

# Read the log file and extract required information
with open(log_file_path, 'r', encoding='utf-8') as file:
    for line in file:
        if "gui_test_case_obj_000 START" in line:
            process_data = True
            print(f"Started processing at line: {line.strip()}")

        if process_data:
            match = re.search(log_pattern, line)
            if match:
                name = match.group(1)
                size = int(match.group(2))
                data.append({'name': name, 'size': size})

        if "gui_test_case_obj_000 PASS" in line:
            process_data = False
            print(f"Stopped processing at line: {line.strip()}")
            break

# Convert the extracted data to a DataFrame
df = pd.DataFrame(data)

# Save the DataFrame as an Excel file
df.to_excel(output_excel_path, index=False)

print(f"Data has been successfully extracted and saved to {output_excel_path}")
