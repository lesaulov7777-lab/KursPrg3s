# Makefile — сборка программы аудита ИБ
# make        — собрать программу (audit)
# make clean  — удалить результаты сборки

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET   = audit
SRCS     = $(wildcard *.cpp)
OBJS     = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

.PHONY: clean
