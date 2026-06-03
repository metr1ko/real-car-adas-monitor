#include <iostream>
#include "obd_parser.h"

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "Real Car ADAS Monitor - Version 1.0" << std::endl;
    std::cout << "========================================" << std::endl;

    OBDParser parser;
    std::string datasetPath = "../data/obd_data.csv";

    int loadedRecords = parser.load(datasetPath);

    if (loadedRecords > 0)
    {
        std::cout << "\nSuccessfully loaded " << loadedRecords << " records.\n";
        std::cout << "\n--- First 5 Records ---\n";

        int slowCount = 0, normalCount = 0, aggressiveCount = 0;

        for (int i = 0; i < loadedRecords; i++)
        {
            OBDRecord record = parser.getRecord(i);

            // Вывод первых 5 записей
            if (i < 5)
            {
                std::cout << "Record " << i + 1 << ": Speed=" << record.speed_kmh
                          << " km/h, RPM=" << record.engine_rpm
                          << ", Label=" << record.label << "\n";
            }

            // Подсчет статистики
            if (record.label == 0)
                slowCount++;
            else if (record.label == 1)
                normalCount++;
            else if (record.label == 2)
                aggressiveCount++;
        }

        std::cout << "\n--- Dataset Statistics ---\n";
        std::cout << "SLOW: " << slowCount << "\n";
        std::cout << "NORMAL: " << normalCount << "\n";
        std::cout << "AGGRESSIVE: " << aggressiveCount << "\n";
    }
    else
    {
        std::cerr << "Failed to load dataset. Please generate it using python script." << std::endl;
    }

    return 0;
}