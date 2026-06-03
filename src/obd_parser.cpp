#include "obd_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

int OBDParser::stringToLabel(const std::string &str)
{
    if (str == "SLOW")
        return 0;
    if (str == "NORMAL")
        return 1;
    if (str == "AGGRESSIVE")
        return 2;
    throw std::invalid_argument("Unknown label: " + str);
}

int OBDParser::load(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return -1;
    }

    records.clear();
    std::string line;

    // Пропускаем заголовок
    std::getline(file, line);

    int lineNum = 1;
    while (std::getline(file, line))
    {
        lineNum++;
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, ','))
        {
            tokens.push_back(token);
        }

        if (tokens.size() != 7)
        {
            std::cerr << "Warning: Invalid data format at line " << lineNum << ". Skipping." << std::endl;
            continue;
        }

        try
        {
            OBDRecord record;
            record.speed_kmh = std::stof(tokens[0]);
            record.engine_rpm = std::stof(tokens[1]);
            record.throttle_pos = std::stof(tokens[2]);
            record.coolant_temp = std::stof(tokens[3]);
            record.fuel_level = std::stof(tokens[4]);
            record.intake_air_temp = std::stof(tokens[5]);
            // Очищаем токен от возможных символов возврата каретки
            if (!tokens[6].empty() && tokens[6].back() == '\r')
            {
                tokens[6].pop_back();
            }
            record.label = stringToLabel(tokens[6]);

            records.push_back(record);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Warning: Data parse error at line " << lineNum << ". Skipping." << std::endl;
        }
    }

    return records.size();
}

OBDRecord OBDParser::getRecord(int index) const
{
    if (index < 0 || index >= records.size())
    {
        throw std::out_of_range("Record index out of bounds");
    }
    return records[index];
}

int OBDParser::getRecordsCount() const
{
    return records.size();
}