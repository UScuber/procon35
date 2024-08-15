CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -MMD -MP

SRCS = $(wildcard *.cpp)

BUILD_DIR = build

OBJS = $(SRCS:%.cpp=${BUILD_DIR}/%.o)

TARGET = solver

DEPENDS = $(OBJS:${BUILD_DIR}/%.o=${BUILD_DIR}/%.d)


all: $(TARGET)

-include $(DEPENDS)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf ${BUILD_DIR} ${TARGET}

.PHONY: all clean
