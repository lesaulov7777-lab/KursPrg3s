// Report.cpp — формирование и чтение отчёта о несоответствиях
#include "Report.h"
#include "Exceptions.h"
#include <fstream>

ReportFile::ReportFile(const std::string& path) : path_(path) {}

long ReportFile::build(const ObjectFile& source, int minRisk) const {
    std::ifstream in(source.path(), std::ios::binary);
    if (!in) throw FileError("не удалось открыть файл " + source.path());
    std::ofstream out(path_, std::ios::binary | std::ios::trunc);
    if (!out) throw FileError("не удалось создать файл " + path_);

    long count = 0;
    AuditObject obj{};
    // Читаем основной файл по одной записи, массив не нужен
    while (in.read(reinterpret_cast<char*>(&obj), sizeof(AuditObject))) {
        if (obj.risk < minRisk) continue;
        ReportRecord rec{};
        setField(rec.invNumber, INV_LEN, obj.invNumber);
        rec.department = obj.department;
        setField(rec.responsible, NAME_LEN, obj.responsible);
        setField(rec.violations, VIOL_LEN, violations(obj));
        rec.risk = obj.risk;
        out.write(reinterpret_cast<const char*>(&rec), sizeof(ReportRecord));
        ++count;
    }
    out.flush();
    if (!out) throw FileError("ошибка записи в файл " + path_);
    return count;
}

bool ReportFile::exists() const {
    std::ifstream in(path_, std::ios::binary);
    return static_cast<bool>(in);
}

std::vector<ReportRecord> ReportFile::readAll() const {
    std::ifstream in(path_, std::ios::binary | std::ios::ate);
    if (!in) throw FileError("не удалось открыть файл " + path_);
    // Размер файла должен быть кратен размеру записи, иначе файл повреждён
    if (static_cast<long>(in.tellg()) % static_cast<long>(sizeof(ReportRecord)) != 0) {
        throw FileError("файл " + path_ + " повреждён (неполная запись)");
    }
    in.seekg(0);
    std::vector<ReportRecord> list;
    ReportRecord rec{};
    while (in.read(reinterpret_cast<char*>(&rec), sizeof(ReportRecord))) {
        list.push_back(rec);
    }
    return list;
}
