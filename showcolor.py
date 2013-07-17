#!/usr/bin/python

import sys
from pgmagick import Image, Geometry, Color

if __name__ == "__main__":
    print sys.argv[1]
    im = Image(Geometry(200,200), Color(sys.argv[1]))
    im.display()

