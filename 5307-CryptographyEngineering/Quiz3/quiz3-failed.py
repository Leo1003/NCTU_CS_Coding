#!/usr/bin/env python3
import math
import itertools
from string import ascii_uppercase, ascii_lowercase, ascii_letters

tri_gram_map = {}

def init_trigram():
    for c0 in ascii_uppercase:
        tri_gram_map[c0] = {}
        for c1 in ascii_uppercase:
            tri_gram_map[c0][c1] = {}
            for c2 in ascii_uppercase:
                tri_gram_map[c0][c1][c2] = 0
            tri_gram_map[c0][c1]['_'] = 0

def training():
    with open('traindata.txt', 'r') as f:
        chr_idx = 0
        chr_list = ['_', '_', '_']

        for linebuf in f.readlines():
            for c in linebuf:
                if c in ascii_uppercase:
                    chr_list[chr_idx % 3] = c
                elif c in ascii_lowercase:
                    chr_list[chr_idx % 3] = c.upper()
                else:
                    continue

                if chr_idx >= 2:
                    c0 = chr_list[(chr_idx - 2) % 3]
                    c1 = chr_list[(chr_idx - 1) % 3]
                    c2 = chr_list[(chr_idx) % 3]
                    #print("{}{}{}".format(c0, c1, c2))
                    tri_gram_map[c0][c1][c2] += 1
                    tri_gram_map[c0][c1]['_'] += 1

                chr_idx += 1

def W(c0, c1, c2):
    tri_A = tri_gram_map[c0][c1][c2]
    tri_AB = tri_gram_map[c0][c1]['_']
    if tri_A == 0 or tri_AB == 0:
        return 0
    return math.log(26 * (tri_A / tri_AB))

def vowel_analyze(s, collen):
    total_diff = 0.0
    for i in range(collen):
        vowel_count = 0
        total_letter = 0

        for j in range(i, len(s), collen):
            if s[j].upper() in "AEIOU":
                vowel_count += 1
            total_letter += 1

        diff = math.fabs(vowel_count - (total_letter * 0.4))
        total_diff += diff

    return total_diff

def transpose(s, collen):
    matrix = []
    for i in range(0, len(s), collen):
        matrix.append(s[i:i+5])
    return matrix

def markov_chain_hint(matrix, hint):
    hint_seq = [[] for i in range(len(hint))]

    for hint_idx in range(len(hint)):
        for r in range(len(matrix)):
            if matrix[r][0] == hint[hint_idx]:
                #print("{} matches {}".format(matrix[r], hint[hint_idx]))
                hint_seq[hint_idx].append(r)

    #print("hint_seq: {}".format(hint_seq))
    for seq in itertools.product(*hint_seq):
        init_seq = []
        other_seq = matrix.copy()

        for row in seq:
            init_seq.append(other_seq[row])

        other_seq = [row for row in other_seq if row not in init_seq]
        final_matrix = markov_chain(init_seq, other_seq)
        
        print_final_matrix(final_matrix, 5)

def markov_chain(init_seq, seq):
    print("markov_chain(): {} ; {}".format(init_seq, seq))
    if len(seq) == 0:
        return init_seq

    highest_score = float('-inf')
    highest_seq = None
    for i in range(len(seq)):
        score = W(init_seq[-1][0], init_seq[-2][0], seq[i][0])
        if score > highest_score:
            highest_score = score
            highest_seq = i

    next_init_seq = init_seq.copy()
    next_init_seq.append(seq[highest_seq])
    next_seq = seq.copy()
    next_seq.pop(highest_seq)

    return markov_chain(next_init_seq, next_seq)

def print_final_matrix(matrix, length):
    for i in range(length):
        for m in matrix:
            if i < len(m):
                print(m[i], end='')

def main():
    init_trigram()
    training()

    ciphtext = ''
    with open('testdata.txt', 'r') as f:
        data = f.read()
        ciphtext = ''.join(c for c in data if any(a in c for a in ascii_letters)).upper()
        ##print("Cipher text: {}".format(ciphtext))

    """
    for k in range(3, int(len(ciphtext) * 0.35)):
        d = vowel_analyze(ciphtext, k)
        print("Key length analyze: length {} is {}".format(k, d))
    """
    matrix = transpose(ciphtext, 5)
    final_matrix = markov_chain_hint(matrix, 'GRE')
    #print_final_matrix(final_matrix, 5)

if __name__ == '__main__':
    main()
