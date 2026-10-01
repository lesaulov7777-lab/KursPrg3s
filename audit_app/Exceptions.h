// Exceptions.h — классы исключений программы аудита ИБ
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// Ошибка работы с файлом (не открылся, не записался и т.д.)
class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& msg) : std::runtime_error(msg) {}
};

// Ошибка ввода (конец потока ввода, некорректные данные)
class InputError : public std::runtime_error {
public:
    explicit InputError(const std::string& msg) : std::runtime_error(msg) {}
};

#endif
