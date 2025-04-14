NAMEBIN = snake

# Project structure
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc
ZEN_DIR = ../../zen
LIB_DIR = ../../lib

# Source files with subdirectories
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) \
            $(wildcard $(SRC_DIR)/components/*.c) \
            $(wildcard $(SRC_DIR)/layers/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))

# Compiler settings
CC = clang
CFLAGS = -std=c11 -Oz $(addprefix -f, visibility=hidden no-math-errno omit-frame-pointer fast-math merge-all-constants function-sections data-sections lto short-enums no-common stack-protector-strong) \
            $(addprefix -W, all extra error pedantic conversion null-dereference \
            cast-align unreachable-code strict-prototypes bad-function-cast \
            strict-aliasing shadow double-promotion missing-prototypes \
            missing-declarations pointer-arith format=2) \
            -I$(INC_DIR) -I$(ZEN_DIR)/inc -I$(ZEN_DIR)

DEBUG_FLAGS = -g3 -DDEBUG -O0

# Library paths
STATIC_LIB = $(LIB_DIR)/libzen.a
DYNAMIC_LIB = $(LIB_DIR)/libzen.so
STATIC_DEBUG_LIB = $(LIB_DIR)/libzen_debug.a
DYNAMIC_DEBUG_LIB = $(LIB_DIR)/libzen_debug.so

# Colors for pretty output
GREEN := \033[32m
YELLOW := \033[33m
RED := \033[31m
BLUE := \033[34m
RESET := \033[0m

MKDIR = mkdir -p
RM = rm -rf

# Default target builds with static library
all: clean static

# Check if required library exists
check_static_lib:
	@if [ ! -f $(STATIC_LIB) ]; then \
		echo "$(RED)Error: $(STATIC_LIB) not found! Run 'make compile' in root directory first.$(RESET)"; \
		exit 1; \
	fi

check_dynamic_lib:
	@if [ ! -f $(DYNAMIC_LIB) ]; then \
		echo "$(RED)Error: $(DYNAMIC_LIB) not found! Run 'make compile' in root directory first.$(RESET)"; \
		exit 1; \
	fi

check_static_debug_lib:
	@if [ ! -f $(STATIC_DEBUG_LIB) ]; then \
		echo "$(RED)Error: $(STATIC_DEBUG_LIB) not found! Run 'make compile' in root directory first.$(RESET)"; \
		exit 1; \
	fi

check_dynamic_debug_lib:
	@if [ ! -f $(DYNAMIC_DEBUG_LIB) ]; then \
		echo "$(RED)Error: $(DYNAMIC_DEBUG_LIB) not found! Run 'make compile' in root directory first.$(RESET)"; \
		exit 1; \
	fi

# Build with static library (release)
.PHONY: static
static: check_static_lib $(OBJ_FILES)
	@echo "$(GREEN)Building with static library...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ_FILES) $(STATIC_LIB) -o $(NAMEBIN)_static -lm
	@strip $(NAMEBIN)_static
	@$(RM) $(OBJ_DIR)

# Build with dynamic library (release)
.PHONY: dynamic
dynamic: check_dynamic_lib $(OBJ_FILES)
	@echo "$(GREEN)Building with dynamic library...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ_FILES) -L$(LIB_DIR) -lzen -Wl,-rpath,'$(shell realpath $(LIB_DIR))' -o $(NAMEBIN)_dynamic -lm
	@strip $(NAMEBIN)_dynamic
	@$(RM) $(OBJ_DIR)

# Build with static library (debug)
.PHONY: static_debug
static_debug: check_static_debug_lib $(OBJ_FILES)
	@echo "$(GREEN)Building with static debug library...$(RESET)"
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OBJ_FILES) $(STATIC_DEBUG_LIB) -o $(NAMEBIN)_static_debug -lm
	@$(RM) $(OBJ_DIR)

# Build with dynamic library (debug)
.PHONY: dynamic_debug
dynamic_debug: check_dynamic_debug_lib $(OBJ_FILES)
	@echo "$(GREEN)Building with dynamic debug library...$(RESET)"
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(OBJ_FILES) -L$(LIB_DIR) -lzen_debug -Wl,-rpath,'$(shell realpath $(LIB_DIR))' -o $(NAMEBIN)_dynamic_debug -lm
	@$(RM) $(OBJ_DIR)

$(OBJ_FILES): | $(OBJ_DIR)

# Generic compilation rule for all source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

# Rules for components
$(OBJ_DIR)/%.o: $(SRC_DIR)/components/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

# Rules for layers
$(OBJ_DIR)/%.o: $(SRC_DIR)/layers/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@$(MKDIR) $@

clean:
	@echo "$(GREEN)Cleaning build artifacts...$(RESET)"
	@$(RM) $(OBJ_DIR)
	@$(RM) $(NAMEBIN)_static
	@$(RM) $(NAMEBIN)_dynamic
	@$(RM) $(NAMEBIN)_static_debug
	@$(RM) $(NAMEBIN)_dynamic_debug
	@echo "$(GREEN)Done cleaning$(RESET)"

reinstall: clean all

# List available targets
.PHONY: list
list:
	@echo "$(BLUE)Available build targets:$(RESET)"
	@echo "$(BLUE)Release builds:$(RESET)"
	@echo "  $(YELLOW)make static$(RESET)        - Build with static library"
	@echo "  $(YELLOW)make dynamic$(RESET)       - Build with dynamic library"
	@echo ""
	@echo "$(BLUE)Debug builds:$(RESET)"
	@echo "  $(YELLOW)make static_debug$(RESET)  - Build with static debug library"
	@echo "  $(YELLOW)make dynamic_debug$(RESET) - Build with dynamic debug library"
	@echo ""
	@echo "$(BLUE)Other targets:$(RESET)"
	@echo "  $(YELLOW)make clean$(RESET)         - Clean all build artifacts"
	@echo "  $(YELLOW)make list$(RESET)          - Show this help message"

.PHONY: all clean reinstall static dynamic static_debug dynamic_debug check_static_lib check_dynamic_lib check_static_debug_lib check_dynamic_debug_lib
