# Lab 2

## Exercise 1: Full Nonogram Solver

Write a program that reads a **full** nonogram description *(up to 15×15)* and outputs a valid filled grid. The program should use the method and I/O from Lab 1 Exercise 5.

### Input E1

Read from `zad1_input.txt`

The first line contains two integers **X** and **Y** where **X** is the number of rows and **Y** is the number of columns, the next **X** lines contain descriptions of rows, which consist of at least **one non-zero** number that smaller than the amount of lines of the opposite axis. They are followed by **Y** descriptions of columns which follow the same rules.

### Output E1

Write to `zad1_output.txt`

Exactly **X** lines, each with **Y** characters so that the image fulfills the nonogram:

- `.` for an empty *(white)* cell  
- `#` for a filled *(black)* cell

> **Note:** There may be multiple valid solutions.

### Solution E1

The solution consists of one file:  
[src/zad1.cpp](src/zad1.cpp).

<details>
  <summary>Notes about solution</summary>

  The solution in the Lab 2 folder worked for most of the examples in the time limit provided by the professor. In the Lab 3 folder there exist a [much better solution](../Lab%203/src/zad2.cpp) that solves these examples in the time limit, but loses a bit of score due to method used being the one from [Lab 3 Exercise 2](../Lab%203/README.md#exercise-2-nonogram-solving-with-backtracking).

</details>

## Exercise 2: Commando in a Maze

You control a commando dropped at night into a **rectangular** maze of some size. Each cell of the maze is one of:

1. `#` - a wall (impassable)
2. `G` - a goal square
3. `S` - a possible starting square; the commando may begin on any one of these, but you don’t know which
4. `B` - both a start and a goal (counts as either role)  
5. ` ` - an ordinary empty square

On each turn you issue a move: **U**, **D**, **L**, or **R**.  Moving into a wall does nothing (the commando stays put).  You must produce a single **sequence of at most 150 moves** that **guarantees** your commando ends on some goal cell, **no matter** which start square he actually began on.

Your solver should consist of two phases:

1. **Uncertainty reduction** via random/greedy moves that narrow down the possible start locations.  
2. **BFS search** (no A*) from the remaining candidate positions to a goal.

Your task is therefore, among other things, to check the level of uncertainty acceptable by BFS and propose an action plan for the first part.

The program should have a setting to show the board state after every move.

### Input E2

Read from `zad2_input.txt`

The file contains a series of lines (all the same length), each consisting solely of the characters `#`, `G`, `S`, `B`, or  ` `.

### Output E2

Write to `zad2_output.txt`

Output a single line with your move sequence consisting only of the characters `U`, `D`, `L` or `R`. There should be no breaks in between the characters.

> **Note:** There may be multiple valid solutions, because of that the tests can't be simply checked with a pre-made out2_X.txt, therefore the one can check the validity of the solution with a `--verbose` flag

### Solution E2

The solution consists of one file:  
[src/zad2.cpp](src/zad2.cpp).

<details>
  <summary>Notes about solution</summary>
    The proffesor stated that the initial uncertainty reduction could be performed by using trial and error to get a <i> good enough </i> sequence, hence my solution uses that fact.
</details>

## Exercise 3: Optimal Commando Path with A* Search

Solve the same maze‐escape task as in **Exercise 2**, but now using an **A\*** search so that the winning move sequence is **provably optimal** (i.e. no shorter winning sequence exists).

Additionally, **no uncertainty‐reduction phase** is required, tests are constructed so that you can start directly with A\*.  

### Input E3

Read from `zad3_input.txt`

The file contains a series of lines (all the same length), each consisting solely of the characters `#`, `G`, `S`, `B`, or  ` `.

### Output E3

Write to `zad3_output.txt`

Output a single line with your move sequence consisting only of the characters `U`, `D`, `L` or `R`. There should be no breaks in between the characters.

> **Note:** There may be multiple valid solutions, because of that the tests can't be simply checked with a pre-made out3_X.txt, therefore the one can check the validity of the solution with a `--verbose` flag

### Solution E3

The solution consists of one file:  
[src/zad3.cpp](src/zad3.cpp).

## Exercise 4: Inadmissible Heuristic Tuning

Building on your solver from **Exercise 3**, you now must **modify** the heuristic so that it becomes **inadmissible** (i.e. no longer guaranteed to preserve optimality). Your modified heuristic must introduce a tunable parameter **ε** (the “degree of inadmissibility”).

Use tests from [Exercise 3](#exercise-3-optimal-commando-path-with-a-search).

### Input E4

Read from `zad4_input.txt`

The file contains a series of lines (all the same length), each consisting solely of the characters `#`, `G`, `S`, `B`, or  ` `.

### Output E4

Write to `zad4_output.txt`

Output a single line with your move sequence consisting only of the characters `U`, `D`, `L` or `R`. There should be no breaks in between the characters.

> **Note:** There may be multiple valid solutions, because of that the tests can't be simply checked with a pre-made out4_X.txt, therefore the one can check the validity of the solution with a `--verbose` flag

### Solution E4

The solution consists of one file:  
[src/zad4.cpp](src/zad4.cpp).

<details>
  <summary>Notes about solution</summary>
  The code is in a separate file, because the python-based validator provided by the professor didn't allow for easy <i>option setting</i> with command-line arguments, and at the time i forgot I could just add a make flag.
</details>

## Exercise 5: Hybrid Commando Solver

Combine and extend your solutions from Exercises 2–4 into a single hybrid algorithm that:

1. **Solves** all of the **Exercise 4** test mazes within the time limit (same input format, same size constraints).  
2. **Produces** overall **shorter** move sequences (sum of lengths) than your pure BFS‐based solution from **Exercise 2**.

Use tests from [Exercise 2](#exercise-2-commando-in-a-maze) and [Exercise 3](#exercise-3-optimal-commando-path-with-a-search).

### Input E5

Read from `zad5_input.txt`

The file contains a series of lines (all the same length), each consisting solely of the characters `#`, `G`, `S`, `B`, or  ` `.

### Output E5

Write to `zad5_output.txt`.

Output a single line with your move sequence consisting only of the characters `U`, `D`, `L` or `R`. There should be no breaks in between the characters.

> **Note:** There may be multiple valid solutions, because of that the tests can't be simply checked with a pre-made out5_X.txt, therefore the one can check the validity of the solution with a `--verbose` flag

### Solution E5

The solution consists of one file:  
[src/zad5.cpp](src/zad5.cpp).

<details>
  <summary>Notes about solution</summary>

For the same reasons as in [Exercise 4](#solution-e4), exercise 5 is a standalone program.
</details>

---

## Makefile usage

Run `make X` to build solution for the `X`-th exercise, where `X` is 1-5.

Run `make clean` to remove all executables.

---

> **Source:**
> This list of problems was assigned as part of the *Artificial Intelligence* (SI) course in the 2025 Summer semester at the University of Wrocław by Paweł Rychlikowski
