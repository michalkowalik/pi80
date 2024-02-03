#!/usr/bin/env python

import sys
from textwrap import wrap

class Hex2bin:

    def __init__(self):
        pass

    def convert(self, source_file, target_file):
        print(f"converting {source_file} to {target_file}")
        with open(source_file, 'r') as source:
            with open(target_file, 'wb') as target:
                for line in source:
                    data = self.convert_line(line)
                    if data is not None and data != len(data) > 0:
                        target.write(bytes(data))


    def convert_line(self, line):
        """
        :param line:  intel hex endcoded data
        :return:      binary format of the hex record
        """
        print("converting hex data : {line}")
        if line[0] != ':':
            print("Error: line should start with \":\"")
            sys.exit(1)
        byte_count = int(line[1:3], 16)
        # bytes 3-7 are address, which we ignore here
        record_type = line[7:9]
        data = line[9: 9 + byte_count*2]
        print(f"record type: {record_type}, data length: {byte_count}, data: {data}")

        match record_type:
            case '00':
                return self.convert_data(data)
            case '01':
                print("EOF")
                return
            case _:
                print(f"ignoring record of type {record_type}")
                return

    def convert_data(self, data):
        data_bytes = wrap(data, 2)
        return bytearray([int(x, 16) for x in data_bytes])

def main():
    args = sys.argv[:]
    hex2bin = Hex2bin()
    hex2bin.convert(args[1], args[2])

if __name__ == "__main__":
    main()