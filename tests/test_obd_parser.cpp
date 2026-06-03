#include <gtest/gtest.h>
#include "../src/obd_parser.h"
#include <fstream>
#include <cstdio> // Для std::remove

// 1. Тест: конвертация меток (SLOW -> 0, NORMAL -> 1, AGGRESSIVE -> 2)
TEST(OBDParserTest, LabelConversion)
{
    EXPECT_EQ(OBDParser::stringToLabel("SLOW"), 0);
    EXPECT_EQ(OBDParser::stringToLabel("NORMAL"), 1);
    EXPECT_EQ(OBDParser::stringToLabel("AGGRESSIVE"), 2);
    EXPECT_THROW(OBDParser::stringToLabel("INVALID"), std::invalid_argument);
}

// 2. Тест: загрузка несуществующего файла возвращает -1
TEST(OBDParserTest, FileNotFound)
{
    OBDParser parser;
    EXPECT_EQ(parser.load("non_existent_file.csv"), -1);
}

// 3. Тест: getRecord() бросает исключение при неверном индексе
TEST(OBDParserTest, OutOfRangeIndex)
{
    OBDParser parser;
    // Парсер пока пуст
    EXPECT_THROW(parser.getRecord(0), std::out_of_range);
    EXPECT_THROW(parser.getRecord(-1), std::out_of_range);
}

// 4. Тест: парсинг корректного CSV файла
TEST(OBDParserTest, ParseValidCSV)
{
    const std::string filename = "temp_valid.csv";
    std::ofstream out(filename);
    out << "speed,rpm,throttle,coolant,fuel,intake,label\n";
    out << "60.5,2000.0,25.0,90.0,50.0,20.0,NORMAL\n";
    out << "120.0,4000.0,80.0,95.0,40.0,25.0,AGGRESSIVE\n";
    out.close();

    OBDParser parser;
    int loaded = parser.load(filename);
    EXPECT_EQ(loaded, 2);

    OBDRecord r1 = parser.getRecord(0);
    EXPECT_FLOAT_EQ(r1.speed_kmh, 60.5);
    EXPECT_EQ(r1.label, 1); // NORMAL -> 1

    std::remove(filename.c_str());
}

// 5. Тест: парсинг файла с некорректной строкой
TEST(OBDParserTest, SkipInvalidRow)
{
    const std::string filename = "temp_invalid.csv";
    std::ofstream out(filename);
    out << "speed,rpm,throttle,coolant,fuel,intake,label\n";
    out << "60.5,2000.0,25.0,90.0,50.0,20.0,NORMAL\n";
    out << "invalid_speed,4000.0,80.0,95.0,40.0,25.0,AGGRESSIVE\n"; // Сломана
    out << "10.0,1000.0,10.0,85.0,60.0,15.0,SLOW\n";
    out.close();

    OBDParser parser;
    int loaded = parser.load(filename);

    // Должно загрузиться 2 валидные строки, 1 пропущена
    EXPECT_EQ(loaded, 2);
    EXPECT_EQ(parser.getRecord(1).label, 0); // SLOW -> 0

    std::remove(filename.c_str());
}