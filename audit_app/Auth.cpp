// Auth.cpp — регистрация и проверка пользователей
#include "Auth.h"
#include "AuditObject.h"
#include "Exceptions.h"
#include "Sha256.h"
#include <fstream>
#include <random>

Auth::Auth(const std::string& path) : path_(path) {}

// Случайная соль из 16 шестнадцатеричных символов
std::string Auth::makeSalt() {
    static const char hex[] = "0123456789abcdef";
    std::random_device rd;
    std::string salt;
    for (int i = 0; i < SALT_LEN - 1; ++i) salt += hex[rd() % 16];
    return salt;
}

bool Auth::hasUsers() const {
    std::ifstream in(path_, std::ios::binary | std::ios::ate);
    if (!in) return false;
    // Размер файла должен быть кратен размеру записи, иначе файл повреждён
    long size = static_cast<long>(in.tellg());
    if (size % static_cast<long>(sizeof(UserRecord)) != 0) {
        throw FileError("файл пользователей " + path_ + " повреждён, удалите его и запустите программу заново");
    }
    return size > 0;
}

void Auth::registerUser(const std::string& login, const std::string& password) const {
    UserRecord rec{};
    std::string salt = makeSalt();
    setField(rec.login, LOGIN_LEN, login);
    setField(rec.salt, SALT_LEN, salt);
    setField(rec.hash, HASH_LEN, sha256(salt + password));

    std::ofstream out(path_, std::ios::binary | std::ios::app);
    if (!out) throw FileError("не удалось открыть файл пользователей");
    out.write(reinterpret_cast<const char*>(&rec), sizeof(UserRecord));
    if (!out) throw FileError("ошибка записи в файл пользователей");
}

bool Auth::check(const std::string& login, const std::string& password) const {
    std::ifstream in(path_, std::ios::binary);
    if (!in) throw FileError("не найден файл пользователей");
    UserRecord rec{};
    while (in.read(reinterpret_cast<char*>(&rec), sizeof(UserRecord))) {
        if (login == rec.login) {
            // Считаем хэш введённого пароля с солью пользователя и сравниваем
            return sha256(std::string(rec.salt) + password) == rec.hash;
        }
    }
    return false;
}
