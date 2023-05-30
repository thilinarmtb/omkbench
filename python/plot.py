#!/bin/bash

import matplotlib
import matplotlib.pyplot as plt
import numpy as np


def read(fname: str) -> np.array:
    A = []
    with open(fname) as f:
        for line in f.readlines():
            A.append(line.split(","))
    return np.array(A)


def plot_h2d_d2h(A: np.array) -> None:
    fig = plt.figure(figsize=(9, 9))

    words = A[:, 0].astype(int)
    h2d = A[:, 1].astype(float)
    d2h = A[:, 2].astype(float)
    plt.plot(words, d2h, label="D2H")
    plt.plot(words, h2d, label="H2D")

    plt.title("H2D and D2H Time (s)", fontsize=23)
    plt.xlabel("# of 8-byte Words", fontsize=18)
    plt.ylabel("Time (s)", fontsize=18)
    plt.xscale("log", base=10)
    plt.yscale("log", base=10)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.legend(loc=0)
    plt.savefig("h2d_d2h.pdf")
    plt.close()


def plot_d2d(A: np.array) -> None:
    block_size = np.unique(A[:, 1].astype(int))

    fig = plt.figure(figsize=(9, 9))

    for bsize in block_size:
        B = A[A[:, 1].astype(int) == bsize]
        words = B[:, 0].astype(int)
        d2d = B[:, 2].astype(float)
        plt.plot(words, d2d, label=f"block-size={bsize}")

    plt.title("D2D Time (s)", fontsize=23)
    plt.xlabel("# of 8-byte Words", fontsize=18)
    plt.ylabel("Time (s)", fontsize=18)
    plt.xscale("log", base=10)
    plt.yscale("log", base=10)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.legend(loc=0)
    plt.savefig("d2d.pdf")
    plt.close()


if __name__ == "__main__":
    A = read("data/frontier/omk_h2d_d2h.txt")
    plot_h2d_d2h(A)

    B = read("data/frontier/omk_d2d.txt")
    plot_d2d(B)

    C = read("data/frontier/omk_daxpy.txt")
    D = read("data/frontier/omk_reduction.txt")
