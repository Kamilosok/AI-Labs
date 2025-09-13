#pragma once

#include <cmath>
#include <vector>
#include <string.h>
#include <stdint.h>
#include <array>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <ios>

typedef char Board[70][70];
typedef bool Squares[24];

constexpr int N = 24;
constexpr int BOARD_SIZE = 70;
static Board bestBoard;
int bestFree = BOARD_SIZE * BOARD_SIZE;
std::vector<int> sizes;
std::vector<int> prefixArea;

void fillBoard(Board &B)
{
    for (int y = 0; y < 70; y++)
    {
        for (int x = 0; x < 70; x++)
        {
            B[y][x] = '.';
        }
    }
}

void fillSquares(Squares &S)
{
    for (int i = 0; i < 24; i++)
        S[i] = false;
}

void copyBoard(const Board &in, Board *out)
{
    std::memcpy(*out, in, sizeof(Board));
}

void printBoard(const Board &B)
{
    for (int y = 0; y < 70; y++)
    {
        std::fwrite(B[y], sizeof(char), 70, stdout);
        std::putchar('\n');
    }
}

void findCandidates(const Board &B, int K, std::vector<std::pair<int, int>> &cands)
{
    cands.clear();
    for (int y = 0; y < BOARD_SIZE && (int)cands.size() < K; ++y)
    {
        for (int x = 0; x < BOARD_SIZE && (int)cands.size() < K; ++x)
        {
            if (B[y][x] == '.')
                cands.emplace_back(y, x);
            else
                x += B[y][x] - 'A';
        }
    }
}

unsigned int countFree(const Board &B)
{
    unsigned int free = 0;
    for (int y = 0; y < 70; y++)
    {
        for (int x = 0; x < 70; x++)
        {
            if (B[y][x] == '.')
                free += 1;
            else
                x += B[y][x] - 'A';
        }
    }
    return free;
}

bool insertSquare(const Board &B, unsigned int sqSize, unsigned int yStart, unsigned int xStart, Board *out)
{
    if (yStart + sqSize >= 70 || xStart + sqSize >= 70)
        return false;

    copyBoard(B, out);
    for (unsigned int y = yStart; y < yStart + sqSize; y++)
    {
        for (unsigned int x = xStart; x < xStart + sqSize; x++)
        {
            if (B[y][x] != '.')
                return false;

            (*out)[y][x] = 'A' + sqSize - 1;
        }
    }

    return true;
}
