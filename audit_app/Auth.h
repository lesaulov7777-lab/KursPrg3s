// Auth.h — учётные записи и вход в систему по хэшу пароля
#ifndef AUTH_H
#define AUTH_H

#include <string>

const int LOGIN_LEN = 32;
const int SALT_LEN = 17;   // 16 шестнадцатеричных символов + '\0'
const int HASH_LEN = 65;   // 64 шестнадцатеричных символа + '\0'

// Запись бинарного файла пользователей. Сам пароль не хранится.
struct UserRecord {
    char login[LOGIN_LEN];
    char salt[SALT_LEN];   // случайная соль
    char hash[HASH_LEN];   // SHA-256(соль + пароль)
};

class Auth {
public:
    explicit Auth(const std::string& path);

    // Есть ли хотя бы один пользователь; FileError, если файл повреждён
    bool hasUsers() const;
    void registerUser(const std::string& login, const std::string& password) const;
    bool check(const std::string& login, const std::string& password) const;

private:
    std::string path_;
    static std::string makeSalt();
};

#endif
