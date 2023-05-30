#!/bin/bash

import numpy as np
import matplotlib
import matplotlib.pyplot as plt

def read(fname: str) -> np.array:
    A = []
    with open(fname) as f:
        for line in f.readlines():
            A.append(line.split())
    return np.array(A)

def plot_h2d_d2h(A : np.array) -> None:
    words = A[:, 0].astype(int)
    h2d = A[:, 1].astype(float)
    d2h = A[:, 2].astype(float)

    fig = plt.figure((9, 9))
    plt.title("H2D and D2H Time (s)")
    plt.xlabel("# of Words")
    plt.ylabel("Time (s)")
    plt.plot(words, d2h, label="D2H")
    plt.plot(words, d2h, label="H2D")
    plt.savefig("h2d_d2h.pdf")

if __name__ == "__main__":
    A = read("data/frontier/omk_h2d_d2h.txt")
    B = read("data/frontier/omk_d2d.txt")
    C = read("data/frontier/omk_daxpy.txt")
    D = read("data/frontier/omk_reduction.txt")
