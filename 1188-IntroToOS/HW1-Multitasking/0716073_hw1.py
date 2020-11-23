#!/usr/bin/env python3

# Standard library modules
import asyncio
import hashlib
import itertools
import multiprocessing
import sys
# External library
import bs4
import requests

class PowChars:
    def __iter__(self):
        self.n = 0x21
        return self

    def __next__(self):
        if self.n <= 0x7e:
            c = chr(self.n)
            self.n += 1
            return c
        else:
            raise StopIteration

def task_pow():
    S = input()[:5]
    chrset = iter(PowChars())

    for p in itertools.product(chrset, repeat=5):
        C = ''.join(p)
        CS = C + S
        m = hashlib.sha256()
        m.update(CS.encode('utf-8'))
        h = m.hexdigest()
        if h[:5] == '00000':
            return CS
        
    return None


def task_web_title():
    url = input()
    headers = {
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64; rv:82.0) Gecko/20100101 Firefox/82.0',
    }

    r = requests.get(url, timeout = 30, headers=headers)
    html = bs4.BeautifulSoup(r.text)
    print(html.title.text)


def main():
    task_id = int(input())
    w = [int(x) for x in input().split()]
    worker_type = w[0]

    if worker_type == 1 or worker_type == 2:
        worker_count = w[1]
    elif worker_type == 3:
        pass
    else:
        sys.exit('Invalid worker type!')

    task_count = int(input())

    if task_id == 1:
        pass
    elif task_id == 2:
        pass
    else:
        sys.exit('Unknown task!')


if __name__ == '__main__':
    main()

