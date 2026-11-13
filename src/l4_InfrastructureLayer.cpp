#include <fstream>

#include "hw/l4_InfrastructureLayer.h"

bool FileStorage::saveLines(const std::string & filename, const std::vector<std::string> & lines)
{
    std::ofstream file(filename);
    if (!file)
        return false;

    for (const std::string & line : lines)
        file << line << std::endl;

    return true;
}

bool FileStorage::loadLines(const std::string & filename, std::vector<std::string> & lines)
{
    std::ifstream file(filename);
    if (!file)
        return false;

    lines.clear();
    std::string line;
    while (std::getline(file, line))
        lines.push_back(line);

    return true;
}
