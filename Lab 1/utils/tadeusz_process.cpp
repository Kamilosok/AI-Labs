#include <fstream>
#include <algorithm>
#include <locale>
#include <string>

std::string normalizeText(std::string input)
{
    std::locale loc("pl_PL.UTF-8");
    bool anything = false;

    for (char &c : input)
    {
        c = std::tolower(c, loc);
    }

    std::string output;
    bool space = false;

    for (char c : input)
    {
        if (std::isspace(c, loc))
        {
            if (!space && !output.empty())
            {
                output += ' ';
                space = true;
            }
        }
        else if (std::isalpha(c, loc) || std::string("ąćęłńóśźż").find(c) != std::string::npos)
        {
            output += c;
            anything = true;
            space = false;
        }
    }

    if (!output.empty() && output.back() == ' ')
    {
        output.pop_back();
    }
    if (anything)
        output += '\n';

    return output;
}

int main(int argc, char *argv[])
{
    std::locale loc("pl_PL.utf8");
    std::ifstream in;
    std::ofstream out;

    in.open("data/pan_tadeusz_raw.txt", std::fstream::in);
    out.open("data/pan_tadeusz_processed.txt", std::fstream::out);

    while (true)
    {
        std::string textLine;
        std::getline(in, textLine);

        if (textLine == "\n")
            continue;

        if (in.eof())
            break;

        std::string outer = normalizeText(textLine);
        out << outer;
    }

    in.close();
    out.close();
}