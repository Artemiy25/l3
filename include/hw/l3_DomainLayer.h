#ifndef HW_L3_DOMAIN_LAYER_H
#define HW_L3_DOMAIN_LAYER_H

#include <map>
#include <string>
#include <vector>

enum class ShipType
{
    DryCargo,
    Tanker,
    Cruise
};

bool parseShipType(const std::string & code, ShipType & type);

std::string shipTypeCode(ShipType type);

std::string shipTypeName(ShipType type);

bool isShipDataCorrect(int dryTonnage, int fullTonnage);

class Position
{
public:
    Position(int x = 0, int y = 0);

    int getX() const;
    int getY() const;

    double distanceTo(const Position & other) const;

    std::string toString() const;

private:
    int _x;
    int _y;
};

class Ship
{
public:
    Ship(const std::string & name, int dryTonnage, int fullTonnage, ShipType type);

    Ship(const std::string & name, int dryTonnage, int fullTonnage, ShipType type,
         const Position & position);

    std::string getName() const;
    int         getDryTonnage() const;
    int         getFullTonnage() const;
    ShipType    getType() const;

    bool hasPosition() const;

    const Position & getPosition() const;

    bool invariant() const;

private:
    std::string _name;
    int         _dryTonnage;
    int         _fullTonnage;
    ShipType    _type;
    bool        _hasPosition;
    Position    _position;
};

class ShipRegistry
{
public:
    bool addShip(const Ship & ship);

    bool updateShip(const Ship & ship);

    bool removeShip(const std::string & name);

    const Ship * findShip(const std::string & name) const;

    const std::map<std::string, Ship> & getShips() const;

    bool saveToFile(const std::string & filename) const;

    bool loadFromFile(const std::string & filename);

private:
    std::map<std::string, Ship> _ships;
};

const int RESCUERS_COUNT = 3;

enum class SignalStatus
{
    Received,
    Assigned,
    Closed
};

std::string getStatusName(SignalStatus status);

struct Rescuer
{
    std::string ship;
    double      distance;
};

class DistressCall
{
public:
    DistressCall(int number, const std::string & ship, const Position & position);

    int                          getNumber() const;
    std::string                  getShip() const;
    const Position &             getPosition() const;
    SignalStatus                 getStatus() const;
    const std::vector<Rescuer> & getRescuers() const;

    std::string rescuersToString() const;

    bool assignRescuers(const std::vector<Rescuer> & rescuers);

    bool close();

    bool invariant() const;

private:
    bool isRescuersCorrect(const std::vector<Rescuer> & rescuers) const;

    int                  _number;
    std::string          _ship;
    Position             _position;
    SignalStatus         _status;
    std::vector<Rescuer> _rescuers;
};

class SignalLog
{
public:
    bool addSignal(const DistressCall & signal);

    DistressCall * findSignal(int number);

    const DistressCall * findActiveSignal(const std::string & ship) const;

    const std::map<int, DistressCall> & getSignals() const;

private:
    std::map<int, DistressCall> _signals;
};

std::vector<Rescuer> findNearestShips(const ShipRegistry & registry, const Position & position,
                                      const std::vector<std::string> & excluded, int count);

std::string assignRescuers(DistressCall & signal, const ShipRegistry & registry,
                           const SignalLog & signals);

#endif
