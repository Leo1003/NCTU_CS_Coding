#!/usr/bin/env python3
import math
import random
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

def transpose(s, collen):
    matrix = []
    for i in range(0, len(s), collen):
        matrix.append(s[i:i + collen])
    return matrix

def markov_chain_hint(matrix, hint, iter_cnt=1000, score_limit=None):
    hint_seq = [[] for i in range(len(hint))]

    for hint_idx in range(len(hint)):
        for r in range(len(matrix)):
            if matrix[r][0] == hint[hint_idx]:
                #print("{} matches {}".format(matrix[r], hint[hint_idx]))
                hint_seq[hint_idx].append(r)

    #print("hint_seq: {}".format(hint_seq))
    highest_score = score_limit
    final_matrix = None
    for seq in itertools.product(*hint_seq):
        init_seq = []
        other_seq = matrix.copy()

        for row in seq:
            init_seq.append(other_seq[row])

        other_seq = [row for row in other_seq if row not in init_seq]
        ans_matrix, score = markov_chain(init_seq, other_seq, iter_cnt)
        
        print("Score: {}".format(score))
        print(matrix_string(ans_matrix))

        if highest_score == None or score > highest_score:
            highest_score = score
            final_matrix = ans_matrix

    return final_matrix, highest_score

def markov_chain(init_seq, seq, iter_cnt):
    #print("markov_chain(): {} ; {}".format(init_seq, seq))
    if len(seq) == 0:
        return init_seq

    current_score = calc_score(matrix_string(init_seq + seq))

    for iter_idx in range(iter_cnt):
        # Swap Randomly
        new_seq = seq.copy()
        idx = range(len(seq))
        i1, i2 = random.sample(idx, 2)
        new_seq[i1], new_seq[i2] = seq[i2], seq[i1]
 
        new_score = calc_score(matrix_string(init_seq + new_seq))
        flip = random.random()

        #if new_score > current_score or flip < (new_score / current_score):
        if new_score > current_score:
            seq = new_seq
            current_score = new_score

    #print("Final score: {}".format(current_score))
    return init_seq + seq, current_score

def calc_score(s: str) -> float:
    chr_idx = 0
    chr_list = ['_', '_', '_']
    total_score = 0

    for c in s:
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
            total_score += W(c0, c1, c2)

        chr_idx += 1
    return total_score

def matrix_string(matrix):
    length = 0
    s = ''
    for m in matrix:
        length = max(length, len(m))

    for i in range(length):
        for m in matrix:
            if i < len(m):
                s += m[i]

    return s

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
    matrix = transpose(ciphtext, 11)
    #matrix = transpose(ciphtext, 9)
    final_matrix = None
    while final_matrix == None:
        final_matrix, score = markov_chain_hint(matrix, 'GRE', 1000, 100)
        #final_matrix = markov_chain_hint(matrix, 'LAS', 1000, 40)

    print(matrix_string(final_matrix))

if __name__ == '__main__':
    main()
