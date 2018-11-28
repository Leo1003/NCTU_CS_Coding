#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *name[10] = {
    "Royal flush",
    "Straight flush",
    "Four of a kind",
    "Full house",
    "Flush",
    "Straight",
    "Three of a kind",
    "Two pair",
    "Pair",
    "High card"
};

int getsuit(int id)
{
    return (id - 1) / 13;
}

/*
 * 0: K
 * 1: A
 * 2: 2
 * ...
 * 11: J
 * 12: Q
 */
int getnum(int id)
{
    return id % 13;
}

void put_slot(int card[5], int slot[13])
{
    memset(slot, 0, sizeof(int) * 13);
    for (int i = 0; i < 5; i++) {
        slot[getnum(card[i])]++;
    }
}

/*
 * Return the start number of a straight, otherwise return -1
 */
int isstraight(int card[5])
{
    int slot[13];
    put_slot(card, slot);
    for (int i = 0; i < 13; i++) {
        if (slot[i]) {
            int pass = 1;
            for (int j = i; j < i + 5; j++) {
                if (!slot[j % 13]) {
                    pass = 0;
                    break;
                }
            }
            if (pass) {
                return i;
            }
        }
    }
    return -1;
}

int isflush(int card[5])
{
    int suit = getsuit(card[0]);
    for (int i = 1; i < 5; i++) {
        if (suit != getsuit(card[i])) {
            return 0;
        }
    }
    return 1;
}

int pair(int card[5])
{
    int slot[13], ans = 0;
    put_slot(card, slot);
    for (int i = 0; i < 13; i++) {
        ans += slot[i] / 2;
    }
    return ans;
}

int isroyal(int card[5])
{
    return (isflush(card) && isstraight(card) == 10);
}

int isstrflush(int card[5])
{
    return (isflush(card) && isstraight(card) != -1);
}

int isfour(int card[5])
{
    int slot[13];
    put_slot(card, slot);
    for (int i = 0; i < 13; i++) {
        if (slot[i] == 4) {
            return 1;
        }
    }
    return 0;
}

int isfull(int card[5])
{
    int slot[13], three = 0, two = 0;
    put_slot(card, slot);
    for (int i = 0; i < 13; i++) {
        if (slot[i] == 3) {
            three = 1;
        } else if (slot[i] == 2) {
            two = 1;
        }
    }
    return (three && two);
}

int isthree(int card[5])
{
    int slot[13];
    put_slot(card, slot);
    for (int i = 0; i < 13; i++) {
        if (slot[i] == 3) {
            return 1;
        }
    }
    return 0;
}

int determine(int card[5])
{
    if (isroyal(card)) {
        return 0;
    } else if (isstrflush(card)) {
        return 1;
    } else if (isfour(card)) {
        return 2;
    } else if (isfull(card)) { 
        return 3;
    } else if (isflush(card)) {
        return 4;
    } else if (isstraight(card) != -1) {
        return 5;
    } else if (isthree(card)) {
        return 6;
    } else if (pair(card) == 2) {
        return 7;
    } else if (pair(card) == 1) {
        return 8;
    } else {
        return 9;
    }
}

int main()
{
    int A[5], B[5];
    for (int i = 0; i < 5; i++) {
        scanf("%d", &A[i]);
    }
    for (int i = 0; i < 5; i++) {
        scanf("%d", &B[i]);
    }
    int scoreA = determine(A);
    printf("%s\n", name[scoreA]);
    int scoreB = determine(B);
    printf("%s\n", name[scoreB]);
    if (scoreA < scoreB) {
        printf("Player A");
    } else if (scoreA > scoreB) {
        printf("Player B");
    } else {
        printf("Tie");
    }
    return 0;
}
