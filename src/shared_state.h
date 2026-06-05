/**
 * @file shared_state.h
 * @brief Потокобезопасная структура для обмена данными между потоком OBD и основным потоком.
 */
#ifndef SHARED_STATE_H
#define SHARED_STATE_H

#include <mutex>
#include <atomic>
#include "obd_parser.h"

/**
 * @struct SharedState
 * @brief Хранит текущие данные автомобиля, результаты ИИ и счетчики статистики.
 */
struct SharedState
{
    OBDRecord current_obd;       /**< Текущая запись телеметрии из CSV */
    int current_style_label = 1; /**< Текущий стиль вождения (0=SLOW, 1=NORMAL, 2=AGGRESSIVE) */

    int drowsy_alerts = 0;     /**< Счетчик предупреждений об усталости */
    int distracted_alerts = 0; /**< Счетчик предупреждений об отвлечении */
    int aggressive_alerts = 0; /**< Счетчик предупреждений об агрессивном вождении */
    int total_obd_records = 0; /**< Общее количество обработанных записей телеметрии */

    std::atomic<bool> running{true}; /**< Флаг работы приложения (false для завершения потоков) */
    std::mutex mtx;                  /**< Мьютекс для защиты доступа к данным */
};

#endif // SHARED_STATE_H