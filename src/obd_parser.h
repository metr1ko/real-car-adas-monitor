/**
 * @file obd_parser.h
 * @brief Модуль чтения и парсинга данных телеметрии автомобиля (OBD-II).
 */
#ifndef OBD_PARSER_H
#define OBD_PARSER_H

#include <string>
#include <vector>

/**
 * @struct OBDRecord
 * @brief Структура для хранения одной записи телеметрии.
 */
struct OBDRecord
{
    float speed_kmh;       /**< Скорость (км/ч) */
    float engine_rpm;      /**< Обороты двигателя (об/мин) */
    float throttle_pos;    /**< Положение дроссельной заслонки (%) */
    float coolant_temp;    /**< Температура охлаждающей жидкости (C) */
    float fuel_level;      /**< Уровень топлива (%) */
    float intake_air_temp; /**< Температура впускаемого воздуха (C) */
};

/**
 * @class OBDParser
 * @brief Класс для загрузки и выдачи записей телеметрии из CSV файла.
 */
class OBDParser
{
public:
    /**
     * @brief Загружает данные из CSV файла.
     * @param filepath Путь к файлу CSV.
     */
    void load(const std::string &filepath);

    /**
     * @brief Получает запись по индексу.
     * @param index Номер строки (начиная с 0).
     * @return Запись телеметрии OBDRecord.
     */
    OBDRecord getRecord(int index) const;

    /**
     * @brief Возвращает общее количество загруженных записей.
     * @return Количество строк данных.
     */
    int getRecordsCount() const;

private:
    std::vector<OBDRecord> data;
};

#endif // OBD_PARSER_H