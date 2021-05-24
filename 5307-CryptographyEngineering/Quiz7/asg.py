#!/usr/bin/env python3
import sys

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
    lfsr_master = LFSR(0b1011010000000000, 35457)
    lfsr_1 = LFSR(0b1000000000000100, 55327)
    lfsr_2 = LFSR(0b1000000000100011, 50577)
    
    if len(sys.argv) < 2:
        sys.exit(1)

    bytes_to_gen = int(sys.argv[1])

    for i in range(bytes_to_gen):
        print((lfsr_1.bit ^ lfsr_2.bit), end='')
        if lfsr_master.bit == 0:
            lfsr_1.step()
        else:
            lfsr_2.step()

        if (i + 1) % 16 == 0:
            print()

if __name__ == '__main__':
    main()

