#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import time
import hashlib
import sys

def main():
    argv = sys.argv[1:]
    if len(argv) != 2:
        sys.exit(0)

    salt = argv[0]
    hash = argv[1]
    
    start = time.time()
    for i in range(1000000):
        pw = "{0}${1:06d}".format(salt, i).encode("utf-8")
        tmp = hashlib.md5(pw).hexdigest()
        if hash == tmp:
            print("match[{0:06d}]".format(i))
    end = time.time()
    print("elapsed time:{0}s".format(end - start))
    
if __name__ == "__main__":
    main()