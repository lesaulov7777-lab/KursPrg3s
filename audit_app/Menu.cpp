// Menu.cpp — реализация экранных форм
#include "Menu.h"
#include "Input.h"
#include "Query.h"
#include <iostream>

// Вывод текста, дополненного пробелами до ширины width (с учётом UTF-8)
static void cell(const std::string& text, size_t width) {
    std::cout << text;
    size_t len = utf8Length(text);
    for (size_t i = len; i < width; ++i) std::cout << ' ';
    std::cout << "| ";
}

static void line(size_t width) {
    for (size_t i = 0; i < width; ++i) std::cout << "-";
    std::cout << '\n';
}

void showSplash() {
    std::cout << "\n";
    line(64);
    std::cout << "  ПРОГРАММА АУДИТА ИНФОРМАЦИОННОЙ БЕЗОПАСНОСТИ\n"
                 "  УПРАВЛЯЮЩЕЙ КОМПАНИИ\n\n"
                 "  Курсовая работа по дисциплине «Программирование»\n"
                 "  НГТУ, кафедра защиты информации\n";
    line(64);
}

int mainMenu() {
    // Пункты меню; ready = false — функция ещё в разработке
    struct Item { const char* text; bool ready; };
    static const Item items[] = {
        {"Добавить объект аудита", true},
        {"Просмотреть все объекты", true},
        {"Найти объект по инвентарному номеру", true},
        {"Изменить объект", true},
        {"Удалить объект", true},
        {"Сформировать отчёт о несоответствиях", true},
        {"Просмотреть отчёт", true},
        {"Сортировка и фильтрация", true},
    };
    std::cout << "\n=== ГЛАВНОЕ МЕНЮ ===\n";
    int n = 1;
    for (const Item& item : items) {
        std::cout << ' ' << n++ << ". " << item.text;
        if (!item.ready) {
            for (size_t i = utf8Length(item.text); i < 38; ++i) std::cout << ' ';
            std::cout << "[в разработке]";
        }
        std::cout << '\n';
    }
    std::cout << " 0. Выход\n";
    return readInt("Выберите пункт: ", 0, 8);
}

void printObjectsTable(const std::vector<AuditObject>& list) {
    if (list.empty()) {
        std::cout << "\nБаза объектов пуста.\n";
        return;
    }
    const size_t W = 113;
    std::cout << '\n';
    line(W);
    std::cout << "| ";
    cell("№", 3); cell("Инв. номер", 12); cell("Тип", 20); cell("Отдел", 18);
    cell("Ответственный", 20); cell("Проверка", 11); cell("Риск", 12);
    std::cout << '\n';
    line(W);
    int n = 1;
    for (const AuditObject& obj : list) {
        std::cout << "| ";
        cell(std::to_string(n++), 3);
        cell(obj.invNumber, 12);
        cell(OBJECT_TYPES[obj.type], 20);
        cell(DEPARTMENTS[obj.department], 18);
        cell(obj.responsible, 20);
        cell(obj.checkDate.toString(), 11);
        cell(std::to_string(obj.risk) + " " + riskLevel(obj.risk), 12);
        std::cout << '\n';
    }
    line(W);
    std::cout << "Всего объектов: " << list.size() << '\n';
}

void printObjectCard(const AuditObject& obj) {
    std::cout << "\nИнвентарный номер:          " << obj.invNumber
              << "\nТип объекта:                " << OBJECT_TYPES[obj.type]
              << "\nОтдел:                      " << DEPARTMENTS[obj.department]
              << "\nОтветственный:              " << obj.responsible
              << "\nОперационная система:       " << obj.os
              << "\nАнтивирус:                  " << (obj.antivirus ? "установлен" : "нет");
    if (obj.antivirus) {
        std::cout << "\nОбновление баз:             " << obj.avUpdate.toString();
    }
    std::cout << "\nПароль по политике:         " << (obj.passwordOk ? "да" : "нет")
              << "\nОбработка ПДн жильцов:      " << (obj.personalData ? "да" : "нет")
              << "\nДата проверки:              " << obj.checkDate.toString()
              << "\nУровень риска:              " << obj.risk << " (" << riskLevel(obj.risk) << ")"
              << "\nНарушения:                  " << violations(obj) << '\n';
}

// Выбор значения из справочника, возвращает индекс
static int chooseFromList(const char* title, const char* const items[], int count) {
    std::cout << title << ":\n";
    for (int i = 0; i < count; ++i) std::cout << "  " << i + 1 << ". " << items[i] << '\n';
    return readInt("Номер: ", 1, count) - 1;
}

AuditObject inputObject(const ObjectFile& file) {
    AuditObject obj{};
    std::cout << "\n--- Новый объект аудита ---\n";

    // Ключ должен быть уникальным: проверяем поиском по файлу
    while (true) {
        std::string inv = readString("Инвентарный номер (например, УК-ПК-001): ", 15);
        AuditObject existing{};
        if (file.findByInv(inv, existing)) {
            std::cout << "  Ошибка: объект с таким номером уже есть.\n";
            continue;
        }
        setField(obj.invNumber, INV_LEN, inv);
        break;
    }
    obj.type = chooseFromList("Тип объекта", OBJECT_TYPES, TYPE_COUNT);
    obj.department = chooseFromList("Отдел", DEPARTMENTS, DEPT_COUNT);
    setField(obj.responsible, NAME_LEN, readString("Ответственный (Фамилия И. О.): ", 40));
    setField(obj.os, OS_LEN, readString("Операционная система: ", 30));

    obj.checkDate = readDate("Дата проверки");
    obj.antivirus = readYesNo("Антивирус установлен?");
    if (obj.antivirus) {
        // Базы не могут быть обновлены позже даты проверки
        while (true) {
            obj.avUpdate = readDate("Дата обновления антивирусных баз");
            if (daysBetween(obj.avUpdate, obj.checkDate) >= 0) break;
            std::cout << "  Ошибка: дата обновления позже даты проверки.\n";
        }
    } else {
        obj.avUpdate = obj.checkDate;
    }
    obj.passwordOk = readYesNo("Пароль соответствует парольной политике?");
    obj.personalData = readYesNo("Обрабатываются персональные данные жильцов?");

    obj.risk = calcRisk(obj);
    return obj;
}

// Ввод даты обновления баз: не позже даты проверки
static Date inputAvUpdate(const Date& checkDate) {
    while (true) {
        Date d = readDate("Дата обновления антивирусных баз");
        if (daysBetween(d, checkDate) >= 0) return d;
        std::cout << "  Ошибка: дата обновления позже даты проверки.\n";
    }
}

void editObject(const ObjectFile& file) {
    std::string inv = readString("Инвентарный номер изменяемого объекта: ", 15);
    AuditObject obj{};
    if (!file.findByInv(inv, obj)) {
        std::cout << "Объект с номером " << inv << " не найден.\n";
        return;
    }

    // Правим копию записи, в файл она попадёт только после «Сохранить»
    while (true) {
        printObjectCard(obj);
        std::cout << "\nЧто изменить:\n"
                     " 1. Тип объекта            6. Дата обновления баз\n"
                     " 2. Отдел                  7. Парольная политика\n"
                     " 3. Ответственный          8. Обработка ПДн\n"
                     " 4. Операционная система   9. Дата проверки\n"
                     " 5. Антивирус\n"
                     " 0. Сохранить и выйти     10. Отменить без сохранения\n";
        int choice = readInt("Выберите пункт: ", 0, 10);
        switch (choice) {
            case 1: obj.type = chooseFromList("Тип объекта", OBJECT_TYPES, TYPE_COUNT); break;
            case 2: obj.department = chooseFromList("Отдел", DEPARTMENTS, DEPT_COUNT); break;
            case 3: setField(obj.responsible, NAME_LEN, readString("Ответственный (Фамилия И. О.): ", 40)); break;
            case 4: setField(obj.os, OS_LEN, readString("Операционная система: ", 30)); break;
            case 5:
                obj.antivirus = readYesNo("Антивирус установлен?");
                // Дата баз имеет смысл только при установленном антивирусе
                obj.avUpdate = obj.antivirus ? inputAvUpdate(obj.checkDate) : obj.checkDate;
                break;
            case 6:
                if (obj.antivirus) obj.avUpdate = inputAvUpdate(obj.checkDate);
                else std::cout << "  Антивирус не установлен, дата баз не используется.\n";
                break;
            case 7: obj.passwordOk = readYesNo("Пароль соответствует парольной политике?"); break;
            case 8: obj.personalData = readYesNo("Обрабатываются персональные данные жильцов?"); break;
            case 9:
                obj.checkDate = readDate("Дата проверки");
                // Дата баз не может быть позже новой даты проверки
                if (obj.antivirus && daysBetween(obj.avUpdate, obj.checkDate) < 0) {
                    std::cout << "  Дата обновления баз позже даты проверки, введите её заново.\n";
                    obj.avUpdate = inputAvUpdate(obj.checkDate);
                }
                break;
            case 0:
                obj.risk = calcRisk(obj);  // риск зависит от полей, пересчитываем
                if (!file.update(inv, obj)) {
                    std::cout << "Ошибка: объект " << inv << " исчез из файла.\n";
                    return;
                }
                std::cout << "\nИзменения сохранены. Уровень риска: " << obj.risk
                          << " (" << riskLevel(obj.risk) << ")\n";
                return;
            case 10:
                std::cout << "Изменения отменены.\n";
                return;
        }
        obj.risk = calcRisk(obj);  // показываем актуальный риск в карточке
    }
}

void deleteObject(const ObjectFile& file) {
    std::string inv = readString("Инвентарный номер удаляемого объекта: ", 15);
    AuditObject obj{};
    if (!file.findByInv(inv, obj)) {
        std::cout << "Объект с номером " << inv << " не найден.\n";
        return;
    }
    printObjectCard(obj);
    if (!readYesNo("\nУдалить этот объект безвозвратно?")) {
        std::cout << "Удаление отменено.\n";
        return;
    }
    file.remove(inv);
    std::cout << "Объект удалён.\n";
}

void queryObjects(const ObjectFile& file) {
    std::vector<AuditObject> all = file.readAll();
    if (all.empty()) {
        std::cout << "\nБаза объектов пуста.\n";
        return;
    }

    std::cout << "\n--- Фильтрация и сортировка ---\n"
                 "Фильтр:\n"
                 "  1. Без фильтра\n"
                 "  2. По отделу\n"
                 "  3. По типу объекта\n"
                 "  4. По уровню риска\n";
    Filter filter{FilterKind::None, 0};
    switch (readInt("Номер: ", 1, 4)) {
        case 2:
            filter = {FilterKind::Department, chooseFromList("Отдел", DEPARTMENTS, DEPT_COUNT)};
            break;
        case 3:
            filter = {FilterKind::Type, chooseFromList("Тип объекта", OBJECT_TYPES, TYPE_COUNT)};
            break;
        case 4: {
            static const char* const levels[] = {"низкий", "средний", "высокий"};
            filter = {FilterKind::RiskLevel, chooseFromList("Уровень риска", levels, 3)};
            break;
        }
    }

    std::cout << "\nСортировка:\n"
                 "  1. Без сортировки (порядок в файле)\n"
                 "  2. По уровню риска\n"
                 "  3. По дате проверки\n"
                 "  4. По инвентарному номеру\n";
    static const SortKind sorts[] = {SortKind::None, SortKind::Risk, SortKind::CheckDate, SortKind::InvNumber};
    SortKind sort = sorts[readInt("Номер: ", 1, 4) - 1];
    bool descending = false;
    if (sort != SortKind::None) {
        std::cout << "Порядок:\n  1. По возрастанию\n  2. По убыванию\n";
        descending = readInt("Номер: ", 1, 2) == 2;
    }

    std::vector<AuditObject> result = applyQuery(all, filter, sort, descending);
    if (result.empty()) std::cout << "\nНи один объект не подходит под фильтр.\n";
    else printObjectsTable(result);
}

void makeReport(const ObjectFile& file, const ReportFile& report) {
    std::cout << "\n--- Отчёт о несоответствиях ---\n"
                 "Какие объекты включить в отчёт:\n"
                 "  1. Все с нарушениями (риск от 1 балла)\n"
                 "  2. Со средним и высоким риском (от 3 баллов)\n"
                 "  3. Только с высоким риском (от 6 баллов)\n";
    static const int thresholds[] = {1, 3, 6};
    int minRisk = thresholds[readInt("Номер: ", 1, 3) - 1];

    long count = report.build(file, minRisk);
    std::cout << "\nОтчёт сформирован (файл report.dat). Объектов в отчёте: " << count << ".\n";
    if (count == 0) std::cout << "Объектов с таким уровнем риска нет.\n";
}

void showReport(const ReportFile& report) {
    if (!report.exists()) {
        std::cout << "\nОтчёт ещё не сформирован. Используйте пункт 6.\n";
        return;
    }
    std::vector<ReportRecord> list = report.readAll();
    if (list.empty()) {
        std::cout << "\nВ отчёте нет объектов.\n";
        return;
    }
    const size_t W = 85;
    std::cout << "\nОТЧЁТ О НЕСООТВЕТСТВИЯХ\n";
    line(W);
    std::cout << "| ";
    cell("№", 3); cell("Инв. номер", 12); cell("Отдел", 18); cell("Ответственный", 20); cell("Риск", 12);
    std::cout << '\n';
    line(W);
    int n = 1;
    for (const ReportRecord& rec : list) {
        std::cout << "| ";
        cell(std::to_string(n++), 3);
        cell(rec.invNumber, 12);
        cell(DEPARTMENTS[rec.department], 18);
        cell(rec.responsible, 20);
        cell(std::to_string(rec.risk) + " " + riskLevel(rec.risk), 12);
        std::cout << "\n|     Нарушения: " << rec.violations << '\n';
        line(W);
    }
    std::cout << "Объектов в отчёте: " << list.size() << '\n';
}

void notReady(const char* what) {
    std::cout << "\nФункция «" << what << "» находится в разработке.\n";
}
