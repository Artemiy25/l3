#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "hw/l2_ApplicationLayer.h"

static std::vector<std::string> splitBySpaces(const std::string & line)
{
    std::vector<std::string> words;
    std::istringstream in(line);
    std::string word;

    while (in >> word)
        words.push_back(word);

    return words;
}

int main()
{
    ShipRegistry registry;
    SignalLog    signals;
    Application  app(registry, signals);

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::vector<std::string> args = splitBySpaces(line);

        if (args.empty())
            continue;
        if (args[0] == "exit")
            break;

        std::cout << app.execute(args);
    }

    return 0;
}
