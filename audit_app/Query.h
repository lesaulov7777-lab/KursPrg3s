// Query.h — фильтрация и сортировка списка объектов аудита
#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include "AuditObject.h"

// Признак, по которому отбираются объекты
enum class FilterKind { None, Department, Type, RiskLevel };
// Поле, по которому сортируется список
enum class SortKind { None, Risk, CheckDate, InvNumber };

struct Filter {
    FilterKind kind;  // вид фильтра
    int value;        // индекс отдела или типа; для уровня риска: 0 — низкий, 1 — средний, 2 — высокий
};

// Подходит ли объект под фильтр
bool matches(const AuditObject& obj, const Filter& filter);

// Возвращает новый список: отобранные по фильтру объекты, отсортированные по выбранному полю.
// descending = true — по убыванию.
std::vector<AuditObject> applyQuery(const std::vector<AuditObject>& source, const Filter& filter,
                                    SortKind sort, bool descending);

#endif
