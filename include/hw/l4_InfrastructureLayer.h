/// @file l4_InfrastructureLayer.h
/// Инфраструктурный уровень (IL): чтение и запись текстовых файлов.

#ifndef HW_L4_INFRASTRUCTURE_LAYER_H
#define HW_L4_INFRASTRUCTURE_LAYER_H

#include <string>
#include <vector>

/// Хранение данных в текстовом файле.
///
/// Класс ничего не знает про суда: он просто записывает и читает строки.
/// Превращать суда в строки и обратно - задача уровня предметной области.
class FileStorage
{
public:
    /// Записать строки в файл (каждую с новой строки).
    /// @return false, если файл не удалось открыть
    bool saveLines(const std::string & filename, const std::vector<std::string> & lines);

    /// Прочитать все строки файла в lines.
    /// @return false, если файл не удалось открыть
    bool loadLines(const std::string & filename, std::vector<std::string> & lines);
};

#endif // HW_L4_INFRASTRUCTURE_LAYER_H
