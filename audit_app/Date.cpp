// Date.cpp — реализация работы с датами
#include "Date.h"
#include <cstdio>

// Високосный ли год
static bool isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

bool Date::isValid() const {
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year < 2000 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    int maxDay = daysInMonth[month - 1];
    if (month == 2 && isLeap(year)) maxDay = 29;
    return day >= 1 && day <= maxDay;
}

// Перевод даты в число дней (алгоритм для григорианского календаря)
long Date::toDays() const {
    int y = year;
    int m = month;
    if (m <= 2) { y -= 1; m += 12; }
    return 365L * y + y / 4 - y / 100 + y / 400 + (153 * (m - 3) + 2) / 5 + day;
}

std::string Date::toString() const {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d.%02d.%04d", day, month, year);
    return buf;
}

bool Date::parse(const std::string& text, Date& out) {
    int d, m, y;
    char tail;
    // Формат строго ДД.ММ.ГГГГ, лишние символы запрещены
    if (std::sscanf(text.c_str(), "%d.%d.%d%c", &d, &m, &y, &tail) != 3) return false;
    Date tmp{d, m, y};
    if (!tmp.isValid()) return false;
    out = tmp;
    return true;
}

long daysBetween(const Date& from, const Date& to) {
    return to.toDays() - from.toDays();
}
