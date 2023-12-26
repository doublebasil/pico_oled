# This file is just for me to check that I've coded the image_encoder correctly

import os
from PIL import Image

def main():
    txt_file = open(os.path.join("output", "example1.txt"), 'r')
    contents = txt_file.read()

    imageWidth = ((ord(contents[0])-32)<<4)+(ord(contents[1])-32)
    print("Detected image width as", imageWidth)
    imageHeight = ((ord(contents[2])-32)<<4)+(ord(contents[3])-32)
    print("Detected image height as", imageHeight)

    if ( ( imageWidth * imageHeight * 4 ) + 4 ) == len(contents):
        print("Total file length is", len(contents), "which is as expected")
    else:
        print("Total file length is", len(contents), "when", ( imageWidth * imageHeight * 4 ) + 4, "was expected")
        return

    im = Image.new("RGB", size=(imageWidth, imageHeight))

    pixelStartIndex = 4
    x = 0
    y = 0
    for pixelNumber in range(0, imageWidth*imageHeight):
        rgb565 = (char2val(contents[pixelStartIndex])<<12)+(char2val(contents[pixelStartIndex+1])<<8)+(char2val(contents[pixelStartIndex+2])<<4)+char2val(contents[pixelStartIndex+3])
        # binaryVal = ("0"*(16-len(bin(rgb565)[2:]))) + bin(rgb565)[2:]
        # print(rgb565, "=", binaryVal)
        # print(rgb565 & 0b1111100000000000)
        # print( (rgb565 & 0b1111100000000000) >> 11)
        # print(float( ( rgb565 & 0b1111100000000000 ) >> 11 ) )
        # print(float( ( rgb565 & 0b1111100000000000 ) >> 11 ) / 0b11111 )
        # print((float( ( rgb565 & 0b1111100000000000 ) >> 11 ) / 0b11111) * 255 )
        r = int((float((rgb565 & 0b1111100000000000) >> 11) / 0b11111)*255)
        g = int((float((rgb565 & 0b0000011111100000) >> 5) / 0b111111)*255)
        b = int((float((rgb565 & 0b0000000000011111) ) / 0b11111)*255)

        print("r =", r, "g =", g, "b =", b, "x =", x, "y =", y)

        im.putpixel((x, y), (r, g, b))

        pixelStartIndex += 4
        x += 1
        if x == imageWidth:
            y += 1
            x = 0
            if y == imageHeight:
                break
    
    im.show()

def char2val(character) -> int:
    return ord(character)-32

if __name__ == '__main__':
    main()
