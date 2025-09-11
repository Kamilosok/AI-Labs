#pragma once

#include <cmath>
#include <vector>
#include <string.h>
#include <stdint.h>
#include <array>
#include <random>
#include <chrono>
#include <algorithm>

enum AgentType : uint8_t
{
    agentRandSim = 0,
    agentMCTS,
    agentRand,
    agentDenRand,
    agentQLearn
};

enum Animal : uint8_t
{
    Rat = 0,
    Cat,
    Dog,
    Wolf,
    Jaguar,
    Tiger,
    Lion,
    Elephant
};

inline constexpr std::array<double, 8> animalValues = {.4, 0.1, 0.2, .3, .5, .7, .8, 1.0};

inline char
getAnimalChar(Animal a, bool isUp)
{
    switch (a)
    {
    case Rat:
        return isUp ? 'R' : 'r';
    case Cat:
        return isUp ? 'C' : 'c';
    case Dog:
        return isUp ? 'D' : 'd';
    case Wolf:
        return isUp ? 'W' : 'w';
    case Jaguar:
        return isUp ? 'J' : 'j';
    case Tiger:
        return isUp ? 'T' : 't';
    case Lion:
        return isUp ? 'L' : 'l';
    case Elephant:
        return isUp ? 'E' : 'e';
    default:
        return '?';
    }
}

struct pos_t
{
    char x;
    char y;
};

inline pos_t rotatePos(pos_t pos)
{
    pos_t outPos;
    outPos.x = (char)(6 - pos.x);
    outPos.y = (char)(8 - pos.y);
    return outPos;
}

inline constexpr pos_t UP = pos_t{.x = 0, .y = -1};
inline constexpr pos_t DOWN = pos_t{.x = 0, .y = 1};
inline constexpr pos_t RIGHT = pos_t{.x = 1, .y = 0};
inline constexpr pos_t LEFT = pos_t{.x = -1, .y = 0};
using Move = std::pair<Animal, pos_t>;

inline pos_t rotateMove(pos_t move)
{
    pos_t outPos;
    outPos.x = -move.x;
    outPos.y = -move.y;
    return outPos;
}

inline constexpr std::array<pos_t, 4> moveList = {UP, DOWN, LEFT, RIGHT};

inline constexpr pos_t deadPos = pos_t{.x = -1, .y = -1};
inline constexpr pos_t upDen = {.x = 3, .y = 0};
inline constexpr pos_t downDen = {.x = 3, .y = 8};

inline constexpr uint8_t MAX_NO_CAPTURE = 50;

extern std::mt19937 gen;
// Potential amount of different moves, 2^3 animals, each one with 2^2 moves
extern std::uniform_int_distribution<uint8_t> moveDist;

constexpr bool operator==(const pos_t &lhs, const pos_t &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr bool operator!=(const pos_t &lhs, const pos_t &rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

struct state_t
{
    bool upMove = false;

    std::array<pos_t, static_cast<uint8_t>(Elephant) + 1>
        up =
            {pos_t{.x = 0, .y = 2}, pos_t{.x = 5, .y = 1}, pos_t{.x = 1, .y = 1}, pos_t{.x = 4, .y = 2}, pos_t{.x = 2, .y = 2}, pos_t{.x = 6, .y = 0}, pos_t{.x = 0, .y = 0}, pos_t{.x = 6, .y = 2}};
    std::array<pos_t, static_cast<uint8_t>(Elephant) + 1> down =
        {pos_t{.x = 6, .y = 6}, pos_t{.x = 1, .y = 7}, pos_t{.x = 5, .y = 7}, pos_t{.x = 2, .y = 6}, pos_t{.x = 4, .y = 6}, pos_t{.x = 0, .y = 8}, pos_t{.x = 6, .y = 8}, pos_t{.x = 0, .y = 6}};

    // Pos access helper
    const pos_t &animalAt(Animal a, bool isUp) const
    {
        return isUp ? up[static_cast<uint8_t>(a)]
                    : down[static_cast<uint8_t>(a)];
    }
};

inline state_t rotateState(state_t state)
{
    state_t outState;
    for (uint8_t a = static_cast<uint8_t>(Rat); a <= static_cast<uint8_t>(Elephant); a++)
    {
        outState.up[a] = rotatePos(state.up[a]);
        outState.down[a] = rotatePos(state.down[a]);
    }

    outState.upMove = state.upMove;

    return outState;
}

constexpr bool operator==(const state_t lhs, const state_t rhs)
{
    for (uint8_t i = 0; i <= static_cast<uint8_t>(Elephant); i++)
    {
        if (lhs.down[i] != rhs.down[i] || lhs.up[i] != rhs.up[i])
        {
            return false;
        }
    }

    return lhs.upMove == rhs.upMove;
}