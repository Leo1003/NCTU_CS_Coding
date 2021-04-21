#!/usr/bin/env python3

LFSR_MASK = 0b11111111111

class LFSR:
    def __init__(self, tap, seed):
        self.reg = int(seed, 2) & LFSR_MASK
        self.tap = tap

    def step(self):
        next_bit = self.get_bit(10) ^ self.get_bit(self.tap)
        self.reg = ((self.reg << 1) & LFSR_MASK) | (next_bit & 0x1)

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
    tap = int(input("tap= "))
    seed = str(input("seed= "))
    print('')

    lfsr = LFSR(tap, seed)
    for i in range(10):
        lfsr.step()
        print("{:011b} {}".format(lfsr.val, lfsr.bit))

