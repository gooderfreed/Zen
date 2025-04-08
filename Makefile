# Main Makefile for building all implementations
# Automatically finds and builds all projects in impl/ directory

# Find all implementation Makefiles
IMPL_MAKEFILES := $(wildcard impl/*/Makefile)
IMPL_DIRS := $(dir $(IMPL_MAKEFILES))

# Colors for pretty build output
GREEN := \033[32m
YELLOW := \033[33m
RED := \033[31m
BLUE := \033[34m
RESET := \033[0m

# Core library settings
CORE_SRC_DIR := core/src
CORE_INC_DIR := core/inc
CORE_OBJ_DIR := core/obj
LIB_DIR := lib

# Source files
CORE_SOURCES := $(wildcard $(CORE_SRC_DIR)/*.c)
CORE_OBJECTS := $(patsubst $(CORE_SRC_DIR)/%.c,$(CORE_OBJ_DIR)/%.o,$(CORE_SOURCES))
CORE_DEBUG_OBJECTS := $(patsubst $(CORE_SRC_DIR)/%.c,$(CORE_OBJ_DIR)/%_debug.o,$(CORE_SOURCES))

# Compiler settings
CC := clang
CFLAGS := -I$(CORE_INC_DIR) \
          -I$(CORE_INC_DIR)/components \
          -I$(CORE_INC_DIR)/interfaces \
          -I$(CORE_INC_DIR)/primitives \
          -fPIC -Wall -Wextra -Oz

# Debug adds symbols but keeps optimizations to handle inlines
DEBUG_FLAGS := -g3 -DDEBUG -Oz

# Library names
STATIC_LIB := $(LIB_DIR)/libcore.a
DYNAMIC_LIB := $(LIB_DIR)/libcore.so
STATIC_DEBUG_LIB := $(LIB_DIR)/libcore_debug.a
DYNAMIC_DEBUG_LIB := $(LIB_DIR)/libcore_debug.so

# Build targets
# ------------

# Default target builds all implementations
.PHONY: all
all: $(IMPL_DIRS)

# Build each implementation
.PHONY: $(IMPL_DIRS)
$(IMPL_DIRS):
	@echo "$(GREEN)Building '$(YELLOW)$(notdir $(patsubst %/,%,$@))$(GREEN)'...$(RESET)"
	@$(MAKE) --no-print-directory -C $@
	@echo "$(GREEN)Done '$(YELLOW)$(notdir $(patsubst %/,%,$@))$(GREEN)'$(RESET)"

# Core library compilation
.PHONY: compile
compile: clean release debug
	@echo "$(GREEN)Cleaning up object files...$(RESET)"
	@rm -rf $(CORE_OBJ_DIR)
	@echo "$(GREEN)All done!$(RESET)"

.PHONY: release
release: $(STATIC_LIB) $(DYNAMIC_LIB)
	@echo "$(GREEN)Release libraries built successfully!$(RESET)"

.PHONY: debug
debug: $(STATIC_DEBUG_LIB) $(DYNAMIC_DEBUG_LIB)
	@echo "$(GREEN)Debug libraries built successfully!$(RESET)"

# Create necessary directories
$(CORE_OBJ_DIR) $(LIB_DIR):
	@mkdir -p $@

# Compile object files (release)
$(CORE_OBJ_DIR)/%.o: $(CORE_SRC_DIR)/%.c | $(CORE_OBJ_DIR)
	@echo "$(GREEN)Compiling $(YELLOW)$<$(GREEN)...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile object files (debug)
$(CORE_OBJ_DIR)/%_debug.o: $(CORE_SRC_DIR)/%.c | $(CORE_OBJ_DIR)
	@echo "$(GREEN)Compiling debug $(YELLOW)$<$(GREEN)...$(RESET)"
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Build static library (release)
$(STATIC_LIB): $(CORE_OBJECTS) | $(LIB_DIR)
	@echo "$(GREEN)Building static library $(YELLOW)$@$(GREEN)...$(RESET)"
	@ar rcs $@ $^

# Build dynamic library (release)
$(DYNAMIC_LIB): $(CORE_OBJECTS) | $(LIB_DIR)
	@echo "$(GREEN)Building dynamic library $(YELLOW)$@$(GREEN)...$(RESET)"
	@$(CC) $(CFLAGS) -shared -o $@ $^

# Build static library (debug)
$(STATIC_DEBUG_LIB): $(CORE_DEBUG_OBJECTS) | $(LIB_DIR)
	@echo "$(GREEN)Building debug static library $(YELLOW)$@$(GREEN)...$(RESET)"
	@ar rcs $@ $^

# Build dynamic library (debug)
$(DYNAMIC_DEBUG_LIB): $(CORE_DEBUG_OBJECTS) | $(LIB_DIR)
	@echo "$(GREEN)Building debug dynamic library $(YELLOW)$@$(GREEN)...$(RESET)"
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -shared -o $@ $^

# Clean all subdirectories and build artifacts
.PHONY: clean
clean:
	@for dir in $(IMPL_DIRS); do \
		name=$$(basename $$dir); \
		echo "$(GREEN)Cleaning '$(YELLOW)$$name$(GREEN)'...$(RESET)"; \
		$(MAKE) --no-print-directory -C $$dir clean; \
		echo "$(GREEN)Done cleaning '$(YELLOW)$$name$(GREEN)'$(RESET)"; \
	done
	@echo "$(GREEN)Cleaning build artifacts...$(RESET)"
	@rm -rf $(CORE_OBJ_DIR) $(LIB_DIR)
	@echo "$(GREEN)Done cleaning$(RESET)"

# List available targets
.PHONY: list
list:
	@echo "$(BLUE)Available targets:$(RESET)"
	@echo "  $(YELLOW)make$(RESET)         - Build all implementations"
	@echo "  $(YELLOW)make compile$(RESET) - Build core libraries (both release and debug versions)"
	@echo "  $(YELLOW)make clean$(RESET)   - Clean all build artifacts"
	@echo ""
	@echo "$(BLUE)Core library targets:$(RESET)"
	@echo "  $(YELLOW)make release$(RESET) - Build release versions of core libraries"
	@echo "  $(YELLOW)make debug$(RESET)   - Build debug versions of core libraries" 