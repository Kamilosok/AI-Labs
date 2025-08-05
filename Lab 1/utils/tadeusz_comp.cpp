#include <fstream>
#include <algorithm>
#include <locale>
#include <string>

int main(int argc, char *argv[])
{
    std::locale loc("pl_PL.utf8");
    std::ifstream inMax;
    std::ifstream inRand;
    std::ifstream inTrue;

    unsigned int maxGood = 0, randGood = 0, lines = 0;

    inMax.open("tad_max.txt", std::fstream::in);
    inRand.open("tad_rand.txt", std::fstream::in);
    inTrue.open("data/pan_tadeusz_processed.txt", std::fstream::in);

    while (true)
    {
        std::string maxLine;
        std::string randLine;
        std::string trueLine;
        std::getline(inMax, maxLine);
        std::getline(inRand, randLine);
        std::getline(inTrue, trueLine);

        if (inRand.eof() || inMax.eof())
            break;

        lines += 1;

        if (maxLine == trueLine)
            maxGood += 1;

        if (randLine == trueLine)
            randGood += 1;
    }

    printf("Max: %.2f%\nRand: %.2f%\n", (float)maxGood * 100 / lines, (float)randGood * 100 / lines);

    inMax.close();
    inRand.close();
    inTrue.close();
}