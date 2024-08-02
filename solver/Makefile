CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

SRCS = $(wildcard *.cpp)

BUILD_DIR = build

OBJS = $(SRCS:%.cpp=${BUILD_DIR}/%.o)

TARGET = solver

MAKEFLAGS += --no-print-directory

MAKEFILE = $(lastword $(MAKEFILE_LIST))

all:
	@if [ ! -f $(TARGET) ] || [ Makefile -nt $(TARGET) ]; then \
		$(MAKE) clean; \
	fi; \
	if [ ! -f $(TARGET) ] || [ $(shell ls -t $(SRCS) Makefile | head -1) -nt $(TARGET) ]; then \
		$(MAKE) $(TARGET); \
		$(MAKE) depend; \
		echo "Build complete."; \
	else \
		echo "The target '$(TARGET)' is already up to date."; \
	fi

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf ${BUILD_DIR} ${TARGET}

depend: $(SRCS)
	$(CXX) -MM $(SRCS) > ${BUILD_DIR}/.depend

-include ${BUILD_DIR}.depend

.PHONY: all clean depend
