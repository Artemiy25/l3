#ifndef HW_L2_APPLICATION_LAYER_H
#define HW_L2_APPLICATION_LAYER_H

#include <string>
#include <vector>

#include "hw/l3_DomainLayer.h"

class Application
{
public:
    Application(ShipRegistry & registry, SignalLog & signals);

    std::string execute(const std::vector<std::string> & args);

private:
    std::string add(const std::vector<std::string> & args);
    std::string remove(const std::vector<std::string> & args);
    std::string update(const std::vector<std::string> & args);
    std::string view();
    std::string save(const std::vector<std::string> & args);
    std::string load(const std::vector<std::string> & args);

    std::string move(const std::vector<std::string> & args);
    std::string sos(const std::vector<std::string> & args);
    std::string nearest(const std::vector<std::string> & args);
    std::string close(const std::vector<std::string> & args);
    std::string signals();

    ShipRegistry & _registry;
    SignalLog &    _signals;
};

#endif
