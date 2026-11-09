/// @file l1_UserInterface.cpp
/// Интерфейс пользователя (UI): читает команды и выводит ответы.
///
/// Команды (по одной в строке):
///     add <наименование> <сухой тоннаж> <полный тоннаж> <тип cargo|tanker|cruise>
///     remove <наименование>
///     update <наименование> <сухой тоннаж> <полный тоннаж>
///     view
///     save <файл>
///     load <файл>
///     move <наименование> <x> <y>         - задать координаты судна (мили)
///     sos <номер сигнала> <судно>         - судно терпит бедствие
///     nearest <номер сигнала>             - три ближайших судна идут на помощь
///     close <номер сигнала>               - спасение завершено
///     signals                             - показать все сигналы SOS
///     exit

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "hw/l2_ApplicationLayer.h"

/// Разбить строку на слова по пробелам
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
    ShipRegistry registry;                 // уровень предметной области: суда
    SignalLog    signals;                  // уровень предметной области: сигналы SOS
    Application  app(registry, signals);   // операционный уровень

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::vector<std::string> args = splitBySpaces(line);

        if (args.empty())
            continue;           // пустая строка
        if (args[0] == "exit")
            break;

        std::cout << app.execute(args);
    }

    return 0;
}
