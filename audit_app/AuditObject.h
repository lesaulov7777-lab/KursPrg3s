// AuditObject.h — запись бинарного файла «Объекты аудита»
#ifndef AUDIT_OBJECT_H
#define AUDIT_OBJECT_H

#include <string>
#include "Date.h"

// Размеры текстовых полей в байтах (с запасом под кириллицу в UTF-8)
const int INV_LEN = 32;
const int NAME_LEN = 96;
const int OS_LEN = 64;

// Справочники типов объектов и отделов (коды хранятся в файле)
const int TYPE_COUNT = 4;
extern const char* const OBJECT_TYPES[TYPE_COUNT];
const int DEPT_COUNT = 5;
extern const char* const DEPARTMENTS[DEPT_COUNT];

// Запись фиксированной длины — пишется в файл целиком
struct AuditObject {
    char invNumber[INV_LEN];      // инвентарный номер (ключ)
    int type;                     // тип объекта (индекс в OBJECT_TYPES)
    int department;               // отдел (индекс в DEPARTMENTS)
    char responsible[NAME_LEN];   // ответственный сотрудник
    char os[OS_LEN];              // операционная система
    bool antivirus;               // антивирус установлен
    Date avUpdate;                // дата обновления антивирусных баз
    bool passwordOk;              // пароль соответствует политике
    bool personalData;            // обрабатываются ПДн жильцов
    Date checkDate;               // дата проверки
    int risk;                     // уровень риска в баллах (считается автоматически)
};

// Расчёт уровня риска в баллах по четырём проверкам
int calcRisk(const AuditObject& obj);
// Класс риска: 0 — низкий (0–2 балла), 1 — средний (3–5), 2 — высокий (6 и более)
int riskClass(int risk);
// Словесный уровень риска: низкий / средний / высокий
std::string riskLevel(int risk);
// Перечень нарушений текстом
std::string violations(const AuditObject& obj);
// Копирование строки в поле фиксированной длины
void setField(char* field, int size, const std::string& value);

#endif
