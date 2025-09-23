#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <cstdint>
#include <tuple>

// Apparently x and y are switched from their default configuration
std::vector<std::vector<unsigned char>> rows;
std::vector<std::vector<unsigned char>> cols;

void printLine(std::vector<bool> lineSpec, unsigned char lineSize)
{
    for (unsigned char i = 0; i < lineSize; i++)
    {
        printf("%c", lineSpec[i] ? '#' : '.');
    }
    printf("\n");
}

void printLineToFile(std::ofstream *out, std::vector<bool> lineSpec)
{
    for (unsigned char i = 0; i < lineSpec.size(); i++)
    {
        (*out) << (lineSpec[i] ? '#' : '.');
    }
    (*out) << '\n';
}

void printImage(bool **image, unsigned char rowSize, unsigned char colSize)
{
    for (unsigned char i = 0; i < rowSize; i++)
    {
        for (unsigned char j = 0; j < colSize; j++)
        {
            printf("%c", image[i][j] ? '#' : '.');
        }
        printf("\n");
    }
}

void printToFile(std::ofstream *out, bool **image, unsigned char rowSize, unsigned char colSize)
{
    for (unsigned char i = 0; i < rowSize; i++)
    {
        for (unsigned char j = 0; j < colSize; j++)
        {
            (*out) << (image[i][j] ? '#' : '.');
        }
        (*out) << '\n';
    }
}

std::vector<std::vector<bool>> genRowDoms(unsigned char rowSize, char rowNum, unsigned int currBlock, unsigned int currPos)
{
    const auto &runs = rows[rowNum];
    unsigned int k = runs.size();

    std::vector<std::vector<bool>> assignments;

    unsigned int minRem = 0;
    for (unsigned int b = currBlock + 1; b < k; ++b)
    {
        minRem += runs[b];
    }
    minRem += (k > currBlock + 1 ? (k - currBlock - 1) : 0);

    unsigned int blockLen = runs[currBlock];
    for (unsigned int start = currPos; start + blockLen + minRem <= rowSize; ++start)
    {
        std::vector<bool> base(rowSize, false);
        for (unsigned int i = start; i < start + blockLen; ++i)
        {
            base[i] = true;
        }

        if (currBlock + 1 == k)
        {
            assignments.push_back(base);
        }
        else // Next blocks
        {
            unsigned int nextPos = start + blockLen + 1;
            auto suffixes = genRowDoms(rowSize, rowNum, currBlock + 1, nextPos);

            // Add with or
            for (auto &suf : suffixes)
            {
                std::vector<bool> combined(rowSize);
                for (unsigned int i = 0; i < rowSize; ++i)
                    combined[i] = base[i] || suf[i];
                assignments.push_back(std::move(combined));
            }
        }
    }

    return assignments;
}

std::vector<std::vector<bool>> genColDoms(unsigned char colSize, unsigned char colNum, unsigned int currBlock, unsigned int currPos)
{
    const auto &runs = cols[colNum];
    unsigned int k = runs.size();

    std::vector<std::vector<bool>> assignments;

    unsigned int minRem = 0;
    for (unsigned int b = currBlock + 1; b < k; ++b)
    {
        minRem += runs[b];
    }
    if (k > currBlock + 1)
    {
        minRem += (k - currBlock - 1);
    }

    unsigned int blockLen = runs[currBlock];
    for (unsigned int start = currPos;
         start + blockLen + minRem <= colSize;
         ++start)
    {
        std::vector<bool> base(colSize, false);
        for (unsigned int i = start; i < start + blockLen; ++i)
        {
            base[i] = true;
        }

        if (currBlock + 1 == k)
        {
            assignments.push_back(base);
        }
        else // Next blocks
        {
            unsigned int nextPos = start + blockLen + 1;
            auto suffixes = genColDoms(colSize, colNum, currBlock + 1, nextPos);

            // Add with or
            for (auto &suf : suffixes)
            {
                std::vector<bool> combined(colSize);
                for (unsigned int i = 0; i < colSize; ++i)
                {
                    combined[i] = base[i] || suf[i];
                }
                assignments.push_back(std::move(combined));
            }
        }
    }

    return assignments;
}

void infer(std::vector<std::vector<std::vector<bool>>> &possRows, std::vector<std::vector<std::vector<bool>>> &possCols)
{
    // Determining which positions need to be colored
    for (unsigned char i = 0; i < rows.size(); i++)
    {
        std::vector<bool> fixCols(cols.size(), true);

        for (unsigned int j = 0; j < cols.size(); j++)
        {
            for (unsigned int l = 0; l < possRows[i].size(); l++)
            {
                if (!possRows[i][l][j])
                {
                    fixCols[j] = false;
                    break;
                }
            }
        }

        for (unsigned int k = 0; k < cols.size(); k++)
        {
            if (fixCols[k])
            {
                // Check if there are any valid options left after deletion
                bool hasValid = false;
                for (auto &p : possCols[k])
                {
                    if (p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = possCols[k].size() - 1; l >= 0; l--)
                    {
                        if (!possCols[k][l][i])
                            possCols[k].erase(possCols[k].begin() + l);
                    }
                }
            }
        }
    }

    for (unsigned char i = 0; i < cols.size(); i++)
    {
        std::vector<bool> fixRows(rows.size(), true);

        for (unsigned int j = 0; j < rows.size(); j++)
        {
            for (unsigned int l = 0; l < possCols[i].size(); l++)
            {
                if (!possCols[i][l][j])
                {
                    fixRows[j] = false;
                    break;
                }
            }
        }

        for (unsigned int k = 0; k < rows.size(); k++)
        {
            if (fixRows[k])
            {
                bool hasValid = false;
                for (auto &p : possRows[k])
                {
                    if (p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = possRows[k].size() - 1; l >= 0; l--)
                    {
                        if (!possRows[k][l][i])
                            possRows[k].erase(possRows[k].begin() + l);
                    }
                }
            }
        }
    }

    // Determining which positions cannot be colored
    for (unsigned char i = 0; i < rows.size(); i++)
    {
        std::vector<bool> fixCols(cols.size(), true);

        for (unsigned int j = 0; j < cols.size(); j++)
        {
            for (unsigned int l = 0; l < possRows[i].size(); l++)
            {
                if (possRows[i][l][j])
                {
                    fixCols[j] = false;
                    break;
                }
            }
        }

        for (unsigned int k = 0; k < cols.size(); k++)
        {
            if (fixCols[k])
            {
                // Check if there are any columns with false here
                bool hasValid = false;
                for (auto &p : possCols[k])
                {
                    if (!p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = possCols[k].size() - 1; l >= 0; l--)
                    {
                        if (possCols[k][l][i])
                            possCols[k].erase(possCols[k].begin() + l);
                    }
                }
            }
        }
    }

    // Now the analogous part for columns
    for (unsigned char i = 0; i < cols.size(); i++)
    {
        std::vector<bool> fixRows(rows.size(), true);

        for (unsigned int j = 0; j < rows.size(); j++)
        {
            for (unsigned int l = 0; l < possCols[i].size(); l++)
            {
                if (possCols[i][l][j])
                {
                    fixRows[j] = false;
                    break;
                }
            }
        }

        for (unsigned int k = 0; k < rows.size(); k++)
        {
            if (fixRows[k])
            {
                bool hasValid = false;
                for (auto &p : possRows[k])
                {
                    if (!p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = possRows[k].size() - 1; l >= 0; l--)
                    {
                        if (possRows[k][l][i])
                            possRows[k].erase(possRows[k].begin() + l);
                    }
                }
            }
        }
    }
}

// Hash for unordered_set
namespace std
{
    template <>
    struct hash<std::tuple<unsigned int, unsigned int, bool>>
    {
        size_t operator()(const std::tuple<unsigned int, unsigned int, bool> &t) const
        {
            return std::get<0>(t) ^ (std::get<1>(t) << 1) ^ (std::get<2>(t) << 2);
        }
    };
}

// A normal arc consistency algorithm, used as a base in an overaching backtracking algorithm
void ac3(std::vector<std::vector<std::vector<bool>>> &possRows, std::vector<std::vector<std::vector<bool>>> &possCols)
{
    // If every domain (possRows[i].size() == 1) we have an unambiguous assignments and a solution
    std::queue<std::tuple<unsigned int, unsigned int, bool>> domQ;
    std::unordered_set<std::tuple<unsigned int, unsigned int, bool>> inQueue;

    for (unsigned int i = 0; i < rows.size(); i++)
    {
        for (unsigned int j = 0; j < cols.size(); j++)
        {
            auto arc1 = std::make_tuple(i, j, true);
            if (inQueue.insert(arc1).second)
                domQ.push(arc1);

            auto arc2 = std::make_tuple(j, i, false);
            if (inQueue.insert(arc2).second)
                domQ.push(arc2);
        }
    }

    while (!domQ.empty())
    {
        auto arc = domQ.front();
        domQ.pop();
        inQueue.erase(arc); // Deleted, because we are processing

        unsigned int fstNum = std::get<0>(arc);
        unsigned int sndNum = std::get<1>(arc);
        bool ifRow = std::get<2>(arc);

        // Remove all elements that do not have a valid counterpart
        // If something has been deleted, add back all the lines of the second axis
        if (ifRow)
        {
            for (int i = possRows[fstNum].size() - 1; i >= 0; i--)
            {
                bool fit1 = false;
                for (unsigned int j = 0; j < possCols[sndNum].size(); j++)
                {
                    if (possRows[fstNum][i][sndNum] == possCols[sndNum][j][fstNum])
                    {
                        fit1 = true;
                        break;
                    }
                }

                if (!fit1)
                {
                    possRows[fstNum].erase(possRows[fstNum].begin() + i);

                    for (unsigned int j = 0; j < cols.size(); j++)
                    {
                        if (j != sndNum)
                        {
                            auto arc = std::make_tuple(j, fstNum, false);
                            if (inQueue.insert(arc).second)
                            {
                                domQ.push(arc); // Added only if wasn't processed earlier
                            }
                        }
                    }
                }
            }
        }
        else
        {
            for (int i = possCols[fstNum].size() - 1; i >= 0; i--)
            {
                bool fit1 = false;
                for (unsigned int j = 0; j < possRows[sndNum].size(); j++)
                {
                    if (possCols[fstNum][i][sndNum] == possRows[sndNum][j][fstNum])
                    {
                        fit1 = true;
                        break;
                    }
                }

                if (!fit1)
                {
                    possCols[fstNum].erase(possCols[fstNum].begin() + i);
                    for (unsigned int j = 0; j < rows.size(); j++)
                    {
                        if (j != sndNum)
                        {
                            auto arc = std::make_tuple(j, fstNum, true);
                            if (inQueue.insert(arc).second)
                            {
                                domQ.push(arc); // Added only if wasn't processed earlier
                            }
                        }
                    }
                }
            }
        }
    }
}

// A normal arc consistency algorithm, used for checking a specific line
void ac3(std::vector<std::vector<std::vector<bool>>> &possRows, std::vector<std::vector<std::vector<bool>>> &possCols, unsigned char lineToCheck, bool isRow)
{
    // If every domain (possRows[i].size() == 1) we have an unambiguous assignments and a solution
    std::queue<std::pair<unsigned int, unsigned int>> domQ;
    std::queue<bool> ifRowQ; // True if left domain is a row

    if (isRow)
    {
        for (unsigned int j = 0; j < cols.size(); j++)
        {
            domQ.push({lineToCheck, j});
            ifRowQ.push(true);
        }
    }
    else
    {
        for (unsigned int j = 0; j < rows.size(); j++)
        {
            domQ.push({lineToCheck, j});
            ifRowQ.push(false);
        }
    }

    while (!domQ.empty())
    {
        auto [fstNum, sndNum] = domQ.front();
        bool ifRow = ifRowQ.front();
        domQ.pop();
        ifRowQ.pop();

        // Remove all elements that do not have a valid counterpart
        // If something has been deleted, add back all the lines of the second axis
        if (ifRow)
        {
            bool fit1 = false;
            for (unsigned int j = 0; j < possCols[sndNum].size(); j++)
            {
                if (possRows[fstNum][0][sndNum] == possCols[sndNum][j][fstNum])
                {
                    fit1 = true;
                    break;
                }
            }

            if (!fit1)
            {
                possRows[fstNum].erase(possRows[fstNum].begin());

                return;
            }
        }
        else
        {
            bool fit1 = false;
            for (unsigned int j = 0; j < possRows[sndNum].size(); j++)
            {
                if (possCols[fstNum][0][sndNum] == possRows[sndNum][j][fstNum])
                {
                    fit1 = true;
                    break;
                }
            }

            if (!fit1)
            {
                possCols[fstNum].erase(possCols[fstNum].begin());
                return;
            }
        }
    }
}

bool checkSolution(std::vector<std::vector<std::vector<bool>>> &possRows, std::vector<std::vector<std::vector<bool>>> &possCols)
{
    for (unsigned int k = 0; k < rows.size(); k++)
    {
        if (possRows[k].size() != 1)
        {
            return false;
        }
    }
    for (unsigned int k = 0; k < cols.size(); k++)
    {
        if (possCols[k].size() != 1)
        {
            return false;
        }
    }

    return true;
}

bool backTrack(std::vector<std::vector<std::vector<bool>>> &possRows, std::vector<std::vector<std::vector<bool>>> &possCols)
{
    ac3(possRows, possCols);

    for (auto &dom : possRows)
        if (dom.empty())
            return false;

    for (auto &dom : possCols)
        if (dom.empty())
            return false;

    if (checkSolution(possRows, possCols))
        return true;

    // Finding MRV – minimum remaining values
    bool isMRVRow;
    unsigned char lineNumMRV;
    unsigned int MRVSize = UINT16_MAX;
    bool solutionFound;

    for (unsigned int k = 0; k < rows.size(); k++)
    {
        if (possRows[k].size() < MRVSize && possRows[k].size() > 1)
        {
            isMRVRow = true;
            lineNumMRV = k;
            MRVSize = possRows[k].size();
        }
    }

    for (unsigned int k = 0; k < cols.size(); k++)
    {
        if (possCols[k].size() < MRVSize && possCols[k].size() > 1)
        {
            isMRVRow = false;
            lineNumMRV = k;
            MRVSize = possCols[k].size();
        }
    }

    // printf("MRV: %s %d , size: %d\n", isMRVRow ? "Row" : "Col", lineNumMRV, MRVSize);

    if (isMRVRow)
    {
        auto backupRows = possRows;
        auto backupCols = possCols;

        auto domain = possRows[lineNumMRV];

        for (auto const &pattern : domain)
        {
            possRows = backupRows;
            possCols = backupCols;

            possRows[lineNumMRV].clear();
            possRows[lineNumMRV].push_back(pattern);

            infer(possRows, possCols);

            ac3(possRows, possCols, lineNumMRV, isMRVRow);

            bool empty = false;
            for (auto &dom : possRows)
                if (dom.empty())
                    empty = true;

            for (auto &dom : possCols)
                if (dom.empty())
                    empty = true;

            if (!empty && backTrack(possRows, possCols))
                return true;
        }
    }
    else
    {
        auto backupRows = possRows;
        auto backupCols = possCols;

        auto domain = possCols[lineNumMRV];

        for (auto const &pattern : domain)
        {
            possRows = backupRows;
            possCols = backupCols;

            possCols[lineNumMRV].clear();
            possCols[lineNumMRV].push_back(pattern);

            infer(possRows, possCols);

            ac3(possRows, possCols, lineNumMRV, isMRVRow);

            bool empty = false;
            for (auto &dom : possRows)
                if (dom.empty())
                    empty = true;

            for (auto &dom : possCols)
                if (dom.empty())
                    empty = true;

            if (!empty && backTrack(possRows, possCols))
                return true;
        }
    }

    return false;
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
    }

    std::string xString;
    std::string yString;

    unsigned char xSize;
    unsigned char ySize;

    in >> xString;
    in >> yString;
    in >> std::ws;

    xSize = stoi(xString);
    ySize = stoi(yString);

    std::string inS;

    for (int i = 0; i < xSize; i++)
    {
        std::getline(in, inS);
        std::stringstream ss(inS);
        int num;
        rows.push_back({});

        while (ss >> num)
        {
            rows[i].push_back(num);
        }
    }

    for (int i = 0; i < ySize; i++)
    {
        std::getline(in, inS);
        std::stringstream ss(inS);
        int num;
        cols.push_back({});

        while (ss >> num)
        {
            cols[i].push_back(num);
        }

        if (in.eof())
            break;
    }

    // First vector for number of lines, second for possibilities, third for elements of line
    std::vector<std::vector<std::vector<bool>>> rowPossibilities;
    std::vector<std::vector<std::vector<bool>>> colPossibilities;

    for (unsigned char i = 0; i < rows.size(); ++i)
    {
        rowPossibilities.push_back(genRowDoms(cols.size(), i, 0, 0));
    }

    for (unsigned char j = 0; j < cols.size(); ++j)
    {
        colPossibilities.push_back(genColDoms(rows.size(), j, 0, 0));
    }

    infer(rowPossibilities, colPossibilities);

    bool solutionFound = backTrack(rowPossibilities, colPossibilities);

    if (!solutionFound)
    {
        printf("No nonogram solution found!\n");
    }
    else
    {
        for (unsigned int i = 0; i < rows.size(); i++)
        {
            printLineToFile(&out, rowPossibilities[i][0]);
        }
    }
}
