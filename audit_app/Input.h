// Input.h — защищённый ввод данных с клавиатуры
#ifndef INPUT_H
#define INPUT_H

#include <string>
#include "Date.h"

// Строка непустая, длиной не более maxLen символов
std::string readString(const std::string& prompt, size_t maxLen);
// Целое число в диапазоне [minVal, maxVal]
int readInt(const std::string& prompt, int minVal, int maxVal);
// Ответ да/нет
bool readYesNo(const std::string& prompt);
// Дата в формате ДД.ММ.ГГГГ
Date readDate(const std::string& prompt);
// Пароль: ввод без отображения символов на экране
std::string readPassword(const std::string& prompt);
// Пауза до нажатия Enter
void waitEnter();
// Длина строки UTF-8 в символах (а не в байтах)
size_t utf8Length(const std::string& s);

#endif
