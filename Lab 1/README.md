# Lab 1

## Exercise 1: Cooperative King-and-Rook vs. King Endgame

In this problem we study a cooperative chess endgame in which **Black** has only a king, and **White** has a king and a rook. Black “helps” White by playing moves that lead more quickly to checkmate. Your task is to compute, for each given position and side to move, the **minimum number of moves** required to reach checkmate.

### Rules and Definitions

- The **rook** moves any number of squares along a row or column.  
- Each **king** moves exactly one square in any direction.  
- Black’s king **must never move into check**.  
- Castling is assumed to have already been performed by White (no further castling rights).  
- A **stalemate** occurs if the side to move has no legal move but is not in check. In our cooperative variant this is a terminal non-winning state.
Your program must support two modes:

1. **Batch mode**: No additional steps.  
2. **Debug mode**: Print the sequence of positions leading to mate (e.g. via a `print` routine) while reading and writing the same as in batch mode; no graphical display is required.

### Input E1

Read from `zad1_input.txt`

In **either** mode, each test case is given on one line in this format:

```text
<side-to-move> <WhiteKing> <WhiteRook> <BlackKing>
```

- `<side-to-move>` is either `white` or `black`.  
- Piece positions use standard algebraic notation (e.g. `e4`, `h1`).  

**Example input lines**:

```text
black c4 c8 h3
white a1 b2 c3
```

### Output E1

Write to `zad1_output.txt`

- If checkmate can be forced (with Black’s cooperation), output the **minimum number of moves** required.
- If the position inevitably leads to stalemate before mate, output: `INF`

<details>
  <summary>Notes about solution</summary>
    In my debug mode I print the whole chessboard.
</details>

---

## Exercise 2: Text Reconstruction by Max-Square Segmentation

In this task you will reconstruct a text with no spaces by inserting spaces so that:

1. Removing all spaces from your result yields the original text.  
2. Every segmented token (sequence between spaces) is a valid word from the provided dictionary.  
3. Among all valid segmentations, choose the one that **maximizes** the sum of the **squares** of token lengths. This criterion favors longer words over shorter ones.

### Dictionary

The dictionary **S** is supplied in the file `polish_words.txt` (one UTF-8 word per line).

### Input E2

Read from `zad2_input.txt`, one line at a time.

Each line contains a UTF-8 string **t** without any spaces.

### Output E2

Write to `zad2_output.txt`, one line per input line.

The input string **t** with spaces inserted according to the optimal segmentation rules above.

### Example E2

- **Input line:**  

`tamatematykapustkinieznosi`

- **Correct output:**  

`ta matematyka pustki nie znosi`

**Not** e.g.

`tama tematy kapustki nie z nosi`

<details>
  <summary>Notes about solution</summary>
    These additional steps were requested by the proffesor or Exercise instructor for education purposes. <br>
    In the <b>utils/</b> folder zad2Random uses a random segmentation method, tadeusz_process.cpp was used to prepare <i> Pan Tadeusz </i> by Adam Mickiewicz for this exercise, and tadeusz_comp.cpp to compare the output from a program using the max-square method to a program using a random method.
</details>

## Exercise 3: Poker-Hand Simulation – “Face Player” vs. “Pip Player"

In this variant of five-card poker (no draws), two players each draw 5 cards and compare hand strengths:

1. **Face Player** draws from a deck containing only high ranks (A, K, Q, J).  
2. **Pip Player** draws from a deck containing only low ranks (2 through 10).  

Your goal is to **estimate** (by simulation) the probability that **Pip Player’s** hand beats **Face Player’s**.

**Note:** You do **not** need to implement the *full* poker‐hand ranking engine.

Using this program *(and conducting several experiments)*, we will check how the probability of success changes if we allow the  **Pip Player** to discard a certain number of selected cards before the draw (in other words, we allow the **Pip Player** to compose his own deck, of course consisting of Pips).

Propose a deck for the **Pip Player** (with as many cards as possible) that will statistically give him victory.

<details>
  <summary>Notes about solution</summary>
    The main program estimates the <b>Pip Player's</b> victory chance by generating random valid hands for both players and comparing them. The program may take in one argument, the amount of low-ranked pips that the <b>Pip player</b> discards. <br>
    Tests show that higher amount of discards increase victory chance. <br>
    In the <b>utils/</b> folder zad3Spec.cpp program computes the exact victory chance without discards. <br>
    In the original problem spec the author used weird naming conventions (<b>Blotkarz</b> for <b>Pip Player</b> and <b>Figurant</b> for <b>Face Player</b>), which is reflected in some naming conventions in the code.
</details>

## Exercise 4: Single-Block Nonogram Edit Distance

We consider a simplified nonogram row (or column) described by a sequence of bits (`0` = empty, `1` = filled). There is at most one continuous block of black cells, of some target length **D**.

Your goal is to find the **minimum number of bit flips** required so that the final sequence contains exactly one run of **D** consecutive `1`s (which may touch the ends) and all other bits are `0`.

### Input E4

Read from `zad4_input.txt`.

One line that contains: `bits`: a string of `0` and `1` characters of some length **N** and `D`: an integer (0 ≤ `D` ≤ **N**), the desired block length, separated by a space.

### Output E4

Write to `zad4_output.txt`.

Output a single integer `m`, so that `m` is the minimum number of bit flips from the input sequence so that a sequence of bits that satisfies the exercise conditions can be achieved.

### Example E4

For the initial sequence `0010001000`:

| D | Minimum Flips |
|---|---------------|
| 5 | 3             |
| 4 | 4             |
| 3 | 3             |
| 2 | 2             |
| 1 | 1             |
| 0 | 2             |

## Exercise 5: Heuristic Nonogram Solver (WalkSAT-Style)

Implement a simplified nonogram solver using the following heuristic *(inspired by WalkSAT)*:

1. **Initialization**  
   Start from an initial coloring of the grid (e.g. random or all zeros).

2. **Iterative Repair**  
   Repeat until the puzzle is solved or a time limit is reached:
   - **Select** a violating line (row or column), one whose black cells do not form exactly one contiguous block of the required length.
   - **Choose** a single cell `(i,j)` in that line whose flip yields the greatest improvement in the total *“fit score"* of both row `i` and column `j`.
   - With small probability, make a **non-optimal** flip (to escape local minima).

3. **Random Restart**  
   If no solution is found after many iterations, reinitialize randomly and repeat.

### Input E5

Read from `zad5_input.txt`.

The first line contains two integers **X** and **Y** where **X** is the number of rows and **Y** is the number of columns, the next **X** lines contain descriptions of rows, which consist of at least **one non-zero** number that smaller than the amount of lines of the opposite axis. They are followed by **Y** descriptions of columns which follow the same rules.

### Output E5

Write to `zad5_output.txt`.

Exactly **X** lines, each with **Y** characters so that the image fulfills the nonogram:

- `.` for an empty cell  
- `#` for a filled (black) cell

> **Note:** There may be multiple valid solutions.

---

> **Source:**
> This list of problems was assigned as part of the **Artificial Intelligence* (SI) course in the 2025 Summer semester at the University of Wrocław by Paweł Rychlikowski
