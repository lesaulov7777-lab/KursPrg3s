// Input.cpp — реализация защищённого ввода
#include "Input.h"
#include "Exceptions.h"
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// Чтение строки; при конце ввода (Ctrl+D / Ctrl+Z) — исключение
static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw InputError("ввод прерван");
    }
    // Убираем пробелы и символ '\r' по краям
    size_t begin = line.find_first_not_of(" \t\r");
    size_t end = line.find_last_not_of(" \t\r");
    if (begin == std::string::npos) return "";
    return line.substr(begin, end - begin + 1);
}

size_t utf8Length(const std::string& s) {
    size_t count = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80) count++;  // считаем только первые байты символов
    }
    return count;
}

std::string readString(const std::string& prompt, size_t maxLen) {
    while (true) {
        std::string s = readLine(prompt);
        if (s.empty()) {
            std::cout << "  Ошибка: поле не может быть пустым.\n";
        } else if (utf8Length(s) > maxLen) {
            std::cout << "  Ошибка: не более " << maxLen << " символов.\n";
        } else {
            return s;
        }
    }
}

int readInt(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::string s = readLine(prompt);
        try {
            size_t pos = 0;
            int value = std::stoi(s, &pos);
            if (pos == s.size() && value >= minVal && value <= maxVal) return value;
        } catch (const std::exception&) {
            // не число — сообщение ниже
        }
        std::cout << "  Ошибка: введите целое число от " << minVal << " до " << maxVal << ".\n";
    }
}

bool readYesNo(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt + " (д/н): ");
        if (s == "д" || s == "Д" || s == "y" || s == "Y" || s == "да") return true;
        if (s == "н" || s == "Н" || s == "n" || s == "N" || s == "нет") return false;
        std::cout << "  Ошибка: введите «д» или «н».\n";
    }
}

Date readDate(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt + " (ДД.ММ.ГГГГ): ");
        Date d{};
        if (Date::parse(s, d)) return d;
        std::cout << "  Ошибка: неверная дата.\n";
    }
}

std::string readPassword(const std::string& prompt) {
    while (true) {
        std::cout << prompt << std::flush;
        std::string password;
#ifdef _WIN32
        // Windows: читаем символы по одному, вместо них печатаем '*'
        int ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!password.empty()) { password.pop_back(); std::cout << "\b \b"; }
            } else {
                password += static_cast<char>(ch);
                std::cout << '*';
            }
        }
        std::cout << '\n';
#else
        // macOS / Linux: временно отключаем эхо терминала
        termios oldt{};
        bool isTerminal = tcgetattr(STDIN_FILENO, &oldt) == 0;
        if (isTerminal) {
            termios newt = oldt;
            newt.c_lflag &= ~ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        }
        bool ok = static_cast<bool>(std::getline(std::cin, password));
        if (isTerminal) {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            std::cout << '\n';
        }
        if (!ok) throw InputError("ввод прерван");
        if (!password.empty() && password.back() == '\r') password.pop_back();
#endif
        if (!password.empty()) return password;
        std::cout << "  Ошибка: пароль не может быть пустым.\n";
    }
}

void waitEnter() {
    std::cout << "\nНажмите Enter, чтобы продолжить...";
    std::string dummy;
    if (!std::getline(std::cin, dummy)) throw InputError("ввод прерван");
}
