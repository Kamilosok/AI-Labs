#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <queue>

std::random_device rd;
auto seed = rd() ^ (std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);

// Apparently x and y are switched from their default configuration
std::vector<std::vector<unsigned char>> rows;
std::vector<std::vector<unsigned char>> cols;
bool **sureBlacks;
bool **sureWhites;

// Initial image generation
bool **genImage(unsigned char rowSize, unsigned char colSize, bool random)
{
    std::uniform_int_distribution<int> dist(0, 1);

    // 1. Allocate memory for the array of row pointers
    bool **image = (bool **)malloc(rowSize * sizeof(bool *));
    if (!image)
    {
        throw std::bad_alloc();
    }

    // 2. Allocate memory for each individual row
    for (int i = 0; i < rowSize; i++)
    {
        image[i] = (bool *)malloc(colSize * sizeof(bool));
        if (!image[i])
        {
            throw std::bad_alloc();
        }
    }

    // 3. Initialize (set) the cell values
    for (int i = 0; i < rowSize * colSize; i++)
    {
        image[i % rowSize][i / colSize] = random ? (bool)(dist(gen)) : 0;
    }

    return image;
}

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

// Verify if the entire row is correct
char verifyRow(const std::vector<bool> &rowSpec, const unsigned char &rowSize, const unsigned char &rowNum)
{
    unsigned char currPos = 0;
    unsigned char blockId = 0;
    unsigned char zeros = 0;
    for (unsigned char blockNum : rows[rowNum])
    {

        unsigned int firstOne = INT16_MAX;
        unsigned int numOnes = 0;

        // Not enough blocks
        if (currPos - 1 + blockNum > rowSize)
        {
            if (zeros >= blockId)
                return -1;
            else
                return 1;
        }

        for (unsigned int i = currPos; i < cols.size(); i++)
        {
            if (rowSpec[i] == 1)
            {
                numOnes += 1;
                if (firstOne == INT16_MAX)
                    firstOne = i;
            }

            if (i == firstOne + blockNum - 1)
            {
                break;
            }
        }

        if (firstOne == INT16_MAX)
            if (blockNum > 0)
                return -1;
            else
                return 0;

        currPos = firstOne + blockNum + 1;
        if (currPos - 1 < cols.size() && rowSpec[currPos - 1] == 0)
            zeros += 1;

        if (numOnes < blockNum)
        {
            return -1; // Too little
        }

        // If there is no break after a correct non-final block
        if (currPos - 1 < cols.size() && rowSpec[currPos - 1] == 1 && blockId != rows[rowNum].size() - 1)
        {
            return 1;
        }

        blockId += 1;
    }

    for (int i = currPos - 1; i < cols.size(); i++)
    {
        if (rowSpec[i] == 1)
            return 1; // Too many
    }

    // If all blocks are correct
    return 0;
}

char verifyCol(const std::vector<bool> &colSpec, const unsigned char &colSize, const unsigned char &colNum)
{
    unsigned char currPos = 0;
    unsigned char blockId = 0;
    unsigned char zeros = 0;
    for (unsigned char blockNum : cols[colNum])
    {
        unsigned int firstOne = INT16_MAX;
        unsigned int numOnes = 0;

        // Not enough blocks
        if (currPos - 1 + blockNum > colSize)
        {
            if (zeros >= blockId)
                return -1;
            else
                return 1;
        }

        for (unsigned int i = currPos; i < rows.size(); i++)
        {
            if (colSpec[i] == 1)
            {
                numOnes += 1;
                if (firstOne == INT16_MAX)
                    firstOne = i;
            }

            if (i == firstOne + blockNum - 1)
            {
                break;
            }
        }

        if (firstOne == INT16_MAX)
            if (blockNum > 0)
                return -1;
            else
                return 0;

        currPos = firstOne + blockNum + 1;

        if (currPos - 1 < rows.size() && colSpec[currPos - 1] == 0)
            zeros += 1;

        if (numOnes < blockNum)
        {
            return -1; // Too little
        }

        // If there is no break after a correct non-final block
        if (currPos - 1 < rows.size() && colSpec[currPos - 1] == 1 && blockId != cols[colNum].size() - 1)
        {
            return 1;
        }

        blockId += 1;
    }

    for (int i = currPos - 1; i < rows.size(); i++)
    {
        if (colSpec[i] == 1)
        {
            return 1; // Too many
        }
    }

    // If all blocks are correct
    return 0;
}

std::vector<std::vector<bool>> genRowDoms(const unsigned char &rowSize, const char &rowNum)
{
    std::vector<std::vector<bool>> assignments;
    unsigned int total = 1 << rowSize; // 2^(n-1) possibilities

    // Every number < total is a possibility
    for (unsigned int num = 0; num < total; num++)
    {
        std::vector<bool> current(rowSize);
        for (unsigned int i = 0; i < rowSize; i++)
        {
            current[i] = (num >> i) & 1u;
        }

        if (verifyRow(current, rowSize, rowNum) == 0)
            assignments.push_back(current);
    }

    return assignments;
}

std::vector<std::vector<bool>> genColDoms(const unsigned char &colSize, const char &colNum)
{
    std::vector<std::vector<bool>> assignments;
    unsigned int total = 1 << colSize; // 2^(n-1) possibilities

    // Every number < total is a possibility
    for (unsigned int num = 0; num < total; num++)
    {
        std::vector<bool> current(colSize);
        for (unsigned int i = 0; i < colSize; i++)
        {
            current[i] = (num >> i) & 1u;
        }

        if (verifyCol(current, colSize, colNum) == 0)
            assignments.push_back(current);
    }

    return assignments;
}

// A normal arc consistency algorithm
void ac3(std::vector<std::vector<std::vector<bool>>> *possRows, std::vector<std::vector<std::vector<bool>>> *possCols)
{
    // If every domain (possRows[i].size() == 1) we have an unambiguous assignments and a solution
    std::queue<std::pair<unsigned int, unsigned int>> domQ;
    std::queue<bool> ifRowQ; // True if left domain is a row

    for (unsigned int i = 0; i < rows.size(); i++)
    {
        for (unsigned int j = 0; j < cols.size(); j++)
        {
            domQ.push({i, j});
            ifRowQ.push(true);
            domQ.push({j, i});
            ifRowQ.push(false);
        }
    }

    while (!domQ.empty())
    {
        auto domPair = domQ.front();
        bool ifRow = ifRowQ.front();
        auto fstNum = domPair.first;
        auto sndNum = domPair.second;
        domQ.pop();
        ifRowQ.pop();

        // Remove all elements that do not have a valid counterpart
        // If something has been deleted, add back all the lines of the second axis
        if (ifRow)
        {
            for (int i = (*possRows)[fstNum].size() - 1; i >= 0; i--)
            {
                bool fit1 = false;
                for (unsigned int j = 0; j < (*possCols)[sndNum].size(); j++)
                {
                    if ((*possRows)[fstNum][i][sndNum] == (*possCols)[sndNum][j][fstNum])
                    {
                        fit1 = true;
                        break;
                    }
                }

                if (!fit1)
                {
                    (*possRows)[fstNum].erase((*possRows)[fstNum].begin() + i);

                    for (unsigned int j = 0; j < cols.size(); j++)
                    {
                        if (j != sndNum)
                        {
                            domQ.push({j, fstNum});
                            ifRowQ.push(false);
                        }
                    }
                }
            }
        }
        else
        {
            for (int i = (*possCols)[fstNum].size() - 1; i >= 0; i--)
            {
                bool fit1 = false;
                for (unsigned int j = 0; j < (*possRows)[sndNum].size(); j++)
                {
                    if ((*possCols)[fstNum][i][sndNum] == (*possRows)[sndNum][j][fstNum])
                    {
                        fit1 = true;
                        break;
                    }
                }

                if (!fit1)
                {
                    (*possCols)[fstNum].erase((*possCols)[fstNum].begin() + i);
                    for (unsigned int j = 0; j < rows.size(); j++)
                    {
                        if (j != sndNum)
                        {
                            domQ.push({j, fstNum});
                            ifRowQ.push(true);
                        }
                    }
                }
            }
        }
    }
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

    sureBlacks = genImage(rows.size(), cols.size(), 0);
    sureWhites = genImage(rows.size(), cols.size(), 0);

    // First vector for number of lines, second for possibilities, third for elements of line
    std::vector<std::vector<std::vector<bool>>> rowPossibilities;
    std::vector<std::vector<std::vector<bool>>> colPossibilities;

    for (unsigned char i = 0; i < rows.size(); ++i)
    {
        rowPossibilities.push_back(genRowDoms(cols.size(), i));
    }

    for (unsigned char j = 0; j < cols.size(); ++j)
    {
        colPossibilities.push_back(genColDoms(rows.size(), j));
    }

    // Determining which areas need to be colored in
    for (unsigned char i = 0; i < rows.size(); i++)
    {
        std::vector<bool> fixCols(cols.size(), true);

        for (unsigned int j = 0; j < cols.size(); j++)
        {
            for (unsigned int l = 0; l < rowPossibilities[i].size(); l++)
            {
                if (!rowPossibilities[i][l][j])
                {
                    fixCols[j] = false;
                    break;
                }
            }
            sureBlacks[i][j] = fixCols[j];
        }

        for (unsigned int k = 0; k < cols.size(); k++)
        {
            if (fixCols[k])
            {
                // Check if there are any valid possibilities left after removal
                bool hasValid = false;
                for (auto &p : colPossibilities[k])
                {
                    if (p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = colPossibilities[k].size() - 1; l >= 0; l--)
                    {
                        if (!colPossibilities[k][l][i])
                            colPossibilities[k].erase(colPossibilities[k].begin() + l);
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
            for (unsigned int l = 0; l < colPossibilities[i].size(); l++)
            {
                if (!colPossibilities[i][l][j])
                {
                    fixRows[j] = false;
                    break;
                }
            }
            sureBlacks[j][i] = fixRows[j];
        }

        for (unsigned int k = 0; k < rows.size(); k++)
        {
            if (fixRows[k])
            {
                bool hasValid = false;
                for (auto &p : rowPossibilities[k])
                {
                    if (p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = rowPossibilities[k].size() - 1; l >= 0; l--)
                    {
                        if (!rowPossibilities[k][l][i])
                            rowPossibilities[k].erase(rowPossibilities[k].begin() + l);
                    }
                }
            }
        }
    }

    // Determining which areas cannot be colored
    for (unsigned char i = 0; i < rows.size(); i++)
    {
        std::vector<bool> fixCols(cols.size(), true);

        for (unsigned int j = 0; j < cols.size(); j++)
        {
            for (unsigned int l = 0; l < rowPossibilities[i].size(); l++)
            {
                if (rowPossibilities[i][l][j])
                {
                    fixCols[j] = false;
                    break;
                }
            }
            sureWhites[i][j] = fixCols[j];
        }

        for (unsigned int k = 0; k < cols.size(); k++)
        {
            if (fixCols[k])
            {
                // Check if there are columns with false here
                bool hasValid = false;
                for (auto &p : colPossibilities[k])
                {
                    if (!p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = colPossibilities[k].size() - 1; l >= 0; l--)
                    {
                        if (colPossibilities[k][l][i])
                            colPossibilities[k].erase(colPossibilities[k].begin() + l);
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
            for (unsigned int l = 0; l < colPossibilities[i].size(); l++)
            {
                if (colPossibilities[i][l][j])
                {
                    fixRows[j] = false;
                    break;
                }
            }
            sureWhites[j][i] = fixRows[j];
        }

        for (unsigned int k = 0; k < rows.size(); k++)
        {
            if (fixRows[k])
            {
                bool hasValid = false;
                for (auto &p : rowPossibilities[k])
                {
                    if (!p[i])
                    {
                        hasValid = true;
                        break;
                    }
                }

                if (hasValid)
                {
                    for (int l = rowPossibilities[k].size() - 1; l >= 0; l--)
                    {
                        if (rowPossibilities[k][l][i])
                            rowPossibilities[k].erase(rowPossibilities[k].begin() + l);
                    }
                }
            }
        }
    }

    ac3(&rowPossibilities, &colPossibilities);

    // The only possibilities should remain
    for (unsigned int i = 0; i < rows.size(); i++)
    {
        printLineToFile(&out, rowPossibilities[i][0]);
    }

    for (unsigned int j = 0; j < rows.size(); j++)
    {
        for (unsigned int i = 0; i < rowPossibilities[j].size(); i++)
        {
            printLine(rowPossibilities[j][i], cols.size());
        }
    }

    for (int i = 0; i < rows.size(); i++)
    {
        free(sureBlacks[i]);
        free(sureWhites[i]);
    }
}
