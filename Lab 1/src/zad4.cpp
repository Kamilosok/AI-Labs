#include <fstream>
#include <vector>
#include <string>

// We're walking through windows of D positions and searching for the place with the largest (<=D) number of ones, that's where we will create our block, the rest is set to 0
unsigned int opt_dist(std::vector<unsigned char> nono, unsigned int D)
{
    unsigned int numOnes = 0;

    if (D > nono.size())
    {
        // Impossible
        throw std::runtime_error("Data error");
    }
    else
    {
        unsigned int maxOnesInRange = 0;
        unsigned int currOnesInRange = 0;
        for (int i = 0; i < D; i++)
        {
            if (nono[i] == 1)
            {
                currOnesInRange += 1;
                numOnes += 1;
            }
        }
        maxOnesInRange = currOnesInRange;

        for (int i = D; i < nono.size(); i++)
        {
            if (nono[i - D] == 1)
            {
                currOnesInRange -= 1;
            }

            if (nono[i] == 1)
            {
                currOnesInRange += 1;
                numOnes += 1;
            }

            if (maxOnesInRange < currOnesInRange)
                maxOnesInRange = currOnesInRange;
        }

        return (D - maxOnesInRange) + (numOnes - maxOnesInRange);
    }

    printf("%d\n", numOnes);

    return 0;
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

    while (true)
    {
        // Bool didn’t work here, so we use uchar instead
        std::vector<unsigned char> nono;
        unsigned int D;

        std::string inS;
        std::getline(in, inS);

        if (in.eof())
            break;

        std::string nonoS = inS.substr(0, inS.size() - 2);

        for (char c : nonoS)
        {
            nono.push_back(c - '0');
        }

        D = inS[inS.size() - 1] - '0';

        out << opt_dist(nono, D) << "\n";
    }

    in.close();
    out.close();
    return 0;
}