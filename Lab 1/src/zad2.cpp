#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>

// All words
std::vector<std::string> lexicon;
std::unordered_map<unsigned int, std::vector<unsigned int>> lensForDepths;

unsigned int sqSum(std::vector<unsigned int> &depths, unsigned int currDepth)
{
    unsigned int sum = 0;
    unsigned int lastChar = depths[0];
    for (int i = 1; i < depths.size(); i++)
    {
        sum += (depths[i - 1] - depths[i]) * (depths[i - 1] - depths[i]);
    }

    sum += (depths[depths.size() - 1] - currDepth) * (depths[depths.size() - 1] - currDepth);

    return sum;
}

void printWSpaces(std::string s, std::vector<unsigned int> spacesSpaces)
{
    for (int i = 0; i < s.size(); i++)
    {
        for (int a : spacesSpaces)
        {
            if (a == i)
                printf(" ");
        }

        printf("%c", s[i]);
    }
    printf("\n");
}

void printToFile(std::ofstream *out, std::string s, std::vector<unsigned int> spacesSpaces)
{
    for (int i = 0; i < s.size(); i++)
    {
        for (int a : spacesSpaces)
        {
            if (a == i)
                (*out) << " ";
        }

        (*out) << s[i];
    }
    (*out) << "\n";
}

std::vector<unsigned int> wordy(const std::string &textLine, unsigned int depth)
{
    // Exit if end of line successfully reached
    if (depth == textLine.length())
        return {};

    // If current depth was already processed then just return the optimal lens
    if (lensForDepths.count(depth))
    {
        return lensForDepths[depth];
    }

    std::vector<std::vector<unsigned int>> choices;

    bool found = false;

    for (int i = 0; i < lexicon.size(); i++)
    {
        if (lexicon[i].size() > textLine.length() - depth || lexicon[i].size() == 0)
            continue;

        // Find all words that fit from the current position onward
        if (strncmp(lexicon[i].c_str(), textLine.c_str() + depth, lexicon[i].size()) == 0)
        {
            found = true;
            auto result = wordy(textLine, depth + lexicon[i].size());

            result.push_back(depth + lexicon[i].size());
            choices.push_back(result);
        }
    }

    // Will automatically be discarded as an option due to having a smaller sqSum
    if (!found)
    {
        return {};
    }

    // Find the choice with the largest sqSum
    int maxPos = 0, maxSum = 0;
    for (size_t i = 0; i < choices.size(); i++)
    {

        int newSum = sqSum(choices[i], depth);
        if (newSum > maxSum)
        {
            maxSum = newSum;
            maxPos = i;
        }
    }

    return lensForDepths[depth] = choices[maxPos];
}

int main(int argc, char *argv[])
{
    unsigned int lines = 0;
    bool inTextGiven = 0, inWordsGiven = 0, outGiven = 0;
    bool debug = 0;

    std::locale loc("pl_PL.utf8");
    std::ifstream inText, inWords;
    std::ofstream out;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (std::strcmp(arg.c_str(), "-d") == 0)
            debug = 1;
        else if (!inTextGiven)
        {
            inText.open(arg, std::fstream::in);

            if (!inText)
            {
                throw std::runtime_error("Cannot open input file!");
            }
            inTextGiven = 1;
        }
        else if (!inWordsGiven)
        {
            inWords.open(arg, std::fstream::in);

            if (!inWords)
            {
                inText.close();
                throw std::runtime_error("Cannot open input file!");
            }
            inWordsGiven = 1;
        }
        else
        {
            out.open(arg, std::fstream::out);
            if (!out)
            {
                throw std::runtime_error("Cannot open output file!");
                inText.close();
                inWords.close();
            }
            outGiven = 1;
        }
    }

    if (!inTextGiven || !inWordsGiven || !outGiven)
    {
        if (inText)
            inText.close();
        if (inWords)
            inWords.close();
        if (out)
            out.close();
        fprintf(stderr, "Both input and output files need to be specified!\n");
    }

    while (true)
    {
        std::string wordLine;
        std::getline(inWords, wordLine);

        if (inWords.eof())
            break;

        lexicon.push_back(wordLine);
    }

    // Only 400 lines for demonstration purposes
    while (lines < 400)
    {
        std::string textLine;
        std::getline(inText, textLine);

        if (inText.eof())
            break;

        std::vector<unsigned int> spacesSpaces = wordy(textLine, 0);
        printToFile(&out, textLine, spacesSpaces);
        printWSpaces(textLine, spacesSpaces);
        lensForDepths.clear();
        lines += 1;
    }

    inText.close();
    inWords.close();
    out.close();
    return 0;
}