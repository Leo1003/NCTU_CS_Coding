#!/usr/bin/env python3

# Standard library modules
import asyncio
import hashlib
import itertools
import multiprocessing
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
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

def task_pow(S):
    #S = input()[:5]
    chrset = iter(PowChars())

    for p in itertools.product(chrset, repeat=5):
        C = ''.join(p)
        CS = C + S
        m = hashlib.sha256()
        m.update(CS.encode('utf-8'))
        h = m.hexdigest()
        if h[:5] == '00000':
            print(CS)
            return CS
        
    return None


def task_web_title(url):
    #url = input()
    headers = {
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64; rv:82.0) Gecko/20100101 Firefox/82.0',
    }

    r = requests.get(url, timeout = 30, headers=headers)
    html = bs4.BeautifulSoup(r.text)
    print(html.title.text)

async def task_web_title_async(loop, url):
    headers = {
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64; rv:82.0) Gecko/20100101 Firefox/82.0',
    }

    r = await loop.run_in_executor(None, requests.get, url, timeout = 30, headers=headers)
    html = bs4.BeautifulSoup(r.text)
    print(html.title.text)

def run_pool(pool, task_id, task_count):
    for i in range(task_count):
        inp = input()
        if task_id == 1:
            pool.submit(task_pow, inp[:5])
        elif task_id == 2:
            pool.submit(task_web_title, inp)
        else:
            sys.exit('Unknown task!')
    pool.shutdown(wait=True)

def run_coroutine(task_id, task_count):
    evloop = asyncio.get_event_loop()
    tasks = []

    for i in range(task_count):
        inp = input()
        if task_id == 1:
            tasks.append(evloop.create_task(task_web_title_async(evloop, inp)))
        elif task_id == 2:
            tasks.append(evloop.run_in_executor(task_pow(inp[:5])))
        else:
            sys.exit('Unknown task!')

    evloop.run_until_complete(asyncio.wait(tasks))

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

    if worker_type == 1:
        with ThreadPoolExecutor(worker_count) as pool:
            run_pool(pool, task_id, task_count)
    elif worker_type == 2:
        with ProcessPoolExecutor(worker_count) as pool:
            run_pool(pool, task_id, task_count)
    else:
        run_coroutine(task_id, task_count)


if __name__ == '__main__':
    main()

