#!/usr/bin/env python3
import os

def next_power2(n: int):
    count = 0;
    if n <= 0:
        return 0
    while n != 0:
        n >>= 1
        count += 1
    return count;

def get_bit_count(value: int):
    n = 0
    while value != 0:
        if value & 1:
            n += 1
        value >>= 1
    return n

class LFSR:
    def __init__(self, poly, seed):
        self.len = next_power2(poly)
        self.mask = (1 << self.len) - 1
        self.poly = (poly << 1) | 1
        if isinstance(seed, str):
            self.reg = int(seed, 2)
        elif isinstance(seed, int):
            self.reg = seed
        else:
            raise TypeError('seed is not int or str')
        self.reg &= self.mask

    def step(self):
        self.reg <<= 1
        if self.reg & (1 << self.len):
            self.reg = (self.reg ^ self.poly)

    def get_bit(self, bit):
        mask = (1 << bit)
        return (self.reg & mask) >> bit

    def gen_byte(self):
        b = 0
        for i in range(8):
            b <<= 1
            b |= self.bit
            self.step()
        return b

    @property
    def val(self):
        return self.reg

    @property
    def bit(self):
        return self.get_bit(self.len - 1)

def main():
    data = bytearray()
    try:
        while True:
            inp = input()
            for binstr in inp.split():
                data.append(int(binstr, 2))
    except EOFError:
        pass

    if os.getenv('QUIZ5_DEBUG'):
        print("input data = {}".format(data))

    for i in range(256):
        lfsr = LFSR(0b10000001, i)

        decrypted = bytearray()
        completed = True
        for c in data:
            d = c ^ lfsr.gen_byte()
            if d & 0b10000000:
                completed = False
                if os.getenv('QUIZ5_DEBUG') == "x":
                    print("failed decrypt data = {}".format(decrypted))
                break
            decrypted.append(d)

        if completed:
            if os.getenv('QUIZ5_DEBUG'):
                print("seed = {:08b}".format(i))
                print("decrypted = {}".format(decrypted))
                print()
            else:
                print(decrypted.decode("utf-8"))

if __name__ == '__main__':
    main()

