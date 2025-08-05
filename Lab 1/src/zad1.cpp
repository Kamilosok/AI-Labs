#include <fstream>
#include <string>
#include <cstring>
#include <queue>
#include <set>
#include <stack>

bool debugMode = false;

// A structure that stores the state of the board (effectively a vertex in a graph for BFS)
struct game
{
    bool wPrio;
    std::pair<char, char> wKing;
    std::pair<char, char> wTower;
    std::pair<char, char> bKing;
    unsigned char depth;

    bool operator<(const game &other) const
    {
        return std::tie(depth, wPrio, wKing, wTower, bKing) < std::tie(other.depth, other.wPrio, other.wKing, other.wTower, other.bKing);
    }

    // For returning in debug mode
    game *father;
};

/*
In debug mode, enabled with the ‘-d’ argument, the board looks like this:

  a b c d e f g h
0 . . . . . . . .
1 . . . . . . . .
2 . . . . . . . .
3 . . . . . . . .
4 . . . . . . . .
5 . . . . . . . .
6 . . . . K . . .
7 . . . . . T k .

K - White king, T - White rook, k - Black king
*/

void printGame(game game)
{
    printf("  a b c d e f g h");
    for (int i = 0; i < 64; i++)
    {
        bool wKingGo = game.wKing.first == i % 8 && game.wKing.second == i / 8;
        bool wTowerGo = game.wTower.first == i % 8 && game.wTower.second == i / 8;
        bool bKingGo = game.bKing.first == i % 8 && game.bKing.second == i / 8;
        char square = wKingGo ? 'K' : wTowerGo ? 'T'
                                  : bKingGo    ? 'k'
                                               : '.';

        if (i % 8 == 0)
            printf("\n%d", i / 8);

        printf(" %c", square);
    }
    puts("\n");
}

bool checkCheck(game currGame)
{
    if (currGame.bKing.first == currGame.wTower.first || currGame.bKing.second == currGame.wTower.second)
        return true;
    else
        return false;
}

// Bfs on graph of possible board states
int moveGame(game startGame)
{
    bool stalemate = false;
    std::queue<game> games;
    std::set<game> uniqueGames;
    std::stack<game> progression;
    games.push(startGame);

    while (!games.empty())
    {
        char currDepth = games.front().depth;

        game currGame = games.front();
        long long gameNum = uniqueGames.size();
        games.pop();
        uniqueGames.insert(currGame);

        // Doesn't work if the solution requires going back and forth
        while (uniqueGames.size() == gameNum && currDepth >= 8)
        {
            currGame = games.front();
            uniqueGames.insert(currGame);
            games.pop();
        }

        if (currDepth > 64 + 64 + 2) // Time for the kings to get into any position + 2 moves for the tower
            return -1;

        // For ease of access
        const char &wKingx = currGame.wKing.first, &wKingy = currGame.wKing.second, &wTowerx = currGame.wTower.first, &wTowery = currGame.wTower.second,
                   &bKingx = currGame.bKing.first, &bKingy = currGame.bKing.second;

        if (currGame.wPrio)
        {

            auto xDiff = [bKingx](char x) -> char
            {
                return std::abs(bKingx - x);
            };

            auto yDiff = [bKingy](char y) -> char
            {
                return std::abs(bKingy - y);
            };

            auto isValidMoveK = [&](char x, char y) -> bool
            {
                // Edges
                if (x < 0 || x > 7 || y < 0 || y > 7)
                    return false;

                // Tower
                if (x == wTowerx && y == wTowery)
                    return false;

                // Black king
                if (xDiff(x) < 2 && yDiff(y) < 2)
                    return false;

                return true;
            };

            char possibleMovesK[8][2] =
                {
                    {wKingx - 1, wKingy - 1}, {wKingx, wKingy - 1}, {wKingx + 1, wKingy - 1}, {wKingx - 1, wKingy}, {wKingx + 1, wKingy}, {wKingx - 1, wKingy + 1}, {wKingx, wKingy + 1}, {wKingx + 1, wKingy + 1}};

            for (auto [x, y] : possibleMovesK)
            {
                if (isValidMoveK(x, y))
                {
                    game newGame{false, {x, y}, {wTowerx, wTowery}, {bKingx, bKingy}, currDepth + 1, const_cast<game *>(&(*(uniqueGames.find(currGame))))};
                    games.push(newGame);
                }
            }

            // Only moves close to the black king
            char possibleMovesT[6][2] =
                {
                    {bKingx - 1, wTowery}, {bKingx, wTowery}, {bKingx + 1, wTowery}, // Horizontal move
                    {wTowerx, bKingy - 1},
                    {wTowerx, bKingy},
                    {wTowerx, bKingy + 1} // Vertical move
                };

            auto isValidMoveT = [&](char x, char y) -> bool
            {
                // Edges
                if (x < 0 || x > 7 || y < 0 || y > 7)
                    return false;

                // To not trample the king
                if (x == bKingx && y == bKingy)
                    return false;

                return true;
            };

            for (auto [x, y] : possibleMovesT)
            {
                if (isValidMoveT(x, y))
                {
                    game newGame{false, {wKingx, wKingy}, {x, y}, {bKingx, bKingy}, currDepth + 1, const_cast<game *>(&(*(uniqueGames.find(currGame))))};
                    games.push(newGame);
                }
            }
            // The white king can't be checked
        }
        else
        {
            bool check = checkCheck(currGame);
            bool moved = false; // White can always move if proper starting positions are given

            // Cardinal directions
            auto xDiff = [wKingx](char x) -> char
            {
                return std::abs(wKingx - x);
            };

            auto yDiff = [wKingy](char y) -> char
            {
                return std::abs(wKingy - y);
            };

            auto isValidMove = [&](char x, char y) -> bool
            {
                // Edges
                if (x < 0 || x > 7 || y < 0 || y > 7)
                    return false;

                // Tower cover
                if (x == wTowerx || y == wTowery)
                    return false;

                // White king
                if (xDiff(x) < 2 && yDiff(y) < 2)
                    return false;

                return true;
            };

            char possibleMoves[8][2] =
                {
                    {bKingx - 1, bKingy - 1}, {bKingx, bKingy - 1}, {bKingx + 1, bKingy - 1}, {bKingx - 1, bKingy}, {bKingx + 1, bKingy}, {bKingx - 1, bKingy + 1}, {bKingx, bKingy + 1}, {bKingx + 1, bKingy + 1}};

            for (auto [x, y] : possibleMoves)
            {
                if (isValidMove(x, y))
                {
                    moved = true;
                    game newGame{true, {wKingx, wKingy}, {wTowerx, wTowery}, {x, y}, currDepth + 1, const_cast<game *>(&(*(uniqueGames.find(currGame))))};
                    games.push(newGame);
                }
            }

            if (moved == false)
                if (check) // If a check is in place, exit, if not then a stalemate
                {
                    // Check for forced taking of the tower
                    if (wTowerx == bKingx && std::abs(wTowery - bKingy) == 1)
                    {
                        if (xDiff(wTowerx) >= 2 || yDiff(wTowery) >= 2)
                            stalemate = true;
                        else
                        {
                            printGame(currGame);
                            return currDepth;
                        }
                    }
                    else if (wTowery == bKingy && std::abs(wTowerx - bKingx) == 1)
                    {
                        if (xDiff(wTowerx) >= 2 || yDiff(wTowery) >= 2)
                            stalemate = true;
                        else
                        {
                            if (debugMode)
                            {
                                progression.push(currGame);
                                game drawer = *currGame.father;
                                while (drawer.father != nullptr)
                                {
                                    progression.push(drawer);
                                    drawer = *drawer.father;
                                }

                                while (!progression.empty())
                                {
                                    game prog = progression.top();
                                    printGame(prog);
                                    progression.pop();
                                }
                            }

                            return currDepth;
                        }
                    }
                    else
                    {
                        if (debugMode)
                        {
                            progression.push(currGame);
                            game drawer = *currGame.father;
                            while (drawer.father != nullptr)
                            {
                                progression.push(drawer);
                                drawer = *drawer.father;
                            }

                            while (!progression.empty())
                            {
                                game prog = progression.top();
                                printGame(prog);
                                progression.pop();
                            }
                        }

                        return currDepth;
                    }
                }
                else
                    stalemate = true;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    std::ios::sync_with_stdio(false);

    bool inGiven = 0, outGiven = 0;

    std::ifstream in;
    std::ofstream out;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (std::strcmp(arg.c_str(), "-d") == 0)
            debugMode = 1;
        else if (!inGiven)
        {
            in.open(arg, std::fstream::in);

            if (!in)
            {
                throw std::runtime_error("Cannot open input file!");
            }
            inGiven = 1;
        }
        else
        {
            out.open(arg, std::fstream::out);
            if (!out)
            {
                throw std::runtime_error("Cannot open output file!");
                in.close();
            }
            outGiven = 1;
        }
    }

    if (!inGiven || !outGiven)
    {
        if (in)
            in.close();
        if (out)
            out.close();
        fprintf(stderr, "Both input and output files need to be specified!\n");

        return 1;
    }

    while (true)
    {
        std::string prio, wKing, wTower, bKing;
        in >> prio;

        if (in.eof())
            break;

        in >> wKing >> wTower >> bKing;

        bool gameWPrio = false;
        std::pair<char, char> gameWKing, gameWTower, gameBKing;

        if (strcmp(prio.c_str(), "white") == 0)
            gameWPrio = true;

        gameWKing.first = wKing[0] - 'a';
        gameWKing.second = wKing[1] - '1';

        gameWTower.first = wTower[0] - 'a';
        gameWTower.second = wTower[1] - '1';

        gameBKing.first = bKing[0] - 'a';
        gameBKing.second = bKing[1] - '1';

        game newGame{gameWPrio, gameWKing, gameWTower, gameBKing, 0, nullptr};

        int minMoves = moveGame(newGame);

        if (minMoves != -1)
        {
            printf("%d\n", minMoves);
            out << minMoves;
        }
        else
        {
            puts("INF\n");
            out << "INF";
        }
    }

    in.close();
    out.close();
    return 0;
}