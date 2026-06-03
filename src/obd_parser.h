#pragma once
#include <string>
#include <vector>
#include <stdexcept>

// Структура, хранящая одну запись телеметрии
struct OBDRecord
{
    float speed_kmh;
    float engine_rpm;
    float throttle_pos;
    float coolant_temp;
    float fuel_level;
    float intake_air_temp;
    int label; // SLOW=0, NORMAL=1, AGGRESSIVE=2
};

// Класс парсера
class OBDParser
{
private:
    std::vector<OBDRecord> records;

public:
    // Конвертация строки в число
    static int stringToLabel(const std::string &str);

    // Загрузка данных из CSV файла, возвращает кол-во записей или -1 при ошибке
    int load(const std::string &filename);

    // Получение записи по индексу (бросает std::out_of_range)
    OBDRecord getRecord(int index) const;

    // Получить общее количество загруженных записей
    int getRecordsCount() const;
};