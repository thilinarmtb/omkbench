#!/bin/python3

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

    plt.title("H2D and D2H Time", fontsize=23)
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
    fig = plt.figure(figsize=(9, 9))

    block_size = np.unique(A[:, 0].astype(int))
    for bsize in block_size:
        B = A[A[:, 0].astype(int) == bsize]
        words = B[:, 1].astype(int)
        time = B[:, 2].astype(float)
        plt.plot(words, time, label=f"block-size={bsize}")

    plt.title("D2D Time", fontsize=23)
    plt.xlabel("# of 8-byte Words", fontsize=18)
    plt.ylabel("Time (s)", fontsize=18)
    plt.xscale("log", base=10)
    plt.yscale("log", base=10)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.legend(loc=0)
    plt.savefig("d2d.pdf")
    plt.close()


def plot_variants(A: np.array, title: str) -> None:
    kernels = np.unique(A[:, 0])

    for kernel in kernels:
        fig = plt.figure(figsize=(9, 9))
        B = A[A[:, 0] == kernel]

        block_size = np.unique(B[:, 1].astype(int))
        for bsize in block_size:
            C = B[B[:, 1].astype(int) == bsize]
            words = C[:, 2].astype(int)
            time = C[:, 3].astype(float)
            plt.plot(words, time, label=f"block-size={bsize}")

        plt.title(f"{title} Time: {kernel}", fontsize=23)
        plt.xlabel("# of 8-byte Words", fontsize=18)
        plt.ylabel("Time (s)", fontsize=18)
        plt.xscale("log", base=10)
        plt.yscale("log", base=10)
        plt.xticks(fontsize=16)
        plt.yticks(fontsize=16)
        plt.legend(loc=0)
        plt.savefig(f"{kernel}.pdf")
        plt.close()


def plot_ratio(A: np.array) -> None:
    kernels = np.unique(A[:, 0])
    assert len(kernels) == 2

    fig = plt.figure(figsize=(9, 9))
    B = A[A[:, 0] == kernels[0]]
    C = A[A[:, 0] == kernels[1]]

    block_size = np.unique(A[:, 1].astype(int))
    for bsize in block_size:
        B1 = B[B[:, 1].astype(int) == bsize]
        C1 = C[C[:, 1].astype(int) == bsize]
        words = B1[:, 2].astype(int)
        time_b = B1[:, 3].astype(float)
        time_c = C1[:, 3].astype(float)
        plt.plot(words, np.divide(time_b, time_c), label=f"block-size={bsize}")

    plt.title(f"Ratio: {kernels[0]}/{kernels[1]}", fontsize=23)
    plt.xlabel("# of 8-byte Words", fontsize=18)
    plt.ylabel("Ratio", fontsize=18)
    plt.xscale("log", base=10)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.legend(loc=0)
    plt.savefig(f"ratio.pdf")
    plt.close()


def plot_roofline(
    kernel: str, A: np.array, n_input_vec: int, w_r_ratio: float, D: np.array
) -> None:
    kernels = np.unique(A[:, 0])
    assert len(kernels) == 1

    fig = plt.figure(figsize=(9, 9))

    bsize = 512
    A1 = A[A[:, 1].astype(int) == bsize]
    D1 = D[D[:, 0].astype(int) == bsize]

    words = A1[:, 2].astype(int)
    t_knl = A1[:, 3].astype(float)
    t_d2d = D1[:, 2].astype(float)
    plt.plot(
        words * n_input_vec,
        np.divide(words * n_input_vec * 8 * 1e-9, t_knl / (1 + w_r_ratio)),
        label=f"{kernel}",
    )
    plt.plot(words, np.divide(words * 8 * 1e-9, t_d2d / 2.0), label=f"roofline")

    plt.title(f"Roofiline for: {kernel}, Blocksize = {bsize}", fontsize=23)
    plt.xlabel("# of 8-byte Words", fontsize=18)
    plt.ylabel("GB/sec", fontsize=18)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.legend(loc=0)
    plt.savefig(f"roofline_{kernel}.pdf")
    plt.close()


if __name__ == "__main__":
    A = read("data/frontier/omk_h2d_d2h.txt")
    B = read("data/frontier/omk_d2d.txt")

    C = read("data/frontier/omk_daxpy.txt")
    plot_roofline("add2s1", C[C[:, 0] == "add2s1"], 2, 0.5, B)
    plot_roofline("add2s2", C[C[:, 0] == "add2s2"], 2, 0.5, B)

    D = read("data/frontier/omk_reduction.txt")
    plot_roofline("sum", D[D[:, 0] == "sum"], 1, 0, B)
    plot_roofline("dot", D[D[:, 0] == "dot"], 2, 0, B)
    plot_roofline("glsc3", D[D[:, 0] == "glsc3"], 3, 0, B)

    E = read("data/frontier/omk_scalar_mul_div.txt")
    plot_roofline("div", E[E[:, 0] == "div"], 1, 1, B)
    plot_roofline("mul", E[E[:, 0] == "mul"], 1, 1, B)
