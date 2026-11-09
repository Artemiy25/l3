/// @file l2_ApplicationLayer.cpp
/// Операционный уровень (AL): выполнение команд пользователя.

#include <iomanip>
#include <sstream>

#include "hw/l2_ApplicationLayer.h"

/// Перевести строку в целое число.
/// @return false, если строка - не целое число (например "abc" или "5x")
static bool toInt(const std::string & text, int & value)
{
    std::istringstream in(text);
    in >> value;
    // Число должно прочитаться, и после него в строке ничего не должно остаться
    return !in.fail() && in.eof();
}

/// Номер сигнала в виде текста: "№5"
static std::string numberText(int number)
{
    std::ostringstream text;
    text << "№" << number;
    return text.str();
}

/// Расстояние в виде текста с одним знаком после запятой: "5.0 миль"
static std::string distanceText(double distance)
{
    std::ostringstream text;
    text << std::fixed << std::setprecision(1) << distance << " миль";
    return text.str();
}

Application::Application(ShipRegistry & registry, SignalLog & signals)
    : _registry(registry)
    , _signals(signals)
{
}

std::string Application::execute(const std::vector<std::string> & args)
{
    const std::string & command = args[0];

    if (command == "add")
        return add(args);
    if (command == "remove")
        return remove(args);
    if (command == "update")
        return update(args);
    if (command == "view")
        return view();
    if (command == "save")
        return save(args);
    if (command == "load")
        return load(args);

    if (command == "move")
        return move(args);
    if (command == "sos")
        return sos(args);
    if (command == "nearest")
        return nearest(args);
    if (command == "close")
        return close(args);
    if (command == "signals")
        return signals();

    return "Неизвестная команда '" + command + "'\n";
}

// add <наименование> <сухой тоннаж> <полный тоннаж> <тип>
std::string Application::add(const std::vector<std::string> & args)
{
    if (args.size() != 5)
        return "Неверный формат команды. Нужно: add <наименование> <сухой тоннаж> <полный тоннаж> <тип cargo|tanker|cruise>\n";

    std::string name = args[1];
    int dryTonnage  = 0;
    int fullTonnage = 0;

    if (!toInt(args[2], dryTonnage) || !toInt(args[3], fullTonnage))
        return "Ошибка: тоннаж должен быть целым числом\n";

    if (!isShipDataCorrect(dryTonnage, fullTonnage))
        return "Ошибка: сухой тоннаж должен быть больше 0, полный тоннаж - не меньше сухого\n";

    ShipType type;
    if (!parseShipType(args[4], type))
        return "Ошибка: тип судна должен быть cargo (сухогруз), tanker (танкер) или cruise (круизный лайнер)\n";

    if (!_registry.addShip(Ship(name, dryTonnage, fullTonnage, type)))
        return "Ошибка: судно '" + name + "' уже наблюдается\n";

    return "Судно '" + name + "' добавлено\n";
}

// remove <наименование>
std::string Application::remove(const std::vector<std::string> & args)
{
    if (args.size() != 2)
        return "Неверный формат команды. Нужно: remove <наименование>\n";

    std::string name = args[1];

    if (!_registry.removeShip(name))
        return "Ошибка: судно '" + name + "' не найдено\n";

    return "Судно '" + name + "' удалено\n";
}

// update <наименование> <сухой тоннаж> <полный тоннаж>
// Тип судна и координаты не меняются.
std::string Application::update(const std::vector<std::string> & args)
{
    if (args.size() != 4)
        return "Неверный формат команды. Нужно: update <наименование> <сухой тоннаж> <полный тоннаж>\n";

    std::string name = args[1];
    int dryTonnage  = 0;
    int fullTonnage = 0;

    if (!toInt(args[2], dryTonnage) || !toInt(args[3], fullTonnage))
        return "Ошибка: тоннаж должен быть целым числом\n";

    const Ship * old_ship = _registry.findShip(name);
    if (old_ship == nullptr)
        return "Ошибка: судно '" + name + "' не найдено\n";

    if (!isShipDataCorrect(dryTonnage, fullTonnage))
        return "Ошибка: сухой тоннаж должен быть больше 0, полный тоннаж - не меньше сухого\n";

    if (old_ship->hasPosition())
        _registry.updateShip(Ship(name, dryTonnage, fullTonnage, old_ship->getType(),
                                  old_ship->getPosition()));
    else
        _registry.updateShip(Ship(name, dryTonnage, fullTonnage, old_ship->getType()));

    return "Судно '" + name + "' изменено\n";
}

// view
std::string Application::view()
{
    const std::map<std::string, Ship> & ships = _registry.getShips();
    if (ships.empty())
        return "Список наблюдаемых судов пуст\n";

    std::ostringstream out;
    out << "Наблюдаемых судов: " << ships.size() << "\n";

    for (auto it = ships.begin(); it != ships.end(); ++it)
    {
        const Ship & ship = it->second;

        out << ship.getName() << " | тип: " << shipTypeName(ship.getType())
            << " | сухой тоннаж: " << ship.getDryTonnage() << " т"
            << " | полный тоннаж: " << ship.getFullTonnage() << " т";
        // Координаты (ЛР2) показываем, только если они известны
        if (ship.hasPosition())
            out << " | координаты: " << ship.getPosition().toString();
        out << "\n";
    }
    return out.str();
}

// save <файл>
std::string Application::save(const std::vector<std::string> & args)
{
    if (args.size() != 2)
        return "Неверный формат команды. Нужно: save <файл>\n";

    if (!_registry.saveToFile(args[1]))
        return "Ошибка: не удалось сохранить список судов в файл " + args[1] + "\n";

    return "Список судов сохранён в файл " + args[1] + "\n";
}

// load <файл>
std::string Application::load(const std::vector<std::string> & args)
{
    if (args.size() != 2)
        return "Неверный формат команды. Нужно: load <файл>\n";

    if (!_registry.loadFromFile(args[1]))
        return "Ошибка: не удалось загрузить список судов из файла " + args[1] + "\n";

    return "Список судов загружен из файла " + args[1] + "\n";
}

// ---------------------------------------------------------------- Координаты и сигналы (ЛР2)

// move <наименование> <x> <y> - задать координаты судна
std::string Application::move(const std::vector<std::string> & args)
{
    if (args.size() != 4)
        return "Неверный формат команды. Нужно: move <наименование> <x> <y>\n";

    std::string name = args[1];
    int x = 0;
    int y = 0;
    if (!toInt(args[2], x) || !toInt(args[3], y))
        return "Ошибка: координаты должны быть целыми числами (мили)\n";

    const Ship * ship = _registry.findShip(name);
    if (ship == nullptr)
        return "Ошибка: судно '" + name + "' не найдено\n";

    Position position(x, y);
    _registry.updateShip(Ship(name, ship->getDryTonnage(), ship->getFullTonnage(),
                              ship->getType(), position));

    return "Судно '" + name + "' находится в точке " + position.toString() + "\n";
}

// sos <номер сигнала> <наименование судна> - судно терпит бедствие
std::string Application::sos(const std::vector<std::string> & args)
{
    if (args.size() != 3)
        return "Неверный формат команды. Нужно: sos <номер сигнала> <судно>\n";

    int number = 0;
    if (!toInt(args[1], number) || number <= 0)
        return "Ошибка: номер сигнала должен быть целым числом больше 0\n";

    std::string name = args[2];
    const Ship * ship = _registry.findShip(name);
    if (ship == nullptr)
        return "Ошибка: судно '" + name + "' не найдено\n";

    if (!ship->hasPosition())
        return "Ошибка: координаты судна '" + name + "' неизвестны, задайте их командой move\n";

    const DistressCall * active = _signals.findActiveSignal(name);
    if (active != nullptr)
        return "Ошибка: судно '" + name + "' уже подало сигнал " + numberText(active->getNumber()) + "\n";

    // Координаты бедствия запоминаются на момент подачи сигнала
    if (!_signals.addSignal(DistressCall(number, name, ship->getPosition())))
        return "Ошибка: сигнал " + numberText(number) + " уже есть\n";

    return "Принят сигнал SOS " + numberText(number) + " от судна '" + name + "' в точке "
         + ship->getPosition().toString() + "\n";
}

// nearest <номер сигнала> - найти три ближайших судна и назначить их на помощь
std::string Application::nearest(const std::vector<std::string> & args)
{
    if (args.size() != 2)
        return "Неверный формат команды. Нужно: nearest <номер сигнала>\n";

    int number = 0;
    if (!toInt(args[1], number))
        return "Ошибка: номер сигнала должен быть целым числом\n";

    DistressCall * signal = _signals.findSignal(number);
    if (signal == nullptr)
        return "Ошибка: сигнал " + numberText(number) + " не найден\n";

    // Поиск и назначение выполняет служба предметной области
    std::string error = assignRescuers(*signal, _registry, _signals);
    if (error != "")
        return "Ошибка: не удалось назначить спасателей по сигналу " + numberText(number) + ": " + error + "\n";

    const std::vector<Rescuer> & rescuers = signal->getRescuers();

    std::ostringstream out;
    out << "Сигнал SOS " << numberText(number) << ": судно '" << signal->getShip()
        << "' в точке " << signal->getPosition().toString() << "\n";
    out << "Ближайшие суда:\n";
    for (size_t i = 0; i < rescuers.size(); i++)
        out << i + 1 << ") " << rescuers[i].ship << " - " << distanceText(rescuers[i].distance) << "\n";

    if (rescuers.size() < static_cast<size_t>(RESCUERS_COUNT))
        out << "Внимание: найдено судов: " << rescuers.size() << " из " << RESCUERS_COUNT << "\n";

    return out.str();
}

// close <номер сигнала> - спасение завершено
std::string Application::close(const std::vector<std::string> & args)
{
    if (args.size() != 2)
        return "Неверный формат команды. Нужно: close <номер сигнала>\n";

    int number = 0;
    if (!toInt(args[1], number))
        return "Ошибка: номер сигнала должен быть целым числом\n";

    DistressCall * signal = _signals.findSignal(number);
    if (signal == nullptr)
        return "Ошибка: сигнал " + numberText(number) + " не найден\n";

    if (!signal->close())
        return "Ошибка: по сигналу " + numberText(number) + " спасатели не назначены или он уже закрыт\n";

    return "Спасение по сигналу " + numberText(number) + " завершено\n";
}

// signals - показать все сигналы SOS
std::string Application::signals()
{
    const std::map<int, DistressCall> & all_signals = _signals.getSignals();
    if (all_signals.empty())
        return "Сигналов SOS нет\n";

    std::ostringstream out;
    out << "Сигналов SOS: " << all_signals.size() << "\n";

    for (auto it = all_signals.begin(); it != all_signals.end(); ++it)
    {
        const DistressCall & signal = it->second;
        out << "Сигнал " << numberText(signal.getNumber()) << " | судно '" << signal.getShip()
            << "' | " << signal.getPosition().toString() << " | " << getStatusName(signal.getStatus())
            << " | " << signal.rescuersToString() << "\n";
    }
    return out.str();
}
