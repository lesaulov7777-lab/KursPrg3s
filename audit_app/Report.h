// Report.h — второй бинарный файл «Отчёт о несоответствиях»
#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>
#include "AuditObject.h"
#include "ObjectFile.h"

const int VIOL_LEN = 256;  // длина поля с перечнем нарушений в байтах

// Запись отчёта: объект с нарушениями
struct ReportRecord {
    char invNumber[INV_LEN];      // инвентарный номер
    int department;               // отдел (индекс в DEPARTMENTS)
    char responsible[NAME_LEN];   // ответственный сотрудник
    char violations[VIOL_LEN];    // нарушения текстом
    int risk;                     // уровень риска в баллах
};

class ReportFile {
public:
    explicit ReportFile(const std::string& path);

    // Формирует отчёт из основного файла: в него попадают объекты с риском не ниже minRisk.
    // Основной файл читается по одной записи. Возвращает число записей в отчёте.
    long build(const ObjectFile& source, int minRisk) const;
    std::vector<ReportRecord> readAll() const;  // прочитать отчёт (FileError, если файла нет или он повреждён)
    bool exists() const;                        // создан ли отчёт

private:
    std::string path_;
};

#endif
