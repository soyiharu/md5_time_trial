#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import time
import hashlib
import sys
from multiprocessing import Pool
from itertools import repeat

def calc_hash(arg):
    hash, salt, i = arg
    tmp = hashlib.md5("{0}${1:06d}".format(salt, i).encode("utf-8")).hexdigest()
    return hash == tmp
    
def main():
    argv = sys.argv[1:]
    if len(argv) != 2:
        sys.exit(0)

    salt = argv[0]
    hash = argv[1]
    
    start = time.time()
    
    pool = Pool(4)
    result = pool.map(calc_hash, zip(repeat(hash), repeat(salt), range(1000000)))
    index = result.index(True)
    print("match[{0:06d}]".format(index))
    
    end = time.time()
    print("elapsed time:{0}s".format(end - start))
    
if __name__ == "__main__":
    main()