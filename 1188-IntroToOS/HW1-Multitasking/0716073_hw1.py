#!/usr/bin/env python3

# Standard library modules
import asyncio
import hashlib
import itertools
import multiprocessing
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import sys
import time
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
    headers = {
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64; rv:82.0) Gecko/20100101 Firefox/82.0',
    }

    r = requests.get(url, headers=headers)
    html = bs4.BeautifulSoup(r.text, features="lxml")
    print(html.title.text)

async def task_web_title_async(loop, url):
    headers = {
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64; rv:82.0) Gecko/20100101 Firefox/82.0',
    }

    r = await loop.run_in_executor(None, requests.get, url, {"headers": headers})
    html = bs4.BeautifulSoup(r.text, features="lxml")
    print(html.title.text)

def run_pool(pool, task_id, task_inps):
    if task_id == 1:
        pool.map(task_pow, map(lambda s: s[:5], task_inps))
    elif task_id == 2:
        pool.map(task_web_title, task_inps)
    else:
        sys.exit('Unknown task!')
    pool.shutdown(wait=True)

def run_coroutine(task_id, task_inps):
    evloop = asyncio.get_event_loop()
    tasks = []

    for t in task_inps:
        if task_id == 1:
            tasks.append(evloop.run_in_executor(None, task_pow, t[:5]))
        elif task_id == 2:
            tasks.append(evloop.create_task(task_web_title_async(evloop, t)))
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

    inputs = []
    task_count = int(input())
    for i in range(task_count):
        inputs.append(input())

    start = time.perf_counter()

    if worker_type == 1:
        with ThreadPoolExecutor(worker_count) as pool:
            run_pool(pool, task_id, inputs)
    elif worker_type == 2:
        with ProcessPoolExecutor(worker_count) as pool:
            run_pool(pool, task_id, inputs)
    else:
        run_coroutine(task_id, inputs)

    end = time.perf_counter()
    print("Task completed in {:.06f} second".format(end - start))


if __name__ == '__main__':
    main()

