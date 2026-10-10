// AuditObject.cpp — расчёт риска и вспомогательные функции записи
#include "AuditObject.h"
#include <cstring>

const char* const OBJECT_TYPES[TYPE_COUNT] = {
    "Рабочее место", "Ноутбук", "Сервер", "Сетевое оборудование"
};

const char* const DEPARTMENTS[DEPT_COUNT] = {
    "Бухгалтерия", "Абонентский отдел", "Паспортный стол", "Диспетчерская", "Администрация"
};

// Предельный «возраст» антивирусных баз в днях
const int AV_MAX_AGE_DAYS = 7;

// Проверка 2: базы устарели (больше 7 дней на дату проверки)
static bool avOutdated(const AuditObject& obj) {
    return obj.antivirus && daysBetween(obj.avUpdate, obj.checkDate) > AV_MAX_AGE_DAYS;
}

int calcRisk(const AuditObject& obj) {
    int risk = 0;
    bool hasViolation = false;

    if (!obj.antivirus) { risk += 4; hasViolation = true; }      // нет антивируса
    if (avOutdated(obj)) { risk += 2; hasViolation = true; }     // устаревшие базы
    if (!obj.passwordOk) { risk += 3; hasViolation = true; }     // слабый пароль
    if (obj.personalData && hasViolation) risk += 2;             // нарушение на ПК с ПДн

    return risk;
}

int riskClass(int risk) {
    if (risk <= 2) return 0;
    if (risk <= 5) return 1;
    return 2;
}

std::string riskLevel(int risk) {
    static const char* const names[] = {"низкий", "средний", "высокий"};
    return names[riskClass(risk)];
}

std::string violations(const AuditObject& obj) {
    std::string text;
    auto add = [&text](const std::string& v) {
        if (!text.empty()) text += "; ";
        text += v;
    };
    if (!obj.antivirus) add("нет антивируса");
    if (avOutdated(obj)) add("устарели антивирусные базы");
    if (!obj.passwordOk) add("пароль не соответствует политике");
    if (text.empty()) return "нарушений нет";
    if (obj.personalData) add("на объекте обрабатываются ПДн");
    return text;
}

void setField(char* field, int size, const std::string& value) {
    std::memset(field, 0, size);
    std::strncpy(field, value.c_str(), size - 1);
}
