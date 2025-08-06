#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>

// WORKS FOR MOST OF THE TESTS, A BETTER PROGRAM FOR THIS EXIST IN LAB 3
std::random_device rd;
auto seed = rd() ^ (std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);
std::uniform_int_distribution<int> rowDist;
std::uniform_int_distribution<int> colDist;
std::uniform_int_distribution<int> exitDist(0, 3000);
constexpr int exitNum = 3;

// Apparently x and y are switched from their default configuration
std::vector<std::vector<unsigned char>> rows;
std::vector<std::vector<unsigned char>> cols;
std::vector<char> rowCorrect;
std::vector<char> colCorrect;
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

// Determining which spots have to be a certain color
void processSures(bool **image)
{
    int height = rows.size();
    int width = cols.size();

    // Analyzing rows
    for (int y = 0; y < height; y++)
    {
        const auto &blocks = rows[y];
        int total_black = 0;
        for (unsigned char b : blocks)
            total_black += b;
        int min_space = total_black + (blocks.size() - 1);
        int free_space = width - min_space;

        std::vector<int> left(width, 0), right(width, 0);

        int pos = 0;
        for (unsigned char b : blocks)
        {
            for (int i = 0; i < b; i++)
                left[pos + i] = 1;
            pos += b + 1;
        }

        pos = width - 1;
        for (int i = blocks.size() - 1; i >= 0; i--)
        {
            unsigned char b = blocks[i];
            for (int j = 0; j < b; j++)
                right[pos - j] = 1;
            pos -= b + 1;
        }

        for (int x = 0; x < width; x++)
        {
            if (left[x] == 1 && right[x] == 1)
            {
                sureBlacks[y][x] = true;
                image[y][x] = true;
            }
            else if (left[x] == 0 && right[x] == 0)
            {
                sureWhites[y][x] = true;
                image[y][x] = false;
            }
        }
    }

    // Analyzing cols
    for (int x = 0; x < width; x++)
    {
        const auto &blocks = cols[x];
        int total_black = 0;
        for (unsigned char b : blocks)
            total_black += b;
        int min_space = total_black + (blocks.size() - 1);
        int free_space = height - min_space;

        std::vector<int> top(height, 0), bottom(height, 0);

        int pos = 0;
        for (unsigned char b : blocks)
        {
            for (int i = 0; i < b; i++)
                top[pos + i] = 1;
            pos += b + 1;
        }

        pos = height - 1;
        for (int i = blocks.size() - 1; i >= 0; i--)
        {
            unsigned char b = blocks[i];
            for (int j = 0; j < b; j++)
                bottom[pos - j] = 1;
            pos -= b + 1;
        }

        for (int y = 0; y < height; y++)
        {
            if (top[y] == 1 && bottom[y] == 1)
            {
                sureBlacks[y][x] = true;
                image[y][x] = true;
            }
            else if (top[y] == 0 && bottom[y] == 0)
            {
                sureWhites[y][x] = true;
                image[y][x] = false;
            }
        }
    }
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
char verifyRow(bool **image, const unsigned char &rowSize, const unsigned char &rowNum)
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
            if (image[rowNum][i] == 1)
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
        if (currPos - 1 < cols.size() && image[rowNum][currPos - 1] == 0)
            zeros += 1;

        if (numOnes < blockNum)
        {
            return -1; // Too little
        }

        // If there is no break after a correct non-final block
        if (currPos - 1 < cols.size() && image[rowNum][currPos - 1] == 1 && blockId != rows[rowNum].size() - 1)
        {
            return 1;
        }

        blockId += 1;
    }

    for (int i = currPos - 1; i < cols.size(); i++)
    {
        if (image[rowNum][i] == 1)
            return 1; // Too many
    }

    // If all blocks are correct
    return 0;
}

char verifyCol(bool **image, const unsigned char &colSize, const unsigned char &colNum)
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
            if (image[i][colNum] == 1)
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

        if (currPos - 1 < rows.size() && image[currPos - 1][colNum] == 0)
            zeros += 1;

        if (numOnes < blockNum)
        {
            return -1; // Too little
        }

        // If there is no break after a correct non-final block
        if (currPos - 1 < rows.size() && image[currPos - 1][colNum] == 1 && blockId != cols[colNum].size() - 1)
        {
            return 1;
        }

        blockId += 1;
    }

    for (int i = currPos - 1; i < rows.size(); i++)
    {
        if (image[i][colNum] == 1)
        {
            return 1; // Too many
        }
    }

    // If all blocks are correct
    return 0;
}

void stepRow(bool **image, const unsigned char &rowSize, const unsigned char &rowNum)
{

    char rowState = verifyRow(image, rowSize, rowNum);
    if (rowState == 0) // Sometimes switch unoptimally to escape local minima
    {
        rowCorrect[rowNum] = 1;
        return;
    }

    bool allCorrect = 1;

    for (int i = 0; i < colCorrect.size(); i++)
    {
        allCorrect = allCorrect && (bool)colCorrect[i];
    }

    if (rowState == 1)
    {
        while (true)
        {
            unsigned char changeSpot = colDist(gen);
            if ((allCorrect || !(bool)colCorrect[changeSpot]) && !sureBlacks[rowNum][changeSpot])
                if (image[rowNum][changeSpot] == 1)
                {
                    image[rowNum][changeSpot] = 0;
                    colCorrect[changeSpot] = !(bool)verifyCol(image, rows.size(), changeSpot);
                    break;
                }
        }
    }
    else if (rowState == -1)
    {
        while (true)
        {
            unsigned char changeSpot = colDist(gen);
            if ((allCorrect || !(bool)colCorrect[changeSpot]) && !sureWhites[rowNum][changeSpot])
                if (image[rowNum][changeSpot] == 0)
                {
                    image[rowNum][changeSpot] = 1;
                    colCorrect[changeSpot] = !(bool)verifyCol(image, rows.size(), changeSpot);

                    break;
                }
        }
    }

    rowState = verifyRow(image, rowSize, rowNum);
    if (rowState == 0)
    {
        rowCorrect[rowNum] = 1;
    }
    else
    {
        rowCorrect[rowNum] = 0;
    }
}

void stepCol(bool **image, const unsigned char &colSize, const unsigned char &colNum)
{
    char colState = verifyCol(image, colSize, colNum);
    if (colState == 0)
    {
        colCorrect[colNum] = 1;
        return;
    }

    bool allCorrect = 1;

    for (int i = 0; i < rowCorrect.size(); i++)
    {
        allCorrect = allCorrect && (bool)rowCorrect[i];
    }
    if (colState == 1)
    {
        while (true)
        {
            unsigned char changeSpot = rowDist(gen);
            if ((allCorrect || !(bool)rowCorrect[changeSpot]) && !sureBlacks[changeSpot][colNum])
                if (image[changeSpot][colNum] == 1)
                {
                    image[changeSpot][colNum] = 0;
                    rowCorrect[changeSpot] = !(bool)verifyRow(image, cols.size(), changeSpot);
                    break;
                }
        }
    }
    else if (colState == -1)
    {

        while (true)
        {
            unsigned char changeSpot = rowDist(gen);
            if ((allCorrect || !(bool)rowCorrect[changeSpot]) && !sureWhites[changeSpot][colNum])
                if (image[changeSpot][colNum] == 0)
                {
                    image[changeSpot][colNum] = 1;
                    rowCorrect[changeSpot] = !(bool)verifyRow(image, cols.size(), changeSpot);
                    break;
                }
        }
    }

    colState = verifyCol(image, colSize, colNum);
    if (colState == 0)
    {
        colCorrect[colNum] = 1;
    }
    else
    {
        colCorrect[colNum] = 0;
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
        unsigned char num;
        rows.push_back({});
        rowCorrect.push_back(0);

        while (ss >> num)
        {
            rows[i].push_back(num - '0');
        }
    }

    for (int i = 0; i < ySize; i++)
    {
        std::getline(in, inS);
        std::stringstream ss(inS);
        unsigned char num;

        cols.push_back({});
        colCorrect.push_back(0);
        while (ss >> num)
        {
            cols[i].push_back(num - '0');
        }

        if (in.eof())
            break;
    }

    rowDist = std::uniform_int_distribution<int>(0, rows.size() - 1);
    colDist = std::uniform_int_distribution<int>(0, cols.size() - 1);

    // Initialize and determine sures
    bool **image = genImage(rows.size(), cols.size(), 1);
    sureBlacks = genImage(rows.size(), cols.size(), 0);
    sureWhites = genImage(rows.size(), cols.size(), 0);
    processSures(image);

    unsigned long long counter = 0;

    bool cool = false;

    // While not all rows or all columns are correct
    while (!cool)
    {
        // Randomly choosing row or column
        if (colDist(gen) % 2)
        {
            // Row
            unsigned char toCheck = rowDist(gen);
            if (rowCorrect[toCheck])
                continue;

            stepRow(image, rows.size(), toCheck);
        }
        else
        {
            // Col
            unsigned char toCheck = colDist(gen);
            if (colCorrect[toCheck])
                continue;

            stepCol(image, cols.size(), toCheck);
        }

        unsigned short correctnessCounter = 0;

        for (unsigned char i = 0; i < rows.size(); i++)
        {
            if (rowCorrect[i] == 1)
                correctnessCounter += 1;
        }

        for (unsigned char i = 0; i < cols.size(); i++)
        {
            if (colCorrect[i] == 1)
            {
                correctnessCounter += 1;
            }
        }

        if (correctnessCounter == rows.size() + cols.size())
        {
            printToFile(&out, image, rows.size(), cols.size());
            printImage(image, rows.size(), cols.size());
            cool = true;
        }

        counter += 1;

        // Reshuffle every some steps
        if (counter % 30000000001 == 0)
        {
            printf("Reshuffling board...\n");

            // Last check
            if (correctnessCounter == rows.size() + cols.size())
            {
                printToFile(&out, image, rows.size(), cols.size());
                printImage(image, rows.size(), cols.size());
                cool = true;
            }
            else // Randomize the board
            {
                printImage(image, rows.size(), cols.size());
                for (int i = 0; i < rows.size() * cols.size(); i++)
                {
                    image[i / cols.size()][i % rows.size()] = (bool)(rowDist(gen) % 2);
                    rowCorrect[i / cols.size()] = 0;
                    colCorrect[i / rows.size()] = 0;
                }
            }
        }
    }

    for (int i = 0; i < rows.size(); i++)
    {
        free(image[i]);
    }

    in.close();
    out.close();
    return 0;
}