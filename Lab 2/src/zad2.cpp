#include <fstream>
#include <vector>
#include <set>
#include <queue>

using pos_t = std::pair<unsigned char, unsigned char>;
using state_t = std::set<pos_t>;

state_t startPositions;
std::vector<std::vector<char>> lab;

// Helper
const void printLabyrinth(state_t s)
{
    unsigned char currH = 0;
    for (const auto &row : lab)
    {
        unsigned char currW = 0;
        for (char cell : row)
        {
            bool startPos = s.contains({currH, currW});
            switch (cell)
            {
            case 0:
                printf("%c", startPos ? 'S' : ' ');
                break;
            case 1:
                printf("#");
                break;
            case -1:
                printf("%c", startPos ? 'B' : 'G');
                break;
            default:
                printf("?");
                break;
            }
            currW += 1;
        }
        currH += 1;
        printf("\n");
    }
}

// Move all possible commando locations in a direction
state_t movePositions(const state_t &s, const char &direction)
{
    state_t next;

    for (const auto &move : s)
    {
        switch (direction)
        {
        case 'U':
            if (lab[move.first - 1][move.second] != 1)
                next.insert({move.first - 1, move.second});
            else
                next.insert({move.first, move.second});
            break;

        case 'L':
            if (lab[move.first][move.second - 1] != 1)
                next.insert({move.first, move.second - 1});
            else
                next.insert({move.first, move.second});
            break;

        case 'D':
            if (lab[move.first + 1][move.second] != 1)
                next.insert({move.first + 1, move.second});
            else
                next.insert({move.first, move.second});
            break;

        case 'R':
            if (lab[move.first][move.second + 1] != 1)
                next.insert({move.first, move.second + 1});
            else
                next.insert({move.first, move.second});
            break;

        default:
            break;
        }
    }

    return next;
}

// Check if all possible commando locations are on a goal
bool isGoalState(const state_t &s)
{
    for (auto pos : s)
    {
        if (lab[pos.first][pos.second] != -1)
            return false;
    }
    return true;
}

std::string findWorkingPlan()
{
    std::queue<std::pair<state_t, std::string>> q;
    std::set<state_t> visited;

    q.push({startPositions, ""});
    visited.insert(startPositions);

    std::vector<char> moves = {'U', 'D', 'L', 'R'};

    while (!q.empty())
    {
        auto [currState, path] = q.front();
        q.pop();
        // printLabyrinth(currState);

        if (isGoalState(currState))
        {
            // printLabyrinth(currState);
            return path;
        }

        for (char m : moves)
        {
            state_t nextState = movePositions(currState, m);
            if (visited.find(nextState) == visited.end())
            {
                visited.insert(nextState);
                q.push({nextState, path + m});
            }
        }
    }
    return "";
}

int main(int argc, char *argv[])
{
    std::ios::sync_with_stdio(false);

    bool inGiven = 0, outGiven = 0, verbose = 0;

    std::ifstream in;
    std::ofstream out;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--verbose")
        {
            verbose = 1;
        }

        if (!inGiven)
        {
            in.open(arg, std::fstream::in);

            if (!in)
            {
                throw std::runtime_error("Cannot open input file!");
            }
            inGiven = 1;
        }
        else if (!outGiven)
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
    }

    std::string inS;
    unsigned char currH = 0;
    while (true)
    {
        std::getline(in, inS);
        lab.push_back({});
        unsigned char currW = 0;
        for (char c : inS)
        {
            // 1 - Wall, -1 - Goal, 0 Empty
            switch (c)
            {
            case '#':
                lab[currH].push_back(1);
                break;

            case 'G':
                lab[currH].push_back(-1);
                break;

            case 'B':
                lab[currH].push_back(-1);
                startPositions.insert({currH, currW});
                break;

            case 'S':
                lab[currH].push_back(0);
                startPositions.insert({currH, currW});
                break;

            default:
                lab[currH].push_back(0);
                break;
            }

            currW += 1;
        }
        currH += 1;

        if (in.eof())
            break;
    }

    std::string setupS = "";
    state_t verboseStartState = startPositions;

    // The initial uncertainty is reduced via a method found
    // using trial and error for the provided tests
    for (int i = 0; i < 10; i++)
    {
        startPositions = movePositions(startPositions, 'U');
        setupS += 'U';
    }

    for (int i = 0; i < 10; i++)
    {
        startPositions = movePositions(startPositions, 'L');
        setupS += 'L';
    }

    for (int i = 0; i < 10; i++)
    {
        startPositions = movePositions(startPositions, 'D');
        setupS += 'D';
    }

    for (int i = 0; i < 10; i++)
    {
        startPositions = movePositions(startPositions, 'L');
        setupS += 'L';
    }

    for (int i = 0; i < 10; i++)
    {
        startPositions = movePositions(startPositions, 'U');
        setupS += 'U';
    }

    for (int i = 0; i < 1; i++)
    {
        startPositions = movePositions(startPositions, 'R');
        setupS += 'R';
    }

    for (int i = 0; i < 2; i++)
    {
        startPositions = movePositions(startPositions, 'U');
        setupS += 'U';
    }

    for (int i = 0; i < 1; i++)
    {
        startPositions = movePositions(startPositions, 'U');
        setupS += 'U';
    }

    for (int i = 0; i < 5; i++)
    {
        startPositions = movePositions(startPositions, 'L');
        setupS += 'L';
    }

    // printLabyrinth(startPositions);

    std::string workingPlan = findWorkingPlan();

    if (verbose)
    {
        printLabyrinth(verboseStartState);

        for (auto c : setupS)
        {
            verboseStartState = movePositions(verboseStartState, c);
            printLabyrinth(verboseStartState);
        }

        for (auto c : workingPlan)
        {
            verboseStartState = movePositions(verboseStartState, c);
            printLabyrinth(verboseStartState);
        }
    }

    out << (setupS + workingPlan).c_str();
    in.close();
    out.close();
    return 0;
}