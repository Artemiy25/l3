# Makefile лабораторной работы.
# Сборка: make        Очистка: make clean

# Флаги компиляции:
#   -std=c++17  - стандарт C++17
#   -Iinclude   - где искать заголовочные файлы (#include "hw/...")
#   -O0         - без оптимизации, чтобы покрытие считалось точно
#   --coverage  - собирать информацию о покрытии кода тестами
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -O0 --coverage

TARGET  = bin/lab
OBJECTS = build/l1_UserInterface.o build/l2_ApplicationLayer.o \
          build/l3_DomainLayer.o build/l4_InfrastructureLayer.o
HEADERS = include/hw/l2_ApplicationLayer.h include/hw/l3_DomainLayer.h \
          include/hw/l4_InfrastructureLayer.h

.PHONY: all clean

# Генератор нагрузочных тестов (ЛР3) собирается без --coverage:
# это вспомогательная программа, её покрытие считать не нужно
STRESSGEN = bin/stressgen

all: $(TARGET) $(STRESSGEN)

$(STRESSGEN): src/stressgen/stressgen.cpp
	$(CXX) -std=c++17 -Wall -Wextra -O2 -o $(STRESSGEN) src/stressgen/stressgen.cpp

# Компоновка исполняемого файла
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Компиляция каждого модуля (уровня) отдельно
build/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf bin/* build/*
