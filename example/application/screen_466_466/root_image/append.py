#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os

def append_string_to_bin(
        input_path: str, output_path: str, text: str, encoding: str = 'utf-8')-> str:


    with open(input_path, 'rb') as f_in:
        original_data = f_in.read()
        
    

    text_bytes = text.encode(encoding)
    

    new_data = original_data + text_bytes

    


    with open(output_path, 'wb') as f_out:
        f_out.write(new_data)
    
    file_size = len(new_data) 
    
    print(f"{output_path}")
    return str(file_size)


if __name__ == "__main__":

    folder = os.path.dirname(os.path.abspath(__file__))
    

    original = "root_0x00950000.bin"
    version = "1_0_1"
    name_append = "HoneyGUI_APP_"
    append = "HoneyGUI_APP_"+version+"_"
    append_text = append+"\0"
    out = name_append+original
    version_file_name = name_append+".version"
    input_file  = os.path.join(folder, "root_0x00950000.bin") 
    output_file = os.path.join(folder, out)
    version_file = os.path.join(folder, version_file_name)

    
    
    size_str = append_string_to_bin(input_file, output_file, append_text)
    append = "#define UI_VERSION_STRING \""+append+"\" \n"+ "#define UI_USERDATA_LENGTH ("+ size_str +")"
    version_bytes = append.encode('utf-8')
    with open(version_file, 'wb') as f_out_v:
         f_out_v.write(version_bytes)