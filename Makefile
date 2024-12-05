# Find all Makefiles in impl directory
IMPL_MAKEFILES := $(wildcard impl/*/Makefile)
IMPL_DIRS := $(dir $(IMPL_MAKEFILES))

# Colors for pretty output
GREEN := \033[32m
YELLOW := \033[33m
RESET := \033[0m

# Default target - build everything
.PHONY: all
all: $(IMPL_DIRS)

# Run make in each subdirectory
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