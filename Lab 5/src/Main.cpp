#include "Common.hpp"

std::random_device rd;
auto seed = rd() ^ (std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);
std::uniform_int_distribution<uint8_t> moveDist(0, 2);

static std::vector<bool> findPlacedSquares(const Board &B)
{
    std::vector<bool> placed(N, false);

    for (int y = 0; y < BOARD_SIZE; ++y)
    {
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            char c = B[y][x];
            if (c >= 'A' && c < 'A' + N)
            {
                int idx = c - 'A';
                placed[idx] = true;
            }
        }
    }

    return placed;
}

void greedY(Board &B, std::vector<bool> used)
{
    for (unsigned int i = 24; i >= 1; i--)
    {
        if (used[i - 1])
            continue;
        bool inserted = false;
        for (unsigned int y = 0; y < 70; y++)
        {
            if (inserted)
                break;

            for (unsigned int x = 0; x < 70; x++)
            {
                Board next;
                if (insertSquare(B, i, y, x, &next))
                {
                    copyBoard(next, &B);
                    inserted = true;
                    break;
                }
            }
        }
    }
}

void greedX(Board &B, std::vector<bool> used)
{
    for (unsigned int i = 24; i >= 1; i--)
    {
        if (used[i - 1])
            continue;
        bool inserted = false;
        for (unsigned int y = 0; y < 70; y++)
        {
            if (inserted)
                break;

            for (unsigned int x = 0; x < 70; x++)
            {
                Board next;
                if (insertSquare(B, i, x, y, &next))
                {
                    copyBoard(next, &B);
                    inserted = true;
                    break;
                }
            }
        }
    }
}

void dfs(int idx, int placedArea, const Board &B)
{
    // Bound: optimistic estimate
    int optimistic = placedArea + (prefixArea[N] - prefixArea[idx]);
    int boundFree = BOARD_SIZE * BOARD_SIZE - optimistic;
    if (boundFree >= bestFree)
        return;

    if (idx == N)
    {
        // Try greedily by both coordinates
        auto usedX = findPlacedSquares(B);
        auto usedY = findPlacedSquares(B);

        Board nextX, nextY;
        copyBoard(B, &nextX);
        copyBoard(B, &nextY);

        greedX(nextX, usedX);
        greedY(nextY, usedY);

        int freeCellsX = countFree(nextX);
        int freeCellsY = countFree(nextY);

        bool Xbigger = freeCellsX >= freeCellsY;
        if (Xbigger)
        {
            if (freeCellsX < bestFree)
            {
                bestFree = freeCellsX;
                copyBoard(nextX, &bestBoard);
            }
        }
        else
        {
            if (freeCellsY < bestFree)
            {
                bestFree = freeCellsY;
                copyBoard(nextY, &bestBoard);
            }
        }

        return;
    }

    unsigned int s = N - idx;
    std::vector<std::pair<int, int>> cands;
    const int K = 210;
    findCandidates(B, K, cands);
    Board next;
    for (auto [y, x] : cands)
    {
        if (insertSquare(B, s, y, x, &next))
        {
            dfs(idx + 1, placedArea + s * s, next);
        }
    }

    // Skip
    dfs(idx + 1, placedArea, B);
}

int main()
{
    Board b;
    fillBoard(b);

    // Prefix sum of areas
    prefixArea.assign(N + 1, 0);

    for (int i = 0; i < N; ++i)
        prefixArea[i + 1] = prefixArea[i] + (N - i) * (N - i);

    dfs(0, 0, b);

    printf("%d\n", countFree(bestBoard));
    printBoard(bestBoard);

    auto used = findPlacedSquares(bestBoard);

    for (bool bb : used)
        printf("%d ", bb);

    return 0;
}