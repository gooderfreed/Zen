.DEFAULT_GOAL := all

# Step 1: Find source files
ZEN_DIR := zen
ZEN_SOURCES := $(shell find $(ZEN_DIR) -name '*.c')

# Step 2: Define object file paths and directories
ZEN_OBJ_DIR := $(ZEN_DIR)/obj
LIB_DIR := lib
ZEN_OBJECTS := $(patsubst $(ZEN_DIR)/%.c,$(ZEN_OBJ_DIR)/%.o,$(filter %.c,$(ZEN_SOURCES)))
ZEN_DEBUG_OBJECTS := $(patsubst $(ZEN_DIR)/%.c,$(ZEN_OBJ_DIR)/%_debug.o,$(filter %.c,$(ZEN_SOURCES)))
ZEN_OBJ_DIRS := $(sort $(dir $(ZEN_OBJECTS) $(ZEN_DEBUG_OBJECTS)))

# Step 3: Compiler settings and Directory/Compilation Rules
CC := clang
CFLAGS := -I$(ZEN_DIR) \
          -I$(ZEN_DIR)/components \
          -I$(ZEN_DIR)/interfaces \
          -I$(ZEN_DIR)/primitives \
          -fPIC -Wall -Wextra -Oz
DEBUG_FLAGS := -g3 -DDEBUG -Oz

# Rule to create the library directory
$(LIB_DIR):
	@mkdir -p $@

# Rule to create all necessary object directories
# Using a phony target as a dependency trigger ensures they are created first
.PHONY: ensure_obj_dirs
ensure_obj_dirs: $(ZEN_OBJ_DIRS)

$(ZEN_OBJ_DIRS):
	@mkdir -p $@

# Rule to compile object files (release)
# Depends on the corresponding object directory being created via ensure_obj_dirs
$(ZEN_OBJ_DIR)/%.o: $(ZEN_DIR)/%.c | ensure_obj_dirs
	@echo "Compiling $< -> $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile object files (debug)
# Depends on the corresponding object directory being created via ensure_obj_dirs
$(ZEN_OBJ_DIR)/%_debug.o: $(ZEN_DIR)/%.c | ensure_obj_dirs
	@echo "Compiling (debug) $< -> $@"
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Target to print found sources
.PHONY: print_sources
print_sources:
	@echo "Found source files:"
	@echo $(ZEN_SOURCES)

# Target to print object file paths
.PHONY: print_objects
print_objects:
	@echo "Object files:"
	@echo $(ZEN_OBJECTS)
	@echo "Debug object files:"
	@echo $(ZEN_DEBUG_OBJECTS)

# Target to print required object directories
.PHONY: print_obj_dirs
print_obj_dirs:
	@echo "Required object directories:"
	@echo $(ZEN_OBJ_DIRS)

# Target to test compilation of one file (e.g., zen.c)
.PHONY: test_compile
test_compile: $(ZEN_OBJ_DIR)/zen.o
	@echo "Test compilation successful for zen.o"

.PHONY: test_compile_debug
test_compile_debug: $(ZEN_OBJ_DIR)/zen_debug.o
	@echo "Test compilation successful for zen_debug.o"

# Step 4: Library names and Final Targets
STATIC_LIB := $(LIB_DIR)/libzen.a
DYNAMIC_LIB := $(LIB_DIR)/libzen.so
STATIC_DEBUG_LIB := $(LIB_DIR)/libzen_debug.a
DYNAMIC_DEBUG_LIB := $(LIB_DIR)/libzen_debug.so

# Colors for pretty build output
GREEN := \033[32m
YELLOW := \033[33m
RED := \033[31m
BLUE := \033[34m
RESET := \033[0m

# Build static library (release)
$(STATIC_LIB): $(ZEN_OBJECTS) | $(LIB_DIR) # Depends on objects and lib directory
	@echo "$(GREEN)Building static library $(YELLOW)$@$(GREEN)...$(RESET)"
	@ar rcs $@ $(ZEN_OBJECTS)

# Build dynamic library (release)
$(DYNAMIC_LIB): $(ZEN_OBJECTS) | $(LIB_DIR) # Depends on objects and lib directory
	@echo "$(GREEN)Building dynamic library $(YELLOW)$@$(GREEN)...$(RESET)"
	@$(CC) $(CFLAGS) -shared -o $@ $(ZEN_OBJECTS)

# Build static library (debug)
$(STATIC_DEBUG_LIB): $(ZEN_DEBUG_OBJECTS) | $(LIB_DIR) # Depends on objects and lib directory
	@echo "$(GREEN)Building debug static library $(YELLOW)$@$(GREEN)...$(RESET)"
	@ar rcs $@ $(ZEN_DEBUG_OBJECTS)

# Build dynamic library (debug)
$(DYNAMIC_DEBUG_LIB): $(ZEN_DEBUG_OBJECTS) | $(LIB_DIR) # Depends on objects and lib directory
	@echo "$(GREEN)Building debug dynamic library $(YELLOW)$@$(GREEN)...$(RESET)"
	@$(CC) $(CFLAGS) $(DEBUG_FLAGS) -shared -o $@ $(ZEN_DEBUG_OBJECTS)

# Main Targets

# Find all implementation Makefiles is already done above
.PHONY: all
all: clean compile build

.PHONY: release
release: ensure_obj_dirs $(STATIC_LIB) $(DYNAMIC_LIB) # Depends on obj dirs and library files
	@echo "$(GREEN)Release libraries built successfully!$(RESET)"

.PHONY: debug
debug: ensure_obj_dirs $(STATIC_DEBUG_LIB) $(DYNAMIC_DEBUG_LIB) # Depends on obj dirs and library files
	@echo "$(GREEN)Debug libraries built successfully!$(RESET)"

.PHONY: compile
compile: clean_lib release debug # Depends on cleaning lib, then building release and debug
	@echo "$(GREEN)Zen library compilation finished.$(RESET)"

# Clean ONLY library build artifacts
.PHONY: clean_lib
clean_lib:
	@echo "$(GREEN)Cleaning library build artifacts...$(RESET)"
	@rm -rf $(ZEN_OBJ_DIR) $(LIB_DIR)
	@echo "$(GREEN)Done cleaning library artifacts$(RESET)"

# Clean ALL subdirectories and build artifacts
# Find all implementation Makefiles
IMPL_MAKEFILES := $(wildcard examples/*/Makefile)
IMPL_DIRS := $(dir $(IMPL_MAKEFILES))

.PHONY: clean
clean: clean_lib # Clean library first
	@for dir in $(IMPL_DIRS); do \
		name=$$(basename $$dir); \
		echo "$(GREEN)Cleaning '$(YELLOW)$$name$(GREEN)'...$(RESET)"; \
		$(MAKE) --no-print-directory -C $$dir clean; \
		echo "$(GREEN)Done cleaning '$(YELLOW)$$name$(GREEN)'$(RESET)"; \
	done
	@echo "$(GREEN)Cleaning complete$(RESET)"

# Target to PRINT commands for building all examples
# Ensure 'make compile' has been run first.
.PHONY: build
build:
	@echo "$(BLUE)Building all examples (ensure library is compiled)...$(RESET)"
	@for dir in $(IMPL_DIRS); do \
		name=$$(basename $$dir); \
		echo "$(GREEN)Building example '$(YELLOW)$$name$(GREEN)'...$(RESET)"; \
		$(MAKE) --no-print-directory -C $$dir; \
		echo "$(GREEN)Done building '$(YELLOW)$$name$(GREEN)'$(RESET)"; \
	done
	@echo "$(BLUE)Finished building examples.$(RESET)"

# List available targets
.PHONY: list
list:
	@echo "$(BLUE)Available targets:$(RESET)"
	@echo "  $(YELLOW)make$(RESET)            - Build all examples (automatically runs 'make compile')"
	@echo "  $(YELLOW)make compile$(RESET)    - Build zen libraries (both release and debug versions)"
	@echo "  $(YELLOW)make build$(RESET)      - Build all examples (run 'make compile' first)"
	@echo "  $(YELLOW)make clean$(RESET)      - Clean all build artifacts and examples"
	@echo "  $(YELLOW)make clean_lib$(RESET)  - Clean only zen library artifacts (obj, lib)"
	@echo "  $(YELLOW)make clean_all$(RESET)  - Clean all build artifacts and examples"
	@echo ""
	@echo "$(BLUE)Zen library targets:$(RESET)"
	@echo "  $(YELLOW)make release$(RESET)    - Build release versions of zen libraries"
	@echo "  $(YELLOW)make debug$(RESET)      - Build debug versions of zen libraries"
