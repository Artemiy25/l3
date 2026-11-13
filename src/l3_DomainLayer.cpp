#include <algorithm>
#include <cassert>
#include <cmath>
#include <sstream>
#include <vector>

#include "hw/l3_DomainLayer.h"
#include "hw/l4_InfrastructureLayer.h"

static bool parseInt(const std::string & text, int & value)
{
    std::istringstream in(text);
    in >> value;
    return !in.fail() && in.eof();
}

bool parseShipType(const std::string & code, ShipType & type)
{
    if (code == "cargo")
        type = ShipType::DryCargo;
    else if (code == "tanker")
        type = ShipType::Tanker;
    else if (code == "cruise")
        type = ShipType::Cruise;
    else
        return false;
    return true;
}

std::string shipTypeCode(ShipType type)
{
    if (type == ShipType::DryCargo)
        return "cargo";
    if (type == ShipType::Tanker)
        return "tanker";
    return "cruise";
}

std::string shipTypeName(ShipType type)
{
    if (type == ShipType::DryCargo)
        return "сухогруз";
    if (type == ShipType::Tanker)
        return "танкер";
    return "круизный лайнер";
}

bool isShipDataCorrect(int dryTonnage, int fullTonnage)
{
    return dryTonnage > 0 && fullTonnage >= dryTonnage;
}

Position::Position(int x, int y)
    : _x(x)
    , _y(y)
{
}

int Position::getX() const
{
    return _x;
}

int Position::getY() const
{
    return _y;
}

double Position::distanceTo(const Position & other) const
{
    double dx = static_cast<double>(_x) - other._x;
    double dy = static_cast<double>(_y) - other._y;
    return std::sqrt(dx * dx + dy * dy);
}

std::string Position::toString() const
{
    std::ostringstream text;
    text << "(" << _x << "; " << _y << ")";
    return text.str();
}

Ship::Ship(const std::string & name, int dryTonnage, int fullTonnage, ShipType type)
    : _name(name)
    , _dryTonnage(dryTonnage)
    , _fullTonnage(fullTonnage)
    , _type(type)
    , _hasPosition(false)
{
    assert(invariant());
}

Ship::Ship(const std::string & name, int dryTonnage, int fullTonnage, ShipType type,
           const Position & position)
    : _name(name)
    , _dryTonnage(dryTonnage)
    , _fullTonnage(fullTonnage)
    , _type(type)
    , _hasPosition(true)
    , _position(position)
{
    assert(invariant());
}

std::string Ship::getName() const
{
    return _name;
}

int Ship::getDryTonnage() const
{
    return _dryTonnage;
}

int Ship::getFullTonnage() const
{
    return _fullTonnage;
}

ShipType Ship::getType() const
{
    return _type;
}

bool Ship::hasPosition() const
{
    return _hasPosition;
}

const Position & Ship::getPosition() const
{
    return _position;
}

bool Ship::invariant() const
{
    return !_name.empty() && isShipDataCorrect(_dryTonnage, _fullTonnage);
}

bool ShipRegistry::addShip(const Ship & ship)
{
    if (_ships.count(ship.getName()) > 0)
        return false;

    _ships.insert({ship.getName(), ship});
    return true;
}

bool ShipRegistry::updateShip(const Ship & ship)
{
    auto it = _ships.find(ship.getName());
    if (it == _ships.end())
        return false;

    it->second = ship;
    return true;
}

bool ShipRegistry::removeShip(const std::string & name)
{
    return _ships.erase(name) > 0;
}

const Ship * ShipRegistry::findShip(const std::string & name) const
{
    auto it = _ships.find(name);
    if (it == _ships.end())
        return nullptr;

    return &it->second;
}

const std::map<std::string, Ship> & ShipRegistry::getShips() const
{
    return _ships;
}

bool ShipRegistry::saveToFile(const std::string & filename) const
{
    std::vector<std::string> lines;
    for (auto it = _ships.begin(); it != _ships.end(); ++it)
    {
        const Ship & ship = it->second;

        std::ostringstream line;
        line << ship.getName() << " " << ship.getDryTonnage() << " "
             << ship.getFullTonnage() << " " << shipTypeCode(ship.getType());
        if (ship.hasPosition())
            line << " " << ship.getPosition().getX() << " " << ship.getPosition().getY();
        lines.push_back(line.str());
    }

    FileStorage storage;
    return storage.saveLines(filename, lines);
}

bool ShipRegistry::loadFromFile(const std::string & filename)
{
    FileStorage storage;
    std::vector<std::string> lines;
    if (!storage.loadLines(filename, lines))
        return false;

    std::map<std::string, Ship> loaded;

    for (const std::string & line : lines)
    {
        std::istringstream words(line);
        std::string name;
        std::string typeCode;
        int dryTonnage  = 0;
        int fullTonnage = 0;
        ShipType type;

        words >> name;
        if (name.empty())
            continue;

        words >> dryTonnage >> fullTonnage >> typeCode;
        if (words.fail() || !isShipDataCorrect(dryTonnage, fullTonnage)
            || !parseShipType(typeCode, type))
            return false;

        std::vector<std::string> rest;
        std::string word;
        while (words >> word)
            rest.push_back(word);

        if (rest.empty())
        {
            loaded.insert({name, Ship(name, dryTonnage, fullTonnage, type)});
            continue;
        }

        int x = 0;
        int y = 0;
        if (rest.size() != 2 || !parseInt(rest[0], x) || !parseInt(rest[1], y))
            return false;

        loaded.insert({name, Ship(name, dryTonnage, fullTonnage, type, Position(x, y))});
    }

    _ships = loaded;
    return true;
}

std::string getStatusName(SignalStatus status)
{
    if (status == SignalStatus::Received)
        return "принят";
    if (status == SignalStatus::Assigned)
        return "спасатели назначены";
    return "завершён";
}

DistressCall::DistressCall(int number, const std::string & ship, const Position & position)
    : _number(number)
    , _ship(ship)
    , _position(position)
    , _status(SignalStatus::Received)
{
    assert(invariant());
}

int DistressCall::getNumber() const
{
    return _number;
}

std::string DistressCall::getShip() const
{
    return _ship;
}

const Position & DistressCall::getPosition() const
{
    return _position;
}

SignalStatus DistressCall::getStatus() const
{
    return _status;
}

const std::vector<Rescuer> & DistressCall::getRescuers() const
{
    return _rescuers;
}

std::string DistressCall::rescuersToString() const
{
    if (_rescuers.empty())
        return "спасатели не назначены";

    std::string text;
    for (size_t i = 0; i < _rescuers.size(); i++)
    {
        if (i > 0)
            text += ", ";
        text += _rescuers[i].ship;
    }
    return text;
}

bool DistressCall::assignRescuers(const std::vector<Rescuer> & rescuers)
{
    if (_status == SignalStatus::Closed || rescuers.empty() || !isRescuersCorrect(rescuers))
        return false;

    _rescuers = rescuers;
    _status   = SignalStatus::Assigned;

    assert(invariant());
    return true;
}

bool DistressCall::close()
{
    if (_status != SignalStatus::Assigned)
        return false;

    _status = SignalStatus::Closed;
    return true;
}

bool DistressCall::isRescuersCorrect(const std::vector<Rescuer> & rescuers) const
{
    if (rescuers.size() > static_cast<size_t>(RESCUERS_COUNT))
        return false;

    for (size_t i = 0; i < rescuers.size(); i++)
    {
        if (rescuers[i].ship == _ship || rescuers[i].distance < 0)
            return false;

        for (size_t j = 0; j < i; j++)
        {
            if (rescuers[j].ship == rescuers[i].ship)
                return false;
        }
    }
    return true;
}

bool DistressCall::invariant() const
{
    return _number > 0 && !_ship.empty() && isRescuersCorrect(_rescuers);
}

bool SignalLog::addSignal(const DistressCall & signal)
{
    if (_signals.count(signal.getNumber()) > 0)
        return false;

    _signals.insert({signal.getNumber(), signal});
    return true;
}

DistressCall * SignalLog::findSignal(int number)
{
    auto it = _signals.find(number);
    if (it == _signals.end())
        return nullptr;

    return &it->second;
}

const DistressCall * SignalLog::findActiveSignal(const std::string & ship) const
{
    for (auto it = _signals.begin(); it != _signals.end(); ++it)
    {
        const DistressCall & signal = it->second;
        if (signal.getShip() == ship && signal.getStatus() != SignalStatus::Closed)
            return &signal;
    }
    return nullptr;
}

const std::map<int, DistressCall> & SignalLog::getSignals() const
{
    return _signals;
}

std::vector<Rescuer> findNearestShips(const ShipRegistry & registry, const Position & position,
                                      const std::vector<std::string> & excluded, int count)
{
    std::vector<Rescuer> candidates;

    const std::map<std::string, Ship> & ships = registry.getShips();
    for (auto it = ships.begin(); it != ships.end(); ++it)
    {
        const Ship & ship = it->second;
        if (!ship.hasPosition())
            continue;
        if (std::find(excluded.begin(), excluded.end(), ship.getName()) != excluded.end())
            continue;

        Rescuer rescuer = {ship.getName(), position.distanceTo(ship.getPosition())};
        candidates.push_back(rescuer);
    }

    std::sort(candidates.begin(), candidates.end(),
              [](const Rescuer & a, const Rescuer & b) {
                  if (a.distance != b.distance)
                      return a.distance < b.distance;
                  return a.ship < b.ship;
              });

    if (candidates.size() > static_cast<size_t>(count))
        candidates.resize(count);

    return candidates;
}

std::string assignRescuers(DistressCall & signal, const ShipRegistry & registry,
                           const SignalLog & signals)
{
    if (signal.getStatus() == SignalStatus::Closed)
        return "сигнал уже закрыт";

    std::vector<std::string> excluded;
    excluded.push_back(signal.getShip());

    const std::map<int, DistressCall> & all_signals = signals.getSignals();
    for (auto it = all_signals.begin(); it != all_signals.end(); ++it)
    {
        if (it->second.getStatus() != SignalStatus::Closed)
            excluded.push_back(it->second.getShip());
    }

    std::vector<Rescuer> nearest = findNearestShips(registry, signal.getPosition(),
                                                    excluded, RESCUERS_COUNT);
    if (nearest.empty())
        return "нет судов с известными координатами, которые могут прийти на помощь";

    signal.assignRescuers(nearest);
    return "";
}
