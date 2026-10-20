# Makefile — сборка программы аудита ИБ
# make        — собрать программу (audit)
# make run    — собрать и запустить
# make test   — собрать и запустить автоматическую проверку (tests/smoke.sh)
# make clean  — удалить результаты сборки (файлы данных не трогает)

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET   = audit
SRCS     = $(wildcard *.cpp)
OBJS     = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	bash tests/smoke.sh

clean:
	rm -f *.o $(TARGET)

.PHONY: run test clean
