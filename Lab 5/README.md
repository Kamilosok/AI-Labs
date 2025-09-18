# Lab 5

> This Exercise list had a lot of exercises for getting extra point during the course, because of this I document only the exercises I solved.

## Exercise 2: Square Packing

You have 24 squares with side lengths 1, 2, 3, ..., 24. Write a program that **places these squares on a 70×70 square board** so that the following conditions are met:

1. The coordinates of the **top-left corner** of each square are integers.
2. Every square **fits entirely** on the board.
3. Squares **do not overlap** (they may touch at edges or corners).
4. The sides of the squares are **parallel** to the corresponding sides of the board.
5. The **unused area of the board** should be minimized as much as possible.

### Output Format

- **First line:** the number of empty squares on the board.
- **Next 70 lines:** each line contains 70 characters representing the placement of the squares.
  - Use `.` for empty cells.
  - Use letters `A`-`X` to represent squares of size 1-24, respectively:
    - `A` → square of side 1
    - `B` → square of side 2
    - ...
    - `X` → square of side 24

### Scoring

Points are awarded as follows:

- **2p** - solution meets the specification.
- **1p** - ≤1000 empty cells.
- **1p** - ≤800 empty cells, program runs in **a few seconds**.
- **1p** - if your program uses **1 or 2 ideas/algorithms from the lectures** (indicate which ones).

Additionally, a **bonus** of `max(0, (200 - W)/100)` is awarded, where `W` is the number of empty cells. To get this bonus, your program may run longer—but short enough to be demonstrable in the lab.

> I included the scoring system because I quickly wrote a program to get *some* points, not all of them.

### Solution E2

The solution consists of one file:  
[src/Main.cpp](src/Main.cpp) and [include/Common.hpp](include/Common.hpp).

<details>
  <summary>Notes about solution</summary>
    The solution uses a slightly improved greedy/dfs algorithm.
</details>

## Exercise 7: Jungle MDP and Q-Learning

Consider the **Jungle game** as a Markov Decision Process (MDP) under two variants:

**Variant A:** The opponent selects moves uniformly at random from all legal moves.
**Variant B:** The opponent tries to move **towards the agent's den** (i.e., a move that decreases the Manhattan distance of one of their pieces to the opponent's den).

- If no such move exists, they move randomly.
- If such a move exists, flip a fair coin:
  - **Heads:** move randomly
  - **Tails:** pick one of the moves toward the den

In both variants, play continues until the game ends. The **reward** is `+1` or `−1` depending on the winner, with a discount factor **γ = 0.99**, and the opponent always moves second.

### Task

Use **Q-learning** to train an agent (or agents) that achieves a **positive expected reward** in both variants.  

- Approximate the Q-function with a **linear function** over features of the `(state, action)` pair.  
- Features should account for **captures** and differentiate between the types of pieces.  
- Present the **learned parameters** and comment on whether they align with your **intuitive expectations** of the game.

### Solution E7

The Q-learning agent is in:

[Lab 4/src/JungleQLearn.cpp](../Lab%204/src/JungleQLearn.cpp)

While the variant A and B agents are in:

[Lab 4/src/JungleRand.cpp](../Lab%204/src/JungleRand.cpp) and [Lab 4/src/JungleDenRand.cpp](../Lab%204/src/JungleDenRand.cpp) respectively.

[src/Main.cpp](src/Main.cpp) – entrypoint for running the MCTS agents

This solution reuses the common Jungle infrastructure from:

- [Lab 4/src/JungleBase.cpp](../Lab%204/src/JungleBase.cpp), [Lab 4/include/JungleBase.hpp](../Lab%204/include/JungleBase.hpp)
- [Lab 4/src/JungleAgent.cpp](../Lab%204/src/JungleAgent.cpp), [Lab 4/include/JungleAgent.hpp](../Lab%204/include/JungleAgent.hpp)
- [Lab 4/include/Common.hpp](../Lab%204/include/Common.hpp)

<details>
  <summary>Notes about solution</summary>
    The solution is in the Lab 4 folder because otherwise it would require duplicating Base files and the ease of polymorphing the agent for using different algorithms was simply too convenient to ignore.
</details>
