// Sha256.h — хэш-функция SHA-256 (FIPS 180-4)
#ifndef SHA256_H
#define SHA256_H

#include <string>

// Возвращает хэш SHA-256 строки в виде 64 шестнадцатеричных символов
std::string sha256(const std::string& data);

#endif
