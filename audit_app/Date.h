// Date.h — дата в формате ДД.ММ.ГГГГ
#ifndef DATE_H
#define DATE_H

#include <string>

struct Date {
    int day;    // день 1..31
    int month;  // месяц 1..12
    int year;   // год

    bool isValid() const;                 // проверка корректности даты
    long toDays() const;                  // номер дня от условного начала отсчёта
    std::string toString() const;         // "ДД.ММ.ГГГГ"
    static bool parse(const std::string& text, Date& out);  // разбор строки
};

// Количество дней между датами (to - from)
long daysBetween(const Date& from, const Date& to);

#endif
