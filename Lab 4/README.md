# Lab 4

For testing your agents in these exercises you may use the [dueler](#dueler-protocol).

> The dueler isn't finished at this time.

## Exercise 1: Reversi agent

> I didn't solve this exercise

## Exercise 2: Defeating reversi bosses

> I didn't solve this exercise

## Exercise 3: Monte-Carlo Jungle Agent

You must implement an **agent** that chooses one legal move in the game *Jungle* (aka Dou Shou Qi / Animal Chess) using **Monte-Carlo** (random playout) evaluation of the resulting positions.

---

## Brief game summary

>The rules differ slightly from any rules i found on the internet, so they are described as the validator interprets the game.

- Board: **7 columns × 9 rows**. Use columns `a`...`g` left to right and rows `1`...`9` top to bottom.
- Cells types: meadow `.` , trap `#` , den `*` , water `~`. The board looks as follows:

```text
..#*#..
...#...
.......
.~~.~~.
.~~.~~.
.~~.~~.
.......
...#...
..#*#..
```

- Pieces: `R`(rat), `C`(cat), `D`(dog), `W`(wolf), `J`(Jaguar), `T`(tiger), `L`(lion), `E`(elephant). Uppercase = upper player, lowercase = lower player.
- The order in the previous rule also defines the seniority of the pieces. Initially, the pieces are arranged as follows:

```text
L.....T
.D...C.
R.J.W.E
.......
.......
.......
e.w.j.r
.c...d.
t.....l
```

The following movement rules apply:

- A player **cannot** enter their own hole.
- **Only** the rat can enter the water.
- A normal move is to move the piece to an adjacent free space, up, down, left, or right.
- The tiger and lion can **jump** over water (to make a jump, the piece must “as if” enter the water and then move in the same direction until it reaches a space that is not water).
- It is **not** allowed to jump over an enemy rat.
- Entering a square occupied by another piece is equivalent to **capturing** it. You can capture a piece of **equal strength or weaker**. You cannot enter squares with your own piece or with a stronger piece.
- The rat *(contrary to seniority)* is stronger than the elephant. Otherwise, the seniority of the pieces corresponds to their strength.
- The rat **cannot** capture when moving from the lake to the land.
- A piece in a trap (one of the squares surrounding the pit) loses all its strength and can be captured by **any** piece. This applies to both traps defending access to the player's den and to the opponent's den.
- The aim of the game is to enter the opponent's den with a piece. After such a move, the game ends and the player who enters the den wins.

---

## Agent specification

The agent should choose moves as follows:

1. **Generate** the set of all legal moves for the side to move and. For each move `m` in this set, simulate it and **assess** the board situaton, we'll call the set of states received this way `S`.

2. **Assess** every situation `s ∈ S` by running *random playouts* (fully random legal play for both sides) starting from  and recording outcomes (win/loss/draw). These playouts are independent Monte-Carlo trials; a trial ends when a terminal state is reached (a den is entered, a player has no legal moves and is lost/drawn per rules you apply), or after a move cap per trial.

3. **Budget**: the agent is allowed to simulate at most **N** *moves* in total across all simulations. A recommended default is `N = 20000`.

4. **Distribution**: distribute the simulation budget *approximately evenly* among `|S|` successor states so each successor receives a similar amount of playouting work. Ensure each successor is tested at least once if possible.

5. **Selection**: choose the move `m` whose state `s` yields the best estimated score (e.g. highest proportion of playouts won by the side to move).

### Solution E3

The solution is located in the following files:

- [src/JungleRandSim.cpp](src/JungleRandSim.cpp) – implementation of the Monte-Carlo style Jungle agent
- [src/Main.cpp](src/Main.cpp) – entrypoint used to run the agent

This solution reuses the common Jungle infrastructure from:

- [src/JungleBase.cpp](src/JungleBase.cpp), [include/JungleBase.hpp](include/JungleBase.hpp)
- [src/JungleAgent.cpp](src/JungleAgent.cpp), [include/JungleAgent.hpp](include/JungleAgent.hpp)
- [include/Common.hpp](include/Common.hpp)

## Exercise 4: Heuristic Jungle Agent

Write an agent that consistently defeats the agent from **Exercise 3**.

Unlike the random playout-based strategy, this agent must rely on a **heuristic evaluation function** to assess moves or board positions. The heuristic can use any reasonable factors you design *(e.g., material balance, positional features, control of traps, proximity to the opponent’s den, etc.)*. The agent may run up to 4x slower than the **Exercise 3** agent.

### Solution E4

The solution is located in the following files:

- [src/JungleMCTS.cpp](src/JungleMCTS.cpp) – the implementation of the Monte-Carlo Jungle agent uses a heuristic function
- [src/Main.cpp](src/Main.cpp) – entrypoint used to run the agent

This solution reuses the common Jungle infrastructure from:

- [src/JungleBase.cpp](src/JungleBase.cpp), [include/JungleBase.hpp](include/JungleBase.hpp)
- [src/JungleAgent.cpp](src/JungleAgent.cpp), [include/JungleAgent.hpp](include/JungleAgent.hpp)
- [include/Common.hpp](include/Common.hpp)

## Exercise 5: Jungle Implementation

Implement the Jungle game inteface without the validator.
> By default a Jungle interface was available via a python program.

### Solution E5

The Jungle game interface was implemented in:

- [src/JungleBase.cpp](src/JungleBase.cpp)  
- [include/JungleBase.hpp](include/JungleBase.hpp)
- [include/Common.hpp](include/Common.hpp)

Other agents rely on this implementation.

<details>
  <summary>Notes about solution</summary>
    Due to me not using the preffered language for this course (Python), I implemented the game before any agents.
</details>

## Exercise 6: Jungle bosses

Adjust your agents' communication protocol to the [dueler protocol](#dueler-protocol) and defeat the prepared bosses.

<details>
  <summary>Notes about solution</summary>
  All of my agents use this protocol, so this exercise was solved using the smartest one by testing (it turned out to be the MCTS).
</details>

## Exercise 7: Chess agent

> I didn't solve this exercise

## Exercise 8: Chess bosses

> I didn't solve this exercise

## Exercise 9: Monte Carlo Tree Search

Choose one game: **Jungle**, **Chess**, or **Reversi**.  
Implement the **Monte Carlo Tree Search (MCTS)** algorithm for it, and then:

**a)** Tune the parameters of both programs so that a single move takes about **0.5 seconds**.  
**b)** Play a match of **10 games** (each program starts 5 times), and record the results.  

You may use `dueler.py` for your experiments.  
If you have already used **MCTS** for the chosen game in a previous exercise, then in this exercise you should instead implement **Alpha-Beta Search**.

### Solution E9

The solution consists of:

- [src/JungleMCTS.cpp](src/JungleMCTS.cpp)  
- [src/Main.cpp](src/Main.cpp) – entrypoint for running the MCTS agent  

This solution reuses the common Jungle infrastructure from:

- [src/JungleBase.cpp](src/JungleBase.cpp), [include/JungleBase.hpp](include/JungleBase.hpp)
- [src/JungleAgent.cpp](src/JungleAgent.cpp), [include/JungleAgent.hpp](include/JungleAgent.hpp)
- [include/Common.hpp](include/Common.hpp)

<details>
  <summary>Notes about solution</summary>
    I obviously chose Jungle for this exercise, and solved it with MCTS.
</details>

## Dueler protocol

> Note: Rewriting the dueler would take some work, hence the testing for this Lab list should be done at one's convenience for testing.

Two player programs communicate with the dueler over **stdin/stdout** using a simple text protocol. All messages are ASCII lines terminated by `\n`.

The dueler alternates between two players P0 (lower) and P1 (upper).

Below are the exact messages, direction, expected formats, and semantics.

### Startup

Player -> Dueler

`RDY`

Sent by each player when it is ready to start. The dueler waits for both players to send `RDY`.

[See Timeouts](#timeouts-illegal-moves-and-errors)

### Starting a game

Dueler -> Player chosen as first

`UGO` `<move_time>` `<remaining_time>`

Example: `UGO` `1.500` `120.000`

`<move_time>` - maximum time (seconds, float) the player may take for this move.

`<remaining_time>` - the remaining time (seconds, float) the player has left in the whole game.

[See Timeouts](#timeouts-illegal-moves-and-errors)

After sending `UGO`, the dueler expects the current player to reply within `<move_time>` by emitting an `IDO` message [see below](#player-move-submission).

### Player move submission

Player -> Dueler

`IDO` `<move>`

Example: `IDO` `6 7 6 6`

The `<move>` must be the game-specific move string. For Jungle it's the `x` and `y` coordinates before and after moving a piece.

#### Semantics

If the move is illegal for the current game state, the dueler will treat it as a [wrong move](#timeouts-illegal-moves-and-errors).

The dueler will apply the move to the game state and check for terminal conditions. If the game ends, the dueler outputs the result and [terminates the game loop or starts a rematch](#game-end--rematch).

### Broadcasting opponent move

Dueler -> Opponent

`HEDID` `<move_time>` `<remaining_time>` `<move>`

Example: `HEDID` `2.47` `25.45` `4 2 5 2`

`<move_time>` and `<remaining_time>` defined the same way as in [Starting a game](#starting-a-game).

`<move>` defined the same way as in [Player move submission](#player-move-submission).

### Game end / Rematch

When a game finishes, the dueler prints one of the following on its own stdout *(these are dueler -> observer, not part of the player protocol)*:

`DRAW` - the game finished as a draw.

`P0 WON` - player 0 won.

`P1 WON` - player 1 won.

Then depending on the amount of games left/other factors the dueler will either terminate the players or initiate a rematch.

#### Termination of players

Dueler -> Both players

`BYE`

After sending `BYE`, the players should terminate, if not, the dueler will shortly kill the player processes.

#### Rematch

Dueler -> Both players

`ONEMORE`

After sending `ONEMORE`, the players should acknowledge that a game has ended and act as in [Startup](#startup).

### Timeouts, illegal moves, and errors

**Ready timeout**: if a player does not send `RDY` within the projected time frame, the dueler considers that player timed out.

**Move timeout**: if a player does not send a valid `IDO` within the `<move_time>` they were given, the dueler considers that player timed out.

**Player remaining time**: if the player’s `<remaining_time>` is exhausted before they make a move, the dueler considers that player timed out.

**Wrong/illegal move**: if the player sends an `IDO` move that is illegal in the current position or malformed, the dueler treats this as a wrong move.

**Harness exception handling**: on timeout or wrong move the dueler awards the win to the player that didn't initiate the exception, and proceeds as if a game ended normally.

> **Source:**
> This list of problems was assigned as part of the *Artificial Intelligence* (SI) course in the 2025 Summer semester at the University of Wrocław by Paweł Rychlikowski
