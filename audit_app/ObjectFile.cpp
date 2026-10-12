// ObjectFile.cpp — реализация работы с бинарным файлом объектов
#include "ObjectFile.h"
#include "Exceptions.h"
#include <cstdio>
#include <cstring>
#include <fstream>

ObjectFile::ObjectFile(const std::string& path) : path_(path) {}

void ObjectFile::createIfMissing() const {
    std::ifstream test(path_, std::ios::binary);
    if (test) return;
    std::ofstream out(path_, std::ios::binary);
    if (!out) throw FileError("не удалось создать файл " + path_);
}

void ObjectFile::append(const AuditObject& obj) const {
    std::ofstream out(path_, std::ios::binary | std::ios::app);
    if (!out) throw FileError("не удалось открыть файл " + path_ + " для записи");
    out.write(reinterpret_cast<const char*>(&obj), sizeof(AuditObject));
    if (!out) throw FileError("ошибка записи в файл " + path_);
}

std::vector<AuditObject> ObjectFile::readAll() const {
    std::ifstream in(path_, std::ios::binary);
    if (!in) throw FileError("не удалось открыть файл " + path_);
    std::vector<AuditObject> list;
    AuditObject obj{};
    while (in.read(reinterpret_cast<char*>(&obj), sizeof(AuditObject))) {
        list.push_back(obj);
    }
    return list;
}

bool ObjectFile::findByInv(const std::string& inv, AuditObject& found) const {
    std::ifstream in(path_, std::ios::binary);
    if (!in) throw FileError("не удалось открыть файл " + path_);
    AuditObject obj{};
    // Читаем по одной записи, массив не используется
    while (in.read(reinterpret_cast<char*>(&obj), sizeof(AuditObject))) {
        if (inv == obj.invNumber) {
            found = obj;
            return true;
        }
    }
    return false;
}

long ObjectFile::count() const {
    std::ifstream in(path_, std::ios::binary | std::ios::ate);
    if (!in) throw FileError("не удалось открыть файл " + path_);
    return static_cast<long>(in.tellg()) / static_cast<long>(sizeof(AuditObject));
}

void ObjectFile::verify() const {
    std::ifstream in(path_, std::ios::binary | std::ios::ate);
    if (!in) throw FileError("не удалось открыть файл " + path_);
    // Размер файла должен быть кратен размеру записи, иначе файл повреждён
    if (static_cast<long>(in.tellg()) % static_cast<long>(sizeof(AuditObject)) != 0) {
        throw FileError("файл " + path_ + " повреждён (неполная запись)");
    }
}

bool ObjectFile::update(const std::string& inv, const AuditObject& obj) const {
    std::fstream file(path_, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) throw FileError("не удалось открыть файл " + path_);
    AuditObject current{};
    // Ищем запись по ключу, запоминая позицию её начала
    while (true) {
        std::streampos pos = file.tellg();
        if (!file.read(reinterpret_cast<char*>(&current), sizeof(AuditObject))) return false;
        if (inv == current.invNumber) {
            file.seekp(pos);  // возвращаемся к началу найденной записи и перезаписываем её
            file.write(reinterpret_cast<const char*>(&obj), sizeof(AuditObject));
            file.flush();
            if (!file) throw FileError("ошибка записи в файл " + path_);
            return true;
        }
    }
}

bool ObjectFile::remove(const std::string& inv) const {
    const std::string tmpPath = path_ + ".tmp";
    bool found = false;
    {
        std::ifstream in(path_, std::ios::binary);
        if (!in) throw FileError("не удалось открыть файл " + path_);
        std::ofstream out(tmpPath, std::ios::binary | std::ios::trunc);
        if (!out) throw FileError("не удалось создать временный файл " + tmpPath);
        AuditObject obj{};
        // Переписываем во временный файл все записи, кроме удаляемой
        while (in.read(reinterpret_cast<char*>(&obj), sizeof(AuditObject))) {
            if (inv == obj.invNumber) {
                found = true;
                continue;
            }
            out.write(reinterpret_cast<const char*>(&obj), sizeof(AuditObject));
        }
        out.flush();
        if (!out) throw FileError("ошибка записи во временный файл " + tmpPath);
    }
    if (!found) {
        std::remove(tmpPath.c_str());
        return false;
    }
    // Подменяем исходный файл временным
    if (std::remove(path_.c_str()) != 0 || std::rename(tmpPath.c_str(), path_.c_str()) != 0) {
        throw FileError("не удалось заменить файл " + path_);
    }
    return true;
}
