# Makefile для управления проектом

# Настройки
CC = gcc
CFLAGS = -Wall -I./include
BUILD_DIR = labs
TARGET = myapp
SRC_EXT = .c
TEST_SUFFIX = _test
FORMATTER = clang-format -i
CHECK_FORMATTER = clang-format --dry-run --Werror

# Файлы
SOURCES = $(shell find . -name "*$(SRC_EXT)" -not -path "./$(BUILD_DIR)/*")
TEST_FILES = $(shell find . -name "*$(TEST_SUFFIX)$(SRC_EXT)")

# Правила
.PHONY: all clean format check-format test

all: clean $(TARGET)

%_test: %_test.c
	$(CC) $(CFLAGS) $< -o $@ -L. -l$*

# Очистка рабочей директории
clean:
	@echo "Очистка проекта..."
	rm -rf $(BUILD_DIR) *.o $(TARGET)
	rm -f $(TEST_FILES:$(SRC_EXT)=) # Удаление скомпилированных тестов

# Форматирование кода (clang-format)
format:
	@echo "Форматирование кода..."
	$(FORMATTER) $(SOURCES)

# Проверка форматирования (fail, если есть ошибки)
check-format:
	@echo "Проверка форматирования..."
	$(CHECK_FORMATTER) $(SOURCES)

# Запуск тестов (выполнение файлов *_test)
test:
	@echo "Запуск тестов..."
	@for test in $(TEST_FILES); do \
		./$${test%$(SRC_EXT)}; \
	done