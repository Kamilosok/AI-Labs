#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <unordered_set>
#include <limits>
#include <unordered_map>

using pos_t = std::pair<char, char>;
using state_t = std::set<pos_t>;

// Helper for pos_t
namespace std
{
    template <>
    struct hash<pos_t>
    {
        std::size_t operator()(const pos_t &p) const
        {
            return (static_cast<size_t>(p.first) << 8) | static_cast<size_t>(p.second);
        }
    };
}

// Helper for state_t
namespace std
{
    template <>
    struct hash<state_t>
    {
        std::size_t operator()(const state_t &s) const
        {
            std::size_t hash_value = 0;
            for (const auto &pos : s)
            {
                hash_value ^= std::hash<pos_t>{}(pos) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
            }
            return hash_value;
        }
    };
}

struct Node
{
    state_t state;
    unsigned int g;
    float f;
    Node(state_t _state, unsigned int _g, float _f) : state(_state), g(_g), f(_f) {};
    bool operator>(const Node &other) const { return f > other.f; }
    bool operator<(const Node &other) const { return f < other.f; }
};

struct CompareNode
{
    bool operator()(const std::pair<Node, std::string> &a, const std::pair<Node, std::string> &b) const
    {
        return a.first.f > b.first.f;
    }
};

state_t startPositions;
std::vector<std::vector<char>> lab;
std::vector<std::vector<unsigned int>> dists;
std::vector<pos_t> goals;
std::unordered_map<state_t, unsigned int> state_map;

// Manhattan or New York distance
unsigned int nyDist(const pos_t &a, const pos_t &b)
{
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

// New York distance to the closest goal, less optimistic heuristic faster
unsigned int heuristic1(const state_t &positions)
{
    float avgMinDist = 0;
    for (auto a : positions)
    {
        unsigned int min = __UINT8_MAX__;
        for (const auto &goal : goals)
        {
            unsigned int currDist = nyDist(a, goal);
            if (min > currDist)
                min = currDist;
        }

        avgMinDist += min;
    }

    return avgMinDist / positions.size();
}

// Maximal minimum heuristic
unsigned int heuristic2(const state_t &positions)
{
    float maxMinDist = 0;
    for (auto a : positions)
    {
        unsigned int min = __UINT8_MAX__;
        for (const auto &goal : goals)
        {
            unsigned int currDist = nyDist(a, goal);
            if (min > currDist)
                min = currDist;
        }

        if (maxMinDist < min)
            maxMinDist = min;
    }

    return maxMinDist;
}

// Maximal real distance
unsigned int heuristic3(const state_t &positions)
{
    float maxDist = 0;
    for (auto a : positions)
    {
        unsigned int newDist = dists[a.first][a.second];
        if (maxDist < newDist)
            maxDist = newDist;
    }

    return maxDist;
}

// Computes the matrix of distances from goals with a simple flooding algorithm
std::vector<std::vector<unsigned int>> computeDistanceMatrix()
{
    int cols = lab.size();
    int rows = lab[0].size();

    std::vector<std::vector<unsigned int>> dist(cols, std::vector<unsigned int>(rows, std::numeric_limits<unsigned int>::max()));

    std::queue<pos_t> q;

    for (const auto &goal : goals)
    {
        dist[goal.first][goal.second] = 0;
        q.push(goal);
    }

    std::vector<pos_t> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!q.empty())
    {

        pos_t curr = q.front();
        q.pop();
        int c = curr.first, r = curr.second;
        int d = dist[c][r];

        for (const auto &dir : directions)
        {
            int nc = c + dir.first;
            int nr = r + dir.second;
            if (lab[nc][nr] != 1)
            {
                if (dist[nc][nr] > d + 1)
                {
                    dist[nc][nr] = d + 1;
                    q.push({nc, nr});
                }
            }
        }
    }

    return dist;
}

// Visualization helper
void printDistanceMatrix()
{
    for (unsigned int i = 0; i < dists.size(); i++)
    {
        for (unsigned int j = 0; j < dists[0].size(); j++)
        {
            printf("%.2d ", dists[i][j] == -1 ? 0 : dists[i][j]);
        }

        printf("\n");
    }
}

// Visualization helper
void printLabyrinth(const state_t &s)
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

// Move all possible positions of a state s in a direction
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

bool isGoalState(const state_t &s)
{
    for (auto pos : s)
    {
        if (lab[pos.first][pos.second] != -1)
            return false;
    }
    return true;
}

// Helper for hashing states
struct state_hash
{
    std::size_t operator()(const state_t &s) const
    {
        std::size_t seed = s.size();
        for (const auto &p : s)
        {
            // Łączymy hash pierwszej i drugiej składowej
            seed ^= std::hash<int>()(p.first) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            seed ^= std::hash<int>()(p.second) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

std::string findSynchronizingWord()
{
    std::priority_queue<std::pair<Node, std::string>, std::vector<std::pair<Node, std::string>>, CompareNode> pq;
    std::unordered_set<state_t, state_hash> visited;

    Node startNode(startPositions, 0, heuristic3(startPositions));
    state_map[startPositions] = 0;

    pq.push({startNode, ""});

    visited.insert(startNode.state);

    std::vector<char> moves = {'U', 'D', 'L', 'R'};

    // A* operating on a priority queue of the heuristic
    while (!pq.empty())
    {
        auto [currNode, path] = pq.top();
        pq.pop();

        // If all possible positions are on goals, return the path taken
        if (isGoalState(currNode.state))
        {
            printLabyrinth(currNode.state);
            return path;
        }

        // Else try to move in all directions without repeating states
        for (char m : moves)
        {
            state_t nextState = movePositions(currNode.state, m);
            if (visited.find(nextState) == visited.end() || state_map[nextState] > currNode.g + 1)
            {
                Node nextNode = Node(nextState, currNode.g + 1, heuristic3(nextState) + currNode.g + 1);
                visited.insert(nextNode.state);
                state_map[nextState] = currNode.g + 1;
                pq.push({nextNode, path + m});
            }
        }
    }
    return "";
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
            // 1 - Wall, -1 - Goal, 0 - Empty
            switch (c)
            {
            case '#':
                lab[currH].push_back(1);
                break;

            case 'G':
                lab[currH].push_back(-1);
                goals.push_back({currH, currW});
                break;

            case 'B':
                lab[currH].push_back(-1);
                startPositions.insert({currH, currW});
                goals.push_back({currH, currW});
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

    std::string pathS = "";

    dists = computeDistanceMatrix();

    // printDistanceMatrix();
    // printLabyrinth(startPositions);

    pathS += findSynchronizingWord();

    out << (pathS).c_str();
    in.close();
    out.close();
    return 0;
}