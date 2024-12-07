# Main Makefile for building all implementations
# Automatically finds and builds all projects in impl/ directory

# Find all implementation Makefiles
IMPL_MAKEFILES := $(wildcard impl/*/Makefile)
IMPL_DIRS := $(dir $(IMPL_MAKEFILES))

# Colors for pretty build output
GREEN := \033[32m
YELLOW := \033[33m
RESET := \033[0m

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

# Clean all subdirectories
.PHONY: clean
clean:
	@for dir in $(IMPL_DIRS); do \
		name=$$(basename $$dir); \
		echo "$(GREEN)Cleaning '$(YELLOW)$$name$(GREEN)'...$(RESET)"; \
		$(MAKE) --no-print-directory -C $$dir clean; \
		echo "$(GREEN)Done cleaning '$(YELLOW)$$name$(GREEN)'$(RESET)"; \
	done 