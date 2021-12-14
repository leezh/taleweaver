import sys
import json

separator = ''
with open(sys.argv[1], 'r') as infile:
    with open(sys.argv[2], 'wb') as outfile:
        data = infile.read().encode('ASCII').decode('ASCII')
        outfile.write(json.dumps(data).encode('ASCII'))
        outfile.write(b',')
