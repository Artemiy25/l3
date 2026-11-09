/// @file l3_DomainLayer.h
/// Уровень предметной области (DL): суда, их координаты, сигналы бедствия
/// и поиск ближайших к терпящему бедствие судну судов.

#ifndef HW_L3_DOMAIN_LAYER_H
#define HW_L3_DOMAIN_LAYER_H

#include <map>
#include <string>
#include <vector>

/// Тип морского судна
enum class ShipType
{
    DryCargo,   ///< сухогруз
    Tanker,     ///< танкер
    Cruise      ///< круизный лайнер
};

/// Перевести код типа из команды или файла (cargo, tanker, cruise) в ShipType.
/// @return false, если код неизвестный
bool parseShipType(const std::string & code, ShipType & type);

/// Код типа для команды и файла: cargo, tanker, cruise
std::string shipTypeCode(ShipType type);

/// Название типа по-русски: сухогруз, танкер, круизный лайнер
std::string shipTypeName(ShipType type);

/// Проверить тоннаж: сухой больше 0, полный не меньше сухого
bool isShipDataCorrect(int dryTonnage, int fullTonnage);

// ======================================================================
// Координаты (добавлено в ЛР2)
// ======================================================================

/// Координаты судна на карте района наблюдения, в морских милях.
///
/// Объект-значение: своего номера нет, две позиции с одинаковыми x и y - одна и та же точка.
/// Район наблюдения небольшой, поэтому карта считается плоской,
/// а прямое расстояние между судами - это длина отрезка между точками.
class Position
{
public:
    /// @param x восток (+) / запад (-), мили
    /// @param y север (+) / юг (-), мили
    Position(int x = 0, int y = 0);

    int getX() const;
    int getY() const;

    /// Прямое расстояние до другой точки, мили
    double distanceTo(const Position & other) const;

    /// Координаты одной строкой, например "(10; -5)"
    std::string toString() const;

private:
    int _x;
    int _y;
};

/// Карточка "судно".
///
/// Сущность: судно отличают от других по наименованию.
/// Класс неизменяемый: чтобы изменить судно, создаётся новый объект Ship.
/// Координаты (ЛР2) могут быть ещё неизвестны - например, у только что добавленного судна.
class Ship
{
public:
    /// Судно с неизвестными координатами
    /// @param name        наименование судна
    /// @param dryTonnage  сухой тоннаж, т
    /// @param fullTonnage полный тоннаж, т
    /// @param type        тип судна
    Ship(const std::string & name, int dryTonnage, int fullTonnage, ShipType type);

    /// Судно с известными координатами
    Ship(const std::string & name, int dryTonnage, int fullTonnage, ShipType type,
         const Position & position);

    std::string getName() const;
    int         getDryTonnage() const;
    int         getFullTonnage() const;
    ShipType    getType() const;

    /// true, если координаты судна известны
    bool hasPosition() const;

    /// Координаты судна (имеют смысл, только если hasPosition() == true)
    const Position & getPosition() const;

    /// Инвариант класса: true, если данные судна корректные
    bool invariant() const;

private:
    std::string _name;          ///< наименование судна
    int         _dryTonnage;    ///< сухой тоннаж, т
    int         _fullTonnage;   ///< полный тоннаж, т (не меньше сухого)
    ShipType    _type;          ///< тип судна
    bool        _hasPosition;   ///< известны ли координаты
    Position    _position;      ///< координаты судна
};

/// Список наблюдаемых судов.
/// Суда хранятся по наименованию, поэтому двух судов с одним наименованием быть не может.
class ShipRegistry
{
public:
    /// Добавить судно. @return false, если судно с таким наименованием уже есть
    bool addShip(const Ship & ship);

    /// Заменить судно с тем же наименованием. @return false, если такого судна нет
    bool updateShip(const Ship & ship);

    /// Удалить судно. @return false, если такого судна нет
    bool removeShip(const std::string & name);

    /// Найти судно. @return nullptr, если такого судна нет
    const Ship * findShip(const std::string & name) const;

    /// Все суда (упорядочены по наименованию)
    const std::map<std::string, Ship> & getShips() const;

    /// Сохранить список в файл. @return false, если записать файл не удалось
    bool saveToFile(const std::string & filename) const;

    /// Загрузить список из файла (старое содержимое заменяется).
    /// Если файла нет или в нём есть ошибка, список не меняется.
    /// @return false, если загрузить не удалось
    bool loadFromFile(const std::string & filename);

private:
    std::map<std::string, Ship> _ships;   ///< наименование -> судно
};

// ======================================================================
// Сигналы бедствия (добавлено в ЛР2)
// ======================================================================

/// Сколько ближайших судов назначается на помощь по одному сигналу SOS
const int RESCUERS_COUNT = 3;

/// Состояние сигнала SOS: принят -> спасатели назначены -> завершён
enum class SignalStatus
{
    Received,   ///< сигнал принят, спасатели ещё не назначены
    Assigned,   ///< ближайшие суда назначены на помощь
    Closed      ///< спасение завершено
};

/// Название состояния сигнала по-русски
std::string getStatusName(SignalStatus status);

/// Судно, назначенное на помощь (объект-значение): какое судно и как далеко оно было.
/// Судно указывается по наименованию - ссылка на судно из списка наблюдаемых.
struct Rescuer
{
    std::string ship;       ///< наименование судна
    double      distance;   ///< прямое расстояние до терпящего бедствие судна, мили
};

/// Сигнал бедствия SOS. Сущность: у сигнала есть номер, по которому его отличают от других.
///
/// Сигнал - корень агрегата "Сигнал SOS": координаты бедствия и список спасателей
/// меняются только через методы сигнала.
class DistressCall
{
public:
    /// @param number   номер сигнала (больше 0)
    /// @param ship     наименование терпящего бедствие судна
    /// @param position координаты судна в момент подачи сигнала
    DistressCall(int number, const std::string & ship, const Position & position);

    int                          getNumber() const;
    std::string                  getShip() const;
    const Position &             getPosition() const;
    SignalStatus                 getStatus() const;
    const std::vector<Rescuer> & getRescuers() const;

    /// Спасатели одной строкой, например "Neva, Ob, Baltica"
    std::string rescuersToString() const;

    /// Назначить спасателей (старый список заменяется - суда могли переместиться).
    /// @return false, если сигнал уже закрыт или список не подходит по инварианту
    bool assignRescuers(const std::vector<Rescuer> & rescuers);

    /// Завершить спасение. @return false, если спасатели ещё не назначены или сигнал уже закрыт
    bool close();

    /// Инвариант класса: номер больше 0, спасателей не больше RESCUERS_COUNT,
    /// среди спасателей нет самого терпящего бедствие судна и нет повторов
    bool invariant() const;

private:
    /// Проверить список спасателей по правилам агрегата
    bool isRescuersCorrect(const std::vector<Rescuer> & rescuers) const;

    int                  _number;
    std::string          _ship;
    Position             _position;
    SignalStatus         _status;
    std::vector<Rescuer> _rescuers;
};

/// Журнал сигналов SOS
class SignalLog
{
public:
    /// Добавить сигнал. @return false, если сигнал с таким номером уже есть
    bool addSignal(const DistressCall & signal);

    /// Найти сигнал. @return nullptr, если такого сигнала нет
    DistressCall * findSignal(int number);

    /// Найти незакрытый сигнал от судна. @return nullptr, если судно бедствия не терпит
    const DistressCall * findActiveSignal(const std::string & ship) const;

    /// Все сигналы (упорядочены по номеру)
    const std::map<int, DistressCall> & getSignals() const;

private:
    std::map<int, DistressCall> _signals;   ///< номер -> сигнал
};

/// Служба поиска ближайших судов.
///
/// Выбирает не больше count судов с известными координатами, находящихся на минимальном
/// прямом расстоянии от точки position. Суда из списка excluded не рассматриваются.
/// При равном расстоянии суда упорядочиваются по наименованию.
/// @return суда по возрастанию расстояния
std::vector<Rescuer> findNearestShips(const ShipRegistry & registry, const Position & position,
                                      const std::vector<std::string> & excluded, int count);

/// Служба назначения спасателей по сигналу SOS.
///
/// Находит RESCUERS_COUNT ближайших к месту бедствия судов и назначает их на помощь.
/// Не рассматриваются само терпящее бедствие судно и суда, которые сами подали
/// незакрытый сигнал SOS. Это служба, потому что операция затрагивает
/// сразу сигнал, список судов и журнал сигналов.
/// @return пустая строка, если спасатели назначены, иначе - текст ошибки
std::string assignRescuers(DistressCall & signal, const ShipRegistry & registry,
                           const SignalLog & signals);

#endif // HW_L3_DOMAIN_LAYER_H
