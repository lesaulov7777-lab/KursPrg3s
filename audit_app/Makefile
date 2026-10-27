# Makefile — сборка программы аудита ИБ
# make        — собрать консольную версию (audit)
# make run    — собрать и запустить консольную версию
# make web    — собрать веб-версию (audit_web): окно открывается в браузере
# make runweb — собрать и запустить веб-версию, затем открыть http://127.0.0.1:8080
# make test   — собрать и запустить автоматическую проверку (tests/smoke.sh)
# make clean  — удалить результаты сборки (файлы данных не трогает)

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Общие модули: данные, расчёт риска, файлы, отчёт, вход
CORE     = AuditObject.cpp Auth.cpp Date.cpp ObjectFile.cpp Query.cpp Report.cpp Sha256.cpp
CORE_OBJ = $(CORE:.cpp=.o)

TARGET   = audit
CONSOLE  = main.cpp Menu.cpp Input.cpp
OBJS     = $(CONSOLE:.cpp=.o) $(CORE_OBJ)

WEB      = audit_web
WEB_OBJS = web/server.o $(CORE_OBJ)
WEB_LIBS =
ifeq ($(OS),Windows_NT)
WEB_LIBS = -lws2_32
endif
ifneq ($(shell uname -s 2>/dev/null),Windows_NT)
WEB_LIBS += -pthread
endif

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(WEB): $(WEB_OBJS)
	$(CXX) $(CXXFLAGS) $(WEB_OBJS) -o $(WEB) $(WEB_LIBS)

# страница веб-интерфейса встраивается в программу из web/index.html
web/Page.h: web/index.html
	( echo 'static const char* const PAGE_HTML = R"HTML('; cat web/index.html; echo ')HTML";' ) > web/Page.h

# веб-сервер включает большой заголовочный файл, поэтому предупреждения для него отключены
web/server.o: web/server.cpp web/Page.h web/httplib.h
	$(CXX) -std=c++17 -O1 -I. -Iweb -c web/server.cpp -o web/server.o

%.o: %.cpp $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c $< -o $@

web: $(WEB)

run: $(TARGET)
	./$(TARGET)

runweb: $(WEB)
	./$(WEB)

test: $(TARGET)
	bash tests/smoke.sh

clean:
	rm -f *.o web/*.o $(TARGET) $(WEB)

.PHONY: web run runweb test clean
