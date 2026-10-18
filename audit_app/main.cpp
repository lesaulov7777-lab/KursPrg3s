// main.cpp — точка входа: вход в систему и главное меню
//
// Программа аудита информационной безопасности управляющей компании.
// Сборка: g++ -std=c++17 -Wall *.cpp -o audit

#include <iostream>
#include "Auth.h"
#include "Exceptions.h"
#include "Input.h"
#include "Menu.h"
#include "ObjectFile.h"
#include "Report.h"

#ifdef _WIN32
#include <windows.h>
#endif

const char* const USERS_FILE = "users.dat";      // файл пользователей
const char* const AUDIT_FILE = "audit.dat";    // основной файл объектов
const char* const REPORT_FILE = "report.dat";  // второй файл — отчёт о несоответствиях
const int MAX_ATTEMPTS = 3;                      // попыток входа

// Вход в систему. При первом запуске — регистрация администратора.
static bool login(const Auth& auth) {
    if (!auth.hasUsers()) {
        std::cout << "\nПервый запуск: создайте учётную запись администратора.\n";
        std::string name = readString("Логин: ", 20);
        while (true) {
            std::string p1 = readPassword("Пароль: ");
            std::string p2 = readPassword("Повторите пароль: ");
            if (p1 == p2) {
                auth.registerUser(name, p1);
                std::cout << "Администратор создан.\n";
                break;
            }
            std::cout << "  Ошибка: пароли не совпадают.\n";
        }
    }

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; ++attempt) {
        std::cout << "\n--- Вход в систему ---\n";
        std::string name = readString("Логин: ", 20);
        std::string password = readPassword("Пароль: ");
        if (auth.check(name, password)) {
            std::cout << "Добро пожаловать, " << name << "!\n";
            return true;
        }
        std::cout << "Неверный логин или пароль. Осталось попыток: "
                  << MAX_ATTEMPTS - attempt << '\n';
    }
    return false;
}

// Обработка пунктов главного меню
static void runMenu(const ObjectFile& file, const ReportFile& report) {
    while (true) {
        int choice = mainMenu();
        try {
            switch (choice) {
                case 1: {
                    AuditObject obj = inputObject(file);
                    file.append(obj);
                    std::cout << "\nОбъект сохранён. Уровень риска: " << obj.risk
                              << " (" << riskLevel(obj.risk) << ")\n";
                    break;
                }
                case 2:
                    printObjectsTable(file.readAll());
                    break;
                case 3: {
                    std::string inv = readString("Инвентарный номер: ", 15);
                    AuditObject obj{};
                    if (file.findByInv(inv, obj)) printObjectCard(obj);
                    else std::cout << "Объект с номером " << inv << " не найден.\n";
                    break;
                }
                case 4: editObject(file); break;
                case 5: deleteObject(file); break;
                case 6: makeReport(file, report); break;
                case 7: showReport(report); break;
                case 8: queryObjects(file); break;
                case 0:
                    std::cout << "Работа завершена.\n";
                    return;
            }
        } catch (const FileError& e) {
            // Ошибка файла не завершает программу: сообщаем и возвращаемся в меню
            std::cout << "Ошибка файла: " << e.what() << '\n';
        }
        waitEnter();
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        showSplash();
        Auth auth(USERS_FILE);
        if (!login(auth)) {
            std::cout << "Доступ запрещён.\n";
            return 1;
        }
        ObjectFile file(AUDIT_FILE);
        file.createIfMissing();
        file.verify();
        ReportFile report(REPORT_FILE);
        runMenu(file, report);
    } catch (const InputError&) {
        std::cout << "\nВвод прерван, программа завершена.\n";
        return 1;
    } catch (const std::exception& e) {
        std::cout << "\nКритическая ошибка: " << e.what() << '\n';
        return 2;
    }
    return 0;
}
