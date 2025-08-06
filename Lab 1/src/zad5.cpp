#include <fstream>
#include <vector>
#include <random>
#include <chrono>

std::random_device rd;
auto seed = rd() ^ (std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);

// Apparently x and y are switched from their default configuration
std::vector<unsigned char> rows;
std::vector<unsigned char> cols;
std::vector<char> rowCorrect;
std::vector<char> colCorrect;

// Initial image generation
bool **genImage(unsigned char rowSize, unsigned char colSize)
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
        image[i / colSize][i % rowSize] = (bool)(dist(gen));
    }

    return image;
}

void printImage(bool **image, unsigned char rowSize, unsigned char colSize)
{
    for (unsigned char i = 0; i < colSize; i++)
    {
        for (unsigned char j = 0; j < rowSize; j++)
        {
            printf("%c", image[i][j] ? '#' : '.');
        }
        printf("\n");
    }
}

void printToFile(std::ofstream *out, bool **image, unsigned char rowSize, unsigned char colSize)
{
    for (unsigned char i = 0; i < colSize; i++)
    {
        for (unsigned char j = 0; j < rowSize; j++)
        {
            (*out) << (image[i][j] ? '#' : '.');
        }
        (*out) << '\n';
    }
}

// Simple one-block check
char verifyRow(bool **image, unsigned char rowSize, unsigned char rowNum)
{
    unsigned int numOnes = 0;
    unsigned int maxOnesInRange = 0;
    unsigned int currOnesInRange = 0;

    if (rows[rowNum] > rowSize)
    {
        // Impossible
        throw std::runtime_error("Data error");
    }
    else
    {
        for (int i = 0; i < rows[rowNum]; i++)
        {
            if (image[rowNum][i] == 1)
            {
                currOnesInRange += 1;
                numOnes += 1;
            }
        }

        maxOnesInRange = currOnesInRange;

        for (int i = rows[rowNum]; i < rowSize; i++)
        {
            if (image[rowNum][i - rows[rowNum]] == 1)
            {
                currOnesInRange -= 1;
            }

            if (image[rowNum][i] == 1)
            {
                currOnesInRange += 1;
                numOnes += 1;
            }

            if (maxOnesInRange < currOnesInRange)
                maxOnesInRange = currOnesInRange;
        }
    }

    if (numOnes == rows[rowNum] && rows[rowNum] == maxOnesInRange)
        return 0; // Valid amount
    else if (numOnes > rows[rowNum])
        return 1; // Too many
    else
        return -1; // Too little or bad arrangement
}

// Simple one-block check
char verifyCol(bool **image, unsigned char colSize, unsigned char colNum)
{
    unsigned int numOnes = 0;
    unsigned int maxOnesInRange = 0;
    unsigned int currOnesInRange = 0;

    if (cols[colNum] > colSize)
    {
        // Impossible
        throw std::runtime_error("Data error");
    }
    else
    {
        for (int i = 0; i < cols[colNum]; i++)
        {
            if (image[i][colNum] == 1)
            {
                currOnesInRange += 1;
                numOnes += 1;
            }
        }

        maxOnesInRange = currOnesInRange;

        for (int i = cols[colNum]; i < colSize; i++)
        {
            if (image[i - cols[colNum]][colNum] == 1)
            {
                currOnesInRange -= 1;
            }

            if (image[i][colNum] == 1)
            {
                currOnesInRange += 1;
                numOnes += 1;
            }

            if (maxOnesInRange < currOnesInRange)
                maxOnesInRange = currOnesInRange;
        }
    }

    // If there are as many as needed and all within range
    if (numOnes == cols[colNum] && cols[colNum] == maxOnesInRange)
        return 0; // Valid amount
    else if (numOnes > cols[colNum])
        return 1; // Too many
    else
        return -1; // Too little or bad arrangement
}

void stepRow(bool **image, unsigned char rowSize, unsigned char rowNum)
{

    char rowState = verifyRow(image, rowSize, rowNum);
    if (rowState == 0) // Sometimes switch unoptimally to escape local minima
    {
        rowCorrect[rowNum] = 1;
        return;
    }

    std::uniform_int_distribution<int> dist(0, rowSize - 1);
    bool allCorrect = 1;

    for (int i = 0; i < colCorrect.size(); i++)
    {
        allCorrect == allCorrect && (bool)colCorrect[i];
    }

    if (rowState == 1)
    {
        while (true)
        {

            unsigned char changeSpot = dist(gen);
            if (allCorrect || !(bool)colCorrect[changeSpot])
                if (image[rowNum][changeSpot] == 1)
                {
                    image[rowNum][changeSpot] = 0;
                    colCorrect[changeSpot] = !(bool)verifyCol(image, rows.size(), changeSpot);
                    break;
                }
        }
    }
    else // rowState == -1
    {
        while (true)
        {
            unsigned char changeSpot = dist(gen);
            if (allCorrect || !(bool)colCorrect[changeSpot])
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

void stepCol(bool **image, unsigned char colSize, unsigned char colNum)
{
    char colState = verifyCol(image, colSize, colNum);
    if (colState == 0)
    {
        colCorrect[colNum] = 1;
        return;
    }

    std::uniform_int_distribution<int> dist(0, colSize - 1);
    bool allCorrect = 1;

    for (int i = 0; i < rowCorrect.size(); i++)
    {
        allCorrect == allCorrect && (bool)rowCorrect[i];
    }

    if (colState == 1)
    {
        while (true)
        {
            unsigned char changeSpot = dist(gen);
            if (allCorrect || !(bool)rowCorrect[changeSpot])
                if (image[changeSpot][colNum] == 1)
                {
                    image[changeSpot][colNum] = 0;
                    rowCorrect[changeSpot] = !(bool)verifyRow(image, cols.size(), changeSpot);
                    break;
                }
        }
    }
    else // rowState == -1
    {
        while (true)
        {
            unsigned char changeSpot = dist(gen);
            if (allCorrect || !(bool)rowCorrect[changeSpot])
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

    unsigned char xSize;
    unsigned char ySize;

    in >> xSize;
    in >> ySize;

    xSize -= '0';
    ySize -= '0';

    for (int i = 0; i < xSize; i++)
    {
        unsigned char a;
        in >> a;
        rows.push_back(a - '0');
        rowCorrect.push_back(0);
    }

    for (int i = 0; i < ySize; i++)
    {
        unsigned char a;
        in >> a;
        cols.push_back(a - '0');
        colCorrect.push_back(0);
    }

    bool **image = genImage(rows.size(), cols.size());

    unsigned long long counter = 0;

    bool cool = false;
    std::uniform_int_distribution<int> rowDist(0, rows.size() - 1);
    std::uniform_int_distribution<int> colDist(0, cols.size() - 1);

    // While not all rows or all columns are correct
    while (!cool)
    {
        // Randomly choosing row or column
        if (rowDist(gen) % 2)
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
                correctnessCounter += 1;
        }

        if (correctnessCounter == rows.size() + cols.size())
        {
            printToFile(&out, image, rows.size(), cols.size());
            printImage(image, rows.size(), cols.size());
            cool = true;
        }

        counter += 1;

        // Reshuffle every some steps
        if (counter % 30000001 == 0)
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