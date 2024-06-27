CC = gcc
CXX = g++
CFLAGS        = -pipe -O2 -Wall -Wextra -D_REENTRANT -fPIC $(DEFINES)
CXXFLAGS      = -pipe -std=c++20 -O2 -Wall -Wextra -D_REENTRANT -fPIC $(DEFINES)
DEFINES       = -DNOMINMAX -DGLOG_NO_ABBREVIATED_SEVERITIES -D_CRT_SECURE_NO_WARNINGS

SRC_DIR = src

OBJ_DIR = release

BIN_DIR = bin

TARGET = $(BIN_DIR)/SystemInfo

SRCS += $(SRC_DIR)/main.cpp \
		$(SRC_DIR)/GetSysInfo.cpp

OBJS += $(OBJ_DIR)/main.o \
        $(OBJ_DIR)/GetSysInfo.o

INCLUDES = -Iinclude/

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS)  -o $@ $^

# 对象文件规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 清理规则
.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

