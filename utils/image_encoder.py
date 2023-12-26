"""
Uses imagemagick to convert the images into the correct size and format
and then spits the images out into a .txt file in this format:
- Each ascii character worth a nibble (b bits, half a byte)
- Yes, this is shockingly inefficient
- There must be an integer number of bytes within the file
- First byte is the width of the image (so max width is 256)
- Second byte is the height of the image (so max height is 256)
- Subsequent bytes are the data in the image, left to right and top to bottom

Use the OPTIONS constant to change what gets converted
"""

# OPTIONS: input file name, convert to this width, convert to this height, output to this txt file
OPTIONS = [
    ["example1.jpg", 128, 128, "example1.txt"],
    ["example2.jpg", 60, 60, "example2.txt"],
    ["test.png", 3, 3, "test.txt"],
]

from PIL import Image
import os
from time import sleep

# Currently only supports RGB565

def main():
    if not os.path.exists("output"):
        os.makedirs("output")

    for row in OPTIONS:
        source_file = os.path.join("images_to_encode", row[0])
        output_image_width = row[1]
        output_image_height = row[2]
        output_image_file_name = row[3]

        if output_image_width > 0xFFFF:
            output_image_width = 0xFFFF
        elif output_image_width < 0x0000:
            output_image_width = 0x0000

        if output_image_height > 0xFFFF:
            output_image_height = 0xFFFF
        elif output_image_height < 0x0000:
            output_image_height = 0x0000

        # Open the image
        image = Image.open(source_file, 'r')
        # Resize the image
        resized_image = image.resize((output_image_width, output_image_height))
        # Ensure the image is in RGB format
        rgb_image = resized_image.convert("RGB")
        # Convert the pixels into a list or a tuple
        pixel_values = tuple(resized_image.getdata())
        
        txt_file = open(os.path.join("output", output_image_file_name), 'w')
        # Write the image width and height
        txt_file.write(convert_byte_to_ascii(output_image_width))
        txt_file.write(convert_byte_to_ascii(output_image_height))

        for pixel in pixel_values:
            r = int((float(pixel[0]) / 255.0) * 0b11111) << 11
            g = int((float(pixel[1]) / 255.0) * 0b111111) << 5
            b = int((float(pixel[2]) / 255.0) * 0b11111)
            # print(bin(r), bin(g), bin(b))
            rgb565 = r + g + b
            byte1 = (rgb565 & 0b1111111100000000) >> 8
            byte2 = rgb565 & 0b0000000011111111
            txt_file.write(convert_byte_to_ascii(byte1))
            txt_file.write(convert_byte_to_ascii(byte2))

        print("First value in", row[0], "is", pixel_values[0])

# Input a value from 0 to 255
# Output is a string with two ascii characters
def convert_byte_to_ascii(byte) -> str:
    if byte > 0xFFFF:
        byte = 0xFFFF
    elif byte < 0x0000:
        byte = 0x0000
    nibble1 = ((byte & 0b11110000) >> 4) + 32
    nibble2 = (byte & 0b00001111) + 32
    char1 = chr(nibble1)
    char2 = chr(nibble2)
    return char1 + char2

if __name__ == '__main__':
    main()
