#ifndef HW_L4_INFRASTRUCTURE_LAYER_H
#define HW_L4_INFRASTRUCTURE_LAYER_H

#include <string>
#include <vector>

class FileStorage
{
public:
    bool saveLines(const std::string & filename, const std::vector<std::string> & lines);

    bool loadLines(const std::string & filename, std::vector<std::string> & lines);
};

#endif
