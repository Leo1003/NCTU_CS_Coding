#!/usr/bin/env python3

LFSR_MASK = 0b1111

class LFSR:
    def __init__(self, seed):
        self.reg = int(seed, 2) & LFSR_MASK

    def step(self):
        next_bit = (self.get_bit(1) ^ self.get_bit(0)) << 3
        self.reg = ((self.reg >> 1) & LFSR_MASK) | (next_bit & 0b1000)

    def get_bit(self, bit):
        mask = (1 << bit)
        return (self.reg & mask) >> bit

    @property
    def val(self):
        return self.reg

    @property
    def bit(self):
        return self.get_bit(0)

if __name__ == '__main__':
    seed = str(input("seed= "))
    print('')

    lfsr = LFSR(seed)
    for i in range(32):
        lfsr.step()
        print("{:04b} {}".format(lfsr.val, lfsr.bit))

