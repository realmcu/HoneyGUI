import os
from PIL import Image
import subprocess  # For opening the folder on different operating systems

def resize_images_in_folder(input_folder, output_folder, new_size):
    """
    Resize all images in the input folder and save them to the output folder.

    :param input_folder: Path to the folder containing the original images.
    :param output_folder: Path where the resized images will be saved.
    :param new_size: A tuple representing the new size (width, height) of the images.
    """
    # Create the output folder if it doesn't exist
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # Iterate over all the files in the input folder
    for filename in os.listdir(input_folder):
        # Check if the file is an image
        if any(filename.lower().endswith(ext) for ext in ['.jpg', '.jpeg', '.png', '.gif']):
            # Construct the full paths for the input and output images
            input_image_path = os.path.join(input_folder, filename)
            output_image_path = os.path.join(output_folder, filename)

            try:
                # Open the image and resize it
                with Image.open(input_image_path) as img:
                    resized_img = img.resize(new_size)

                # Save the resized image to the output folder
                resized_img.save(output_image_path)

                print(f"Resized '{input_image_path}' to '{output_image_path}'")

            except Exception as e:
                # Print any errors that occurred during resizing
                print(f"Error resizing '{input_image_path}': {e}")

# Open the output folder using the appropriate method for the operating system
def open_folder(folder_path):
    if os.name == 'nt':  # Windows
        subprocess.Popen(f'explorer "{folder_path}"')
    elif os.name == 'posix':  # MacOS, Linux, etc.
        subprocess.Popen(['open', folder_path])


input_folder = r'C:\Users\triton_yu\Pictures\launcher3\icons'  # Windows format path with raw string
output_folder = r'C:\Users\triton_yu\Pictures\launcher3\icons\ICON100'  # Windows format path with raw string
new_size = (100, 100)  # The new size (width, height) for the resized images
# Resize the images in the input folder and save them to the output folder
resize_images_in_folder(input_folder, output_folder, new_size)

# Open the output folder
open_folder(output_folder)