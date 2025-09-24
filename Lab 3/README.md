# Lab 3

## Exercise 1: Logical Inference for Nonograms

Implement an **inference module** for your nonogram interface, a procedure that, given the specification of rows and columns, deduces the value of each pixel without guessing.

The **module** should analyze the specification of each line (row or column) to deduce which cells are **definitely** filled *(black)* (`#`) or empty *(white)* (`.`). The newly deduced information is then **propagated** to the intersecting lines, and this process is repeated until all cells have been determined. For the given test cases, you may assume that **pure inference, without any trial and error, is sufficient to determine the value of every pixel**.

### Input E1

Read from `zad1_input.txt` in **the same format as Lab 2's E1**:

The first line contains two integers **X** and **Y** where **X** is the number of rows and **Y** is the number of columns, the next **X** lines contain descriptions of rows, which consist of at least **one non-zero** number that smaller than the amount of lines of the opposite axis. They are followed by **Y** descriptions of columns which follow the same rules.

### Output E1

Write to `zad1_output.txt`

Exactly **X** lines, each with **Y** characters so that the image fulfills the nonogram:

- `.` for an empty *(white)* cell  
- `#` for a filled *(black)* cell

### Solution E1

The solution consists of one file:  
[src/zad1.cpp](src/zad1.cpp).

<details>
  <summary>Notes about solution</summary>

  For the same reasons as in [Lab 2](../Lab%202/README.md#solution-e4), this exercise is a standalone program.
</details>

## Exercise 2: Nonogram Solving with Backtracking

In this task, you will continue solving nonograms, but this time the solution must employ **backtracking**. The test cases are chosen so that the pure **inference** approach from Exercise 1 will most likely be unable to determine all pixel values.

Your solution should interweave **inference** with **backtracking** to efficiently complete the puzzle.

### Input E2

Read from `zad2_input.txt` in **the same format as the previous exercise**:

The first line contains two integers **X** and **Y** where **X** is the number of rows and **Y** is the number of columns, the next **X** lines contain descriptions of rows, which consist of at least **one non-zero** number that smaller than the amount of lines of the opposite axis. They are followed by **Y** descriptions of columns which follow the same rules.

### Output E2

Write to `zad2_output.txt`

Exactly **X** lines, each with **Y** characters so that the image fulfills the nonogram:

- `.` for an empty *(white)* cell  
- `#` for a filled *(black)* cell

### Solution E2

The solution consists of one file:  
[src/zad2.cpp](src/zad2.cpp).

<details>
  <summary>Notes about solution</summary>

  For the same reasons as in [Lab 2](../Lab%202/README.md#solution-e4), this exercise is a standalone program.
</details>

---

## Makefile usage

Run `make X` to build solution for the `X`-th exercise, where `X` is 1 or 2.

Run `make clean` to remove all executables.

---

> **Source:**
> This list of problems was assigned as part of the *Artificial Intelligence* (SI) course in the 2025 Summer semester at the University of Wrocław by Paweł Rychlikowski
