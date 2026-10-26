// server.cpp — веб-интерфейс программы аудита ИБ: локальный HTTP-сервер на C++
//
// Окно программы открывается в браузере по адресу http://127.0.0.1:8080
// Данные хранятся в тех же бинарных файлах, что и у консольной версии.
// Сборка: make web

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "httplib.h"
#include "Auth.h"
#include "AuditObject.h"
#include "Exceptions.h"
#include "ObjectFile.h"
#include "Query.h"
#include "Report.h"
#include "Page.h"

static const char* const USERS_FILE = "users.dat";
static const char* const AUDIT_FILE = "audit.dat";
static const char* const REPORT_FILE = "report.dat";
static const int PORT = 8080;
static const int MAX_ATTEMPTS = 3;       // неверных входов подряд до блокировки
static const int LOCK_SECONDS = 30;      // длительность блокировки входа

static std::mutex g_mutex;               // файлы обслуживаются по одному запросу за раз
static std::set<std::string> g_sessions; // действующие сессии (случайные токены)
static int g_failed = 0;                 // неверных входов подряд
static std::time_t g_lockUntil = 0;      // до какого момента вход заблокирован

// ---------- вспомогательные функции ----------

// Экранирование строки для JSON
static std::string js(const std::string& s) {
    std::string out = "\"";
    for (unsigned char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) { char b[8]; std::snprintf(b, sizeof b, "\\u%04x", c); out += b; }
                else out += static_cast<char>(c);
        }
    }
    return out + "\"";
}

static std::string jb(bool v) { return v ? "true" : "false"; }

// Длина строки UTF-8 в символах
static size_t utf8Len(const std::string& s) {
    size_t n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++n;
    return n;
}

// Приведение к нижнему регистру для латиницы и кириллицы (UTF-8), нужно для поиска
static std::string lower(const std::string& s) {
    std::string r;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = s[i];
        if (c >= 'A' && c <= 'Z') r += static_cast<char>(c + 32);
        else if (c == 0xD0 && i + 1 < s.size()) {
            unsigned char d = s[i + 1];
            if (d >= 0x90 && d <= 0x9F) { r += static_cast<char>(0xD0); r += static_cast<char>(d + 0x20); }
            else if (d >= 0xA0 && d <= 0xAF) { r += static_cast<char>(0xD1); r += static_cast<char>(d - 0x20); }
            else if (d == 0x81) { r += static_cast<char>(0xD1); r += static_cast<char>(0x91); }
            else { r += static_cast<char>(c); r += static_cast<char>(d); }
            ++i;
        } else r += static_cast<char>(c);
    }
    return r;
}

static std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

static std::string newToken() {
    static const char hex[] = "0123456789abcdef";
    std::random_device rd;
    std::string t;
    for (int i = 0; i < 32; ++i) t += hex[rd() % 16];
    return t;
}

// Ошибка проверки данных (возвращается клиенту с кодом 400)
struct BadRequest {
    std::string text;
};

// ---------- проверка полей и преобразование записи ----------

static const int AV_MAX_AGE = 7;
static bool avOutdated(const AuditObject& o) {
    return o.antivirus && daysBetween(o.avUpdate, o.checkDate) > AV_MAX_AGE;
}

static int intParam(const httplib::Request& req, const char* name, int lo, int hi, const char* label) {
    std::string v = trim(req.get_param_value(name));
    char* end = nullptr;
    long n = std::strtol(v.c_str(), &end, 10);
    if (v.empty() || *end != '\0' || n < lo || n > hi) throw BadRequest{std::string("Некорректное поле: ") + label};
    return static_cast<int>(n);
}

static std::string textParam(const httplib::Request& req, const char* name, size_t maxLen, const char* label) {
    std::string v = trim(req.get_param_value(name));
    if (v.empty()) throw BadRequest{std::string("Поле «") + label + "» не может быть пустым"};
    if (utf8Len(v) > maxLen)
        throw BadRequest{std::string("Поле «") + label + "»: не более " + std::to_string(maxLen) + " символов"};
    return v;
}

static Date dateParam(const httplib::Request& req, const char* name, const char* label) {
    Date d{};
    if (!Date::parse(trim(req.get_param_value(name)), d))
        throw BadRequest{std::string("Поле «") + label + "»: неверная дата (формат ДД.ММ.ГГГГ, 2000–2100 годы)"};
    return d;
}

// Собирает запись из параметров запроса. Ключ (инвентарный номер) проверяется отдельно.
static AuditObject objectFromRequest(const httplib::Request& req, const std::string& inv) {
    AuditObject o{};
    setField(o.invNumber, INV_LEN, inv);
    o.type = intParam(req, "type", 0, TYPE_COUNT - 1, "тип объекта");
    o.department = intParam(req, "department", 0, DEPT_COUNT - 1, "отдел");
    setField(o.responsible, NAME_LEN, textParam(req, "responsible", 40, "Ответственный"));
    setField(o.os, OS_LEN, textParam(req, "os", 30, "Операционная система"));
    o.checkDate = dateParam(req, "checkDate", "Дата проверки");
    o.antivirus = req.get_param_value("antivirus") == "1";
    if (o.antivirus) {
        o.avUpdate = dateParam(req, "avUpdate", "Дата обновления баз");
        if (daysBetween(o.avUpdate, o.checkDate) < 0)
            throw BadRequest{"Дата обновления баз не может быть позже даты проверки"};
    } else {
        o.avUpdate = o.checkDate;
    }
    o.passwordOk = req.get_param_value("passwordOk") == "1";
    o.personalData = req.get_param_value("personalData") == "1";
    o.risk = calcRisk(o);
    return o;
}

// ---------- запись в JSON ----------

static std::string objectJson(const AuditObject& o) {
    std::ostringstream s;
    s << "{\"inv\":" << js(o.invNumber)
      << ",\"type\":" << o.type << ",\"typeName\":" << js(OBJECT_TYPES[o.type])
      << ",\"department\":" << o.department << ",\"departmentName\":" << js(DEPARTMENTS[o.department])
      << ",\"responsible\":" << js(o.responsible) << ",\"os\":" << js(o.os)
      << ",\"antivirus\":" << jb(o.antivirus) << ",\"avUpdate\":" << js(o.avUpdate.toString())
      << ",\"passwordOk\":" << jb(o.passwordOk) << ",\"personalData\":" << jb(o.personalData)
      << ",\"checkDate\":" << js(o.checkDate.toString())
      << ",\"risk\":" << o.risk << ",\"riskClass\":" << riskClass(o.risk)
      << ",\"level\":" << js(riskLevel(o.risk)) << ",\"violations\":" << js(violations(o)) << "}";
    return s.str();
}

static std::string listJson(const std::vector<AuditObject>& list) {
    std::string s = "[";
    for (size_t i = 0; i < list.size(); ++i) s += (i ? "," : "") + objectJson(list[i]);
    return s + "]";
}

static void sendJson(httplib::Response& res, int status, const std::string& body) {
    res.status = status;
    res.set_content(body, "application/json; charset=utf-8");
    res.set_header("Cache-Control", "no-store");
}

static void sendError(httplib::Response& res, int status, const std::string& text) {
    sendJson(res, status, "{\"error\":" + js(text) + "}");
}

// ---------- сессии ----------

static std::string sessionToken(const httplib::Request& req) {
    std::string cookie = req.get_header_value("Cookie");
    size_t p = cookie.find("sid=");
    if (p == std::string::npos) return "";
    size_t e = cookie.find(';', p);
    return cookie.substr(p + 4, e == std::string::npos ? std::string::npos : e - p - 4);
}

static bool loggedIn(const httplib::Request& req) {
    std::string t = sessionToken(req);
    return !t.empty() && g_sessions.count(t) > 0;
}

// Оборачивает обработчик: проверяет вход, блокирует файлы, превращает исключения в ответы
template <class F>
static httplib::Server::Handler api(bool needLogin, F fn) {
    return [needLogin, fn](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        try {
            if (needLogin && !loggedIn(req)) return sendError(res, 401, "Требуется вход в систему");
            fn(req, res);
        } catch (const BadRequest& e) {
            sendError(res, 400, e.text);
        } catch (const FileError& e) {
            sendError(res, 500, std::string("Ошибка файла: ") + e.what());
        } catch (const std::exception& e) {
            sendError(res, 500, std::string("Внутренняя ошибка: ") + e.what());
        }
    };
}

// ---------- запуск ----------

int main() {
    Auth auth(USERS_FILE);
    ObjectFile file(AUDIT_FILE);
    ReportFile report(REPORT_FILE);
    try {
        file.createIfMissing();
        file.verify();
        auth.hasUsers();  // проверка целостности файла пользователей
    } catch (const std::exception& e) {
        std::cout << "Критическая ошибка: " << e.what() << '\n';
        return 2;
    }

    httplib::Server srv;

    srv.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(PAGE_HTML, "text/html; charset=utf-8");
    });

    // Состояние: есть ли администратор, выполнен ли вход, справочники
    srv.Get("/api/state", api(false, [&](const httplib::Request& req, httplib::Response& res) {
        std::string types = "[", depts = "[";
        for (int i = 0; i < TYPE_COUNT; ++i) types += (i ? "," : "") + js(OBJECT_TYPES[i]);
        for (int i = 0; i < DEPT_COUNT; ++i) depts += (i ? "," : "") + js(DEPARTMENTS[i]);
        sendJson(res, 200, "{\"hasUsers\":" + jb(auth.hasUsers()) + ",\"loggedIn\":" + jb(loggedIn(req)) +
                           ",\"types\":" + types + "],\"departments\":" + depts + "]}");
    }));

    // Создание администратора при первом запуске
    srv.Post("/api/register", api(false, [&](const httplib::Request& req, httplib::Response& res) {
        if (auth.hasUsers()) return sendError(res, 403, "Администратор уже создан");
        std::string login = trim(req.get_param_value("login"));
        std::string pass = req.get_param_value("password");
        if (login.empty() || utf8Len(login) > 20) throw BadRequest{"Логин: от 1 до 20 символов"};
        if (pass.size() < 4) throw BadRequest{"Пароль: не короче 4 символов"};
        if (pass != req.get_param_value("password2")) throw BadRequest{"Пароли не совпадают"};
        auth.registerUser(login, pass);
        std::string t = newToken();
        g_sessions.insert(t);
        res.set_header("Set-Cookie", "sid=" + t + "; HttpOnly; SameSite=Strict; Path=/");
        sendJson(res, 200, "{\"ok\":true}");
    }));

    // Вход: проверка хеша пароля; после трёх неверных попыток вход блокируется на 30 секунд
    srv.Post("/api/login", api(false, [&](const httplib::Request& req, httplib::Response& res) {
        std::time_t now = std::time(nullptr);
        if (now < g_lockUntil)
            return sendError(res, 429, "Слишком много попыток. Повторите через " +
                                           std::to_string(g_lockUntil - now) + " с");
        if (auth.check(trim(req.get_param_value("login")), req.get_param_value("password"))) {
            g_failed = 0;
            std::string t = newToken();
            g_sessions.insert(t);
            res.set_header("Set-Cookie", "sid=" + t + "; HttpOnly; SameSite=Strict; Path=/");
            return sendJson(res, 200, "{\"ok\":true}");
        }
        if (++g_failed >= MAX_ATTEMPTS) {
            g_failed = 0;
            g_lockUntil = now + LOCK_SECONDS;
            return sendError(res, 429, "Три неверные попытки. Вход заблокирован на 30 с");
        }
        sendError(res, 401, "Неверный логин или пароль. Осталось попыток: " + std::to_string(MAX_ATTEMPTS - g_failed));
    }));

    srv.Post("/api/logout", api(false, [&](const httplib::Request& req, httplib::Response& res) {
        g_sessions.erase(sessionToken(req));
        res.set_header("Set-Cookie", "sid=; Max-Age=0; Path=/");
        sendJson(res, 200, "{\"ok\":true}");
    }));

    // Список объектов с фильтром, сортировкой и поиском по подстроке
    srv.Get("/api/objects", api(true, [&](const httplib::Request& req, httplib::Response& res) {
        std::string fk = req.get_param_value("filter");
        Filter filter{FilterKind::None, 0};
        if (fk == "dept") filter = {FilterKind::Department, intParam(req, "value", 0, DEPT_COUNT - 1, "отдел")};
        else if (fk == "type") filter = {FilterKind::Type, intParam(req, "value", 0, TYPE_COUNT - 1, "тип")};
        else if (fk == "risk") filter = {FilterKind::RiskLevel, intParam(req, "value", 0, 2, "уровень риска")};
        std::string sk = req.get_param_value("sort");
        SortKind sort = sk == "risk" ? SortKind::Risk : sk == "date" ? SortKind::CheckDate
                      : sk == "inv" ? SortKind::InvNumber : SortKind::None;
        bool desc = req.get_param_value("desc") == "1";
        std::vector<AuditObject> list = applyQuery(file.readAll(), filter, sort, desc);
        std::string q = lower(trim(req.get_param_value("q")));
        if (!q.empty()) {
            list.erase(std::remove_if(list.begin(), list.end(), [&q](const AuditObject& o) {
                return lower(o.invNumber).find(q) == std::string::npos &&
                       lower(o.responsible).find(q) == std::string::npos;
            }), list.end());
        }
        sendJson(res, 200, listJson(list));
    }));

    // Предварительный расчёт риска для формы (запись не сохраняется)
    srv.Post("/api/preview", api(true, [&](const httplib::Request& req, httplib::Response& res) {
        AuditObject o{};
        o.antivirus = req.get_param_value("antivirus") == "1";
        o.passwordOk = req.get_param_value("passwordOk") == "1";
        o.personalData = req.get_param_value("personalData") == "1";
        Date check{}, upd{};
        o.checkDate = Date::parse(trim(req.get_param_value("checkDate")), check) ? check : Date{1, 1, 2026};
        o.avUpdate = Date::parse(trim(req.get_param_value("avUpdate")), upd) ? upd : o.checkDate;
        o.risk = calcRisk(o);
        sendJson(res, 200, "{\"risk\":" + std::to_string(o.risk) + ",\"riskClass\":" + std::to_string(riskClass(o.risk)) +
                           ",\"level\":" + js(riskLevel(o.risk)) + ",\"violations\":" + js(violations(o)) + "}");
    }));

    // Добавление объекта
    srv.Post("/api/object", api(true, [&](const httplib::Request& req, httplib::Response& res) {
        std::string inv = textParam(req, "inv", 15, "Инвентарный номер");
        AuditObject existing{};
        if (file.findByInv(inv, existing)) throw BadRequest{"Объект с таким инвентарным номером уже есть"};
        AuditObject o = objectFromRequest(req, inv);
        file.append(o);
        sendJson(res, 200, objectJson(o));
    }));

    // Корректировка: запись заменяется на месте в файле
    srv.Post("/api/object/update", api(true, [&](const httplib::Request& req, httplib::Response& res) {
        std::string inv = trim(req.get_param_value("inv"));
        AuditObject existing{};
        if (!file.findByInv(inv, existing)) return sendError(res, 404, "Объект не найден");
        AuditObject o = objectFromRequest(req, inv);
        if (!file.update(inv, o)) return sendError(res, 404, "Объект не найден");
        sendJson(res, 200, objectJson(o));
    }));

    srv.Post("/api/object/delete", api(true, [&](const httplib::Request& req, httplib::Response& res) {
        if (!file.remove(trim(req.get_param_value("inv")))) return sendError(res, 404, "Объект не найден");
        sendJson(res, 200, "{\"ok\":true}");
    }));

    // Сводка для главной страницы
    srv.Get("/api/stats", api(true, [&](const httplib::Request&, httplib::Response& res) {
        std::vector<AuditObject> all = file.readAll();
        int level[3] = {0, 0, 0}, noAv = 0, oldAv = 0, weakPwd = 0, pdRisk = 0;
        for (const AuditObject& o : all) {
            ++level[riskClass(o.risk)];
            if (!o.antivirus) ++noAv;
            if (avOutdated(o)) ++oldAv;
            if (!o.passwordOk) ++weakPwd;
            if (o.personalData && o.risk > 0) ++pdRisk;
        }
        std::vector<AuditObject> top = applyQuery(all, Filter{FilterKind::None, 0}, SortKind::Risk, true);
        if (top.size() > 5) top.resize(5);
        top.erase(std::remove_if(top.begin(), top.end(), [](const AuditObject& o) { return o.risk == 0; }), top.end());
        sendJson(res, 200, "{\"total\":" + std::to_string(all.size()) + ",\"low\":" + std::to_string(level[0]) +
                           ",\"medium\":" + std::to_string(level[1]) + ",\"high\":" + std::to_string(level[2]) +
                           ",\"noAntivirus\":" + std::to_string(noAv) + ",\"oldAntivirus\":" + std::to_string(oldAv) +
                           ",\"weakPassword\":" + std::to_string(weakPwd) + ",\"pdAtRisk\":" + std::to_string(pdRisk) +
                           ",\"top\":" + listJson(top) + "}");
    }));

    // Отчёт о несоответствиях: формирование второго файла и чтение
    srv.Post("/api/report/build", api(true, [&](const httplib::Request& req, httplib::Response& res) {
        int minRisk = intParam(req, "minRisk", 1, 9, "порог риска");
        long n = report.build(file, minRisk);
        sendJson(res, 200, "{\"count\":" + std::to_string(n) + "}");
    }));

    srv.Get("/api/report", api(true, [&](const httplib::Request&, httplib::Response& res) {
        if (!report.exists()) return sendJson(res, 200, "{\"exists\":false,\"items\":[]}");
        std::string s = "{\"exists\":true,\"items\":[";
        std::vector<ReportRecord> list = report.readAll();
        for (size_t i = 0; i < list.size(); ++i) {
            const ReportRecord& r = list[i];
            s += std::string(i ? "," : "") + "{\"inv\":" + js(r.invNumber) +
                 ",\"departmentName\":" + js(DEPARTMENTS[r.department]) + ",\"responsible\":" + js(r.responsible) +
                 ",\"violations\":" + js(r.violations) + ",\"risk\":" + std::to_string(r.risk) +
                 ",\"riskClass\":" + std::to_string(riskClass(r.risk)) + ",\"level\":" + js(riskLevel(r.risk)) + "}";
        }
        sendJson(res, 200, s + "]}");
    }));

    std::cout << "Аудит ИБ УК. Откройте в браузере: http://127.0.0.1:" << PORT << "\n"
              << "Остановить сервер: Ctrl+C\n";
    if (!srv.listen("127.0.0.1", PORT)) {
        std::cout << "Не удалось запустить сервер на порту " << PORT << " (порт занят?)\n";
        return 1;
    }
    return 0;
}
