#!/usr/bin/python

from pgmagick import Image

im = Image('/windata/Downloads/IMG_0550.PNG')

for j in range(1136):
    for i in range(640):
        print i, ",", j, im.pixelColor(i,j).to_std_string()
    print "\n"


