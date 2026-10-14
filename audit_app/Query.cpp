// Query.cpp — реализация фильтрации и сортировки
#include "Query.h"
#include <algorithm>
#include <cstring>

bool matches(const AuditObject& obj, const Filter& filter) {
    switch (filter.kind) {
        case FilterKind::Department: return obj.department == filter.value;
        case FilterKind::Type:       return obj.type == filter.value;
        case FilterKind::RiskLevel:  return riskClass(obj.risk) == filter.value;
        case FilterKind::None:       break;
    }
    return true;
}

std::vector<AuditObject> applyQuery(const std::vector<AuditObject>& source, const Filter& filter,
                                    SortKind sort, bool descending) {
    // Фильтрация: копируем в результат только подходящие объекты
    std::vector<AuditObject> result;
    std::copy_if(source.begin(), source.end(), std::back_inserter(result),
                 [&filter](const AuditObject& obj) { return matches(obj, filter); });

    // Сравнение по выбранному полю; при равенстве порядок сохраняется (stable_sort)
    auto less = [sort](const AuditObject& a, const AuditObject& b) {
        switch (sort) {
            case SortKind::Risk:      return a.risk < b.risk;
            case SortKind::CheckDate: return a.checkDate.toDays() < b.checkDate.toDays();
            case SortKind::InvNumber: return std::strcmp(a.invNumber, b.invNumber) < 0;
            case SortKind::None:      break;
        }
        return false;
    };
    if (sort != SortKind::None) {
        if (descending) {
            std::stable_sort(result.begin(), result.end(),
                             [&less](const AuditObject& a, const AuditObject& b) { return less(b, a); });
        } else {
            std::stable_sort(result.begin(), result.end(), less);
        }
    }
    return result;
}
