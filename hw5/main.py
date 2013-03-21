#!/usr/bin/python

# Imports for the module.
from struct import pack, unpack
from sysv_ipc import Semaphore, SharedMemory
from threading import Thread

# Constants for the module.
MEMFRMT = "iii"
MEMSIZE = calcsize(MEMFRMT)
KEYS = {"M" : 42,
        "O" : 43,
        "H" : 44,
        "B" : 45,
        "S" : 46
        }
THREADS = [
        Thread(target=Oxygen),
        Thread(target=Oxygen),
        Thread(target=Hydrogen),
        Thread(target=Hydrogen),
        Thread(target=Hydrogen),
        Thread(target=Hydrogen),
        Thread(target=Oxygen),
        Thread(target=Hydrogen),
        Thread(target=Hydrogen),
        Thread(target=Hydrogen),
        Thread(target=Oxygen),
        Thread(target=Oxygen),
        Thread(target=Hydrogen),
        Thread(target=Hydrogen),
        Thread(target=Hydrogen)
        ]

# All non-binary; B is for barrier synchronzation.
M = Semaphore(KEYS["M"], IPC_CREX, 0600, 1)
O = Semaphore(KEYS["O"], IPC_CREX, 0600, 0)
H = Semaphore(KEYS["H"], IPC_CREX, 0600, 0)
B = Semaphore(KEYS["B"], IPC_CREX, 0600, 0)

# Shread memory for the threads. Saved in ints as hcount, ocount, bcount
shm = SharedMemory(KEYS["S"], IPC_CREX, 0600, MEMSIZE)
shm.update_write(pack(MEMFRMT, 0, 0, 0))

def main():
    for thread in THREADS:
        thread.start()
    for thread in THREADS:
        thread.join()

def Hydrogen():
    hcount, ocount, bcount = acquire_and_update()
    if (ocount >= 1 and hcount >= 1):
        bcount = 2
        H.release()
        O.release()
        update_write(hcount, ocount, bcount)
        exit()
    else:
        hcount += 1
        update_and_release(hcount, ocount, bcount)
        H.acquire()
        B.acquire()
        hcount, ocount, bcount = update_read()
        hcount -= 1
        bcount -= 1
        if (bcount != 0):
            B.release()
            update_write(hcount, ocount, bcount)
            exit()
        else:
            B.release()
            update_and_release(hcount, ocount, bcount)
    print "Made it, new H2O molecule"

def Oxygen():
    hcount, ocount, bcount = acquire_and_update()
    if (hcount >= 2):
        bcount = 2
        H.release(2)
        update_write(hcount, ocount, bcount)
        exit()
    else:
        ocount += 1
        update_and_release(hcount, ocount, bcount)
        O.acquire()
        B.acquire()
        hcount, ocount, bcount = update_read()
        ocount -= 1
        bcount -= 1
        if (bcount != 0):
            B.release()
            update_write(hcount, ocount, bcount)
            exit()
        else:
            B.release()
            update_and_release(hcount, ocount, bcount)
    print "Made it, new H2O molecule"

def update_and_release(hcount, ocount, bcount):
    update_write(hcount, ocount, bcount)
    M.release()

def acquire_and_update():
    M.acquire()
    return update_read()

def update_read():
    return unpack(MEMFRMT, shm.update_read(MEMSIZE))

def update_write(hcount, ocount, bcount):
    shm.update_write(pack(MEMFRMT, hcount, ocount, bcount))

