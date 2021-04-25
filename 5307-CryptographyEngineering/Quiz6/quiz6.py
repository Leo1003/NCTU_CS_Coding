#!/usr/bin/env python3
import os

def berlekamp_massey(S):
    B = [1]
    C = [1]
    l = 0
    n = len(S)
    m = -1

    for i in range(n):
        if os.getenv('QUIZ6_DEBUG') == "x":
            print("i = {}".format(i))
            print("l = {}".format(l))
            print("C = {}".format(C))
        # Calculate discrepancy
        d = S[i]
        for j in range(1, l + 1):
            d ^= C[j] & S[i - j]

        if os.getenv('QUIZ6_DEBUG') == "x":
            print("d = {}".format(d))
            print()

        if d != 0:
            T = C.copy()

            
            P = []
            for j in range(i - m):
                P.append(0)
            for j in range(len(B)):
                P.append(B[j])

            if os.getenv('QUIZ6_DEBUG') == "x":
                print("P = {}".format(P))

            if len(P) > len(C):
                C.extend([0] * (len(P) - len(C)))
            if len(C) > len(P):
                P.extend([0] * (len(C) - len(P)))
            for j in range(len(C)):
                C[j] = (C[j] + P[j]) % 2

            if os.getenv('QUIZ6_DEBUG') == "x":
                print("_bm_shift() = {}".format(C))
                print()

            if (2 * l) <= i:
                l = i + 1 - l
                B = T
                m = i

    if os.getenv('QUIZ6_DEBUG'):
        print("C = {}".format(C))
    return l, C

def main():
    x = []
    try:
        while True:
            inps = list(map(int, input().split()))
            x.extend(inps)
    except EOFError:
        pass
    
    l, C = berlekamp_massey(x)
    print("length = {}".format(l))
    print("ploynomial = ", end='')
    for i in range(l, 0, -1):
        if C[l - i] != 0:
            print("x^{} + ".format(i), end='')
    print("1")
    print("seed = {}".format(x[0:l]))

if __name__ == '__main__':
    main()

