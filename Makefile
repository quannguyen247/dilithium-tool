# Compiler settings
CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
CFLAGS = -Wall -Wextra -O3 -fPIC

# Directories
DILITHIUM_DIR = dilithium-core/ref
CODEC_DIR = codec-core
BUILD_DIR = build

# Dilithium settings (Mode 2 is standard)
DILITHIUM_MODE ?= 5
CFLAGS += -DDILITHIUM_MODE=$(DILITHIUM_MODE)
CXXFLAGS += -DDILITHIUM_MODE=$(DILITHIUM_MODE)

# Source files
# Dilithium core sources (C)
DILITHIUM_SRCS = $(DILITHIUM_DIR)/sign.c \
                 $(DILITHIUM_DIR)/packing.c \
                 $(DILITHIUM_DIR)/polyvec.c \
                 $(DILITHIUM_DIR)/poly.c \
                 $(DILITHIUM_DIR)/ntt.c \
                 $(DILITHIUM_DIR)/reduce.c \
                 $(DILITHIUM_DIR)/rounding.c \
                 $(DILITHIUM_DIR)/symmetric-shake.c \
                 $(DILITHIUM_DIR)/fips202.c \
                 $(DILITHIUM_DIR)/randombytes.c

# Codec core sources (C++)
CODEC_SRCS = $(CODEC_DIR)/api.cpp \
             $(CODEC_DIR)/enc.cpp \
             $(CODEC_DIR)/dec.cpp

# Main application
MAIN_SRC = main.cpp

# Object files
DILITHIUM_OBJS = $(DILITHIUM_SRCS:%.c=$(BUILD_DIR)/%.o)
CODEC_OBJS = $(CODEC_SRCS:%.cpp=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(BUILD_DIR)/main.o

# Target executable
TARGET = dilithium-tool

# Include paths
INCLUDES = -I$(DILITHIUM_DIR) -I$(CODEC_DIR)

.PHONY: all clean dirs

all: dirs $(TARGET)

dirs:
	@if not exist "$(subst /,\,$(BUILD_DIR)/$(DILITHIUM_DIR))" mkdir "$(subst /,\,$(BUILD_DIR)/$(DILITHIUM_DIR))"
	@if not exist "$(subst /,\,$(BUILD_DIR)/$(CODEC_DIR))" mkdir "$(subst /,\,$(BUILD_DIR)/$(CODEC_DIR))"

# Link everything together
$(TARGET): $(DILITHIUM_OBJS) $(CODEC_OBJS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile C sources (Dilithium)
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile C++ sources (Codec)
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@if exist "$(subst /,\,$(BUILD_DIR))" rmdir /s /q "$(subst /,\,$(BUILD_DIR))"
	@if exist "$(TARGET).exe" del /q "$(TARGET).exe"
