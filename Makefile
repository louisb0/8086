CXX = g++
CXXFLAGS = -Wall -Wextra
TARGET = 8086
BUILD_DIR = build

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)

$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: clean
