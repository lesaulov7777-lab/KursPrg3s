// Menu.h — экранные формы: заставка, меню, таблицы, ввод записи
#ifndef MENU_H
#define MENU_H

#include <vector>
#include "AuditObject.h"
#include "ObjectFile.h"
#include "Report.h"

void showSplash();                                     // заставка программы
int mainMenu();                                        // главное меню, возвращает пункт
void printObjectsTable(const std::vector<AuditObject>& list);  // таблица объектов
void printObjectCard(const AuditObject& obj);          // подробная карточка объекта
AuditObject inputObject(const ObjectFile& file);       // ввод новой записи с клавиатуры
void editObject(const ObjectFile& file);                // изменение полей записи (пункт меню 4)
void deleteObject(const ObjectFile& file);              // удаление записи (пункт меню 5)
void queryObjects(const ObjectFile& file);                // фильтрация и сортировка (пункт 8)
void makeReport(const ObjectFile& file, const ReportFile& report);  // формирование отчёта (пункт 6)
void showReport(const ReportFile& report);              // просмотр отчёта (пункт 7)
void notReady(const char* what);                       // заглушка для пунктов в разработке

#endif
