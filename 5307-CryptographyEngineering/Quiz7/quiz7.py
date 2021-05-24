#!/usr/bin/env python3
import random
import sys

def main():
    sysrand = random.SystemRandom()

    if len(sys.argv) < 2:
        sys.exit(1)

    bytes_to_gen = int(sys.argv[1])

    for i in range(0, bytes_to_gen, 2):
        r = sysrand.randbytes(2)
        print("{:08b}{:08b}".format(r[0], r[1]))

if __name__ == '__main__':
    main()

