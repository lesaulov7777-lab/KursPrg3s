// ObjectFile.h — работа с бинарным файлом «Объекты аудита»
#ifndef OBJECT_FILE_H
#define OBJECT_FILE_H

#include <string>
#include <vector>
#include "AuditObject.h"

class ObjectFile {
public:
    explicit ObjectFile(const std::string& path);

    void createIfMissing() const;                       // создать пустой файл, если его нет
    void append(const AuditObject& obj) const;          // добавить запись в конец файла
    std::vector<AuditObject> readAll() const;           // прочитать все записи
    // Поиск по ключу прямо в файле, без массива: true, если найдена
    bool findByInv(const std::string& inv, AuditObject& found) const;
    // Замена записи с заданным ключом прямо в файле (без массива): true, если запись найдена
    bool update(const std::string& inv, const AuditObject& obj) const;
    // Удаление записи с заданным ключом через временный файл: true, если запись найдена
    bool remove(const std::string& inv) const;
    const std::string& path() const { return path_; }   // путь к файлу
    void verify() const;                                // проверка целостности файла (FileError)
    long count() const;                                 // количество записей

private:
    std::string path_;  // путь к файлу
};

#endif
