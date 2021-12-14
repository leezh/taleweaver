import sys

with open(sys.argv[1], 'rb') as infile:
    with open(sys.argv[2], 'wb') as outfile:
        for data in infile.read():
            outfile.write(hex(data).encode('ASCII'))
            outfile.write(b',')
