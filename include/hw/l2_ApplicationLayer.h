/// @file l2_ApplicationLayer.h
/// Операционный уровень (AL): выполнение команд пользователя.

#ifndef HW_L2_APPLICATION_LAYER_H
#define HW_L2_APPLICATION_LAYER_H

#include <string>
#include <vector>

#include "hw/l3_DomainLayer.h"

/// Выполняет команды пользователя.
///
/// Получает команду в виде списка слов (например {"remove", "Volga"}),
/// проверяет аргументы, вызывает методы предметной области и возвращает текст ответа.
/// Сам ничего на экран не выводит - это делает интерфейс пользователя.
/// Данных о судах и сигналах не хранит, только ссылки на список судов и журнал сигналов.
class Application
{
public:
    Application(ShipRegistry & registry, SignalLog & signals);

    /// Выполнить команду. args[0] - имя команды, дальше её аргументы.
    /// @return текст ответа пользователю
    std::string execute(const std::vector<std::string> & args);

private:
    // Команды списка судов (ЛР1)
    std::string add(const std::vector<std::string> & args);
    std::string remove(const std::vector<std::string> & args);
    std::string update(const std::vector<std::string> & args);
    std::string view();
    std::string save(const std::vector<std::string> & args);
    std::string load(const std::vector<std::string> & args);

    // Координаты и сигналы бедствия (ЛР2)
    std::string move(const std::vector<std::string> & args);
    std::string sos(const std::vector<std::string> & args);
    std::string nearest(const std::vector<std::string> & args);
    std::string close(const std::vector<std::string> & args);
    std::string signals();

    ShipRegistry & _registry;
    SignalLog &    _signals;
};

#endif // HW_L2_APPLICATION_LAYER_H
