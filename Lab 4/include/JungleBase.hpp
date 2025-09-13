#pragma once
#include "Common.hpp"

#include <ios>

// Base class for operating on Jungle states
class JungleBase
{
protected:
    static char getTile(char x, char y);

public:
    static void printState(const state_t &state);
    static bool checkWin(const state_t &state, bool playsUp);
    static bool checkCapture(const state_t &state, const Move &move);
    static bool validateMove(const state_t &state, Animal animal, pos_t dest);
    static bool moveState(const state_t &state, Move move, state_t &outState);
    static void genMoves(const state_t &state, std::vector<Move> &possMoves);
    static void genMoves(const state_t &state, std::vector<Move> &possMoves, bool rotated);
};