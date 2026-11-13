#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static int randomNumber(int from, int to)
{
    return from + rand() % (to - from + 1);
}

static int last_signal = 0;

static std::string randomValue(const std::string & placeholder)
{
    const std::vector<std::string> types = {"cargo", "tanker", "cruise"};
    std::ostringstream text;

    if (placeholder == "{ship}")
        text << "s" << randomNumber(0, 49);
    else if (placeholder == "{tonnage}")
        text << randomNumber(1, 100000);
    else if (placeholder == "{type}")
        text << types[randomNumber(0, 2)];
    else if (placeholder == "{coord}")
        text << randomNumber(-100, 100);
    else if (placeholder == "{newsignal}")
        text << ++last_signal;
    else if (placeholder == "{lastsignal}")
        text << last_signal;
    else if (placeholder == "{signal}")
        text << randomNumber(std::max(1, last_signal - 19), std::max(1, last_signal));

    return text.str();
}

static std::string fillTemplate(std::string line)
{
    const std::vector<std::string> placeholders = {"{ship}", "{tonnage}", "{type}", "{coord}", "{newsignal}", "{lastsignal}", "{signal}"};

    for (const std::string & placeholder : placeholders)
    {
        size_t pos = line.find(placeholder);
        while (pos != std::string::npos)
        {
            line.replace(pos, placeholder.size(), randomValue(placeholder));
            pos = line.find(placeholder);
        }
    }
    return line;
}

int main(int argc, char * argv[])
{
    if (argc != 4)
    {
        std::cerr << "Запуск: stressgen <файл шаблонов> <количество команд> <seed>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cerr << "Не удалось открыть файл " << argv[1] << std::endl;
        return 1;
    }

    std::vector<std::string> templates;
    std::string line;
    while (std::getline(file, line))
    {
        if (line != "" && line[0] != '#')
            templates.push_back(line);
    }

    if (templates.empty())
    {
        std::cerr << "В файле нет шаблонов" << std::endl;
        return 1;
    }

    int n_commands = atoi(argv[2]);
    int seed       = atoi(argv[3]);
    srand(seed);

    int n_printed = 0;
    while (n_printed < n_commands)
    {
        int index = randomNumber(0, templates.size() - 1);
        std::string commands = fillTemplate(templates[index]) + " | ";

        size_t pos = commands.find(" | ");
        while (pos != std::string::npos && n_printed < n_commands)
        {
            std::cout << commands.substr(0, pos) << "\n";
            n_printed++;
            commands.erase(0, pos + 3);
            pos = commands.find(" | ");
        }
    }

    return 0;
}
