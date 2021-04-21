import numpy

def LFSR(tap, seed):
    return

if __name__ == '__main__':
    tap  = int(input("tap= "))
    seed  = str(input("seed= "))
    print('')

    lfsr = LFSR(tap, seed)
    for i in range(10):
        lfsr.step()
        print(lfsr.val, lfsr.bit)
