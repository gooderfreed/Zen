NAMEBIN = solitaire

# Project structure
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc
CORE_DIR = ../../core

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/components/*.c) $(wildcard $(SRC_DIR)/layers/*.c) $(wildcard $(SRC_DIR)/ui/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))

# Core library sources
CORE_SRC_DIR = $(CORE_DIR)/src
CORE_INC_DIR = $(CORE_DIR)/inc
CORE_SRC_FILES = $(wildcard $(CORE_SRC_DIR)/*.c)
CORE_OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(CORE_SRC_FILES:%.c=%.o)))
CORE_INC_FILES = $(wildcard $(CORE_INC_DIR)/*.h)

# Compiler settings
CC = clang
CFLAGS = -std=c11 -Oz $(addprefix -f, visibility=hidden no-math-errno omit-frame-pointer fast-math merge-all-constants function-sections data-sections lto short-enums no-common stack-protector-strong) \
            $(addprefix -W, all extra error pedantic conversion null-dereference \
            cast-align unreachable-code strict-prototypes bad-function-cast \
            strict-aliasing shadow double-promotion missing-prototypes \
            missing-declarations pointer-arith format=2) \
            -I$(INC_DIR) -I$(CORE_INC_DIR)

# for debugging purposes
# CFLAGS = -std=c11 -O0 -g3 -ggdb \
#             -I$(INC_DIR) -I$(CORE_INC_DIR)

AR = ar
AFLAGS = rcs
MKDIR = mkdir -p
RM = rm -rf

all: $(NAMEBIN) clean

$(NAMEBIN): $(OBJ_FILES) $(CORE_OBJ_FILES)
	@$(CC) $(CFLAGS) $^ -o $@
	@strip $@

$(OBJ_FILES): | $(OBJ_DIR)

# Compile solitaire files with context
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I $(CORE_INC_DIR) -include $(INC_DIR)/solitaire.h

# Compile layers files with context
$(OBJ_DIR)/%.o: $(SRC_DIR)/layers/%.c $(INC_FILES)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I $(CORE_INC_DIR) -include $(INC_DIR)/solitaire.h

# Compile ui files with context
$(OBJ_DIR)/%.o: $(SRC_DIR)/ui/%.c $(INC_FILES)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I $(CORE_INC_DIR) -include $(INC_DIR)/solitaire.h

# Compile components files with context
$(OBJ_DIR)/%.o: $(SRC_DIR)/components/%.c $(INC_FILES)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I $(CORE_INC_DIR) -include $(INC_DIR)/solitaire.h

# Then compile core files with context
$(OBJ_DIR)/%.o: $(CORE_SRC_DIR)/%.c $(CORE_INC_FILES)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(CORE_INC_DIR) -include $(INC_DIR)/solitaire.h

$(OBJ_DIR):
	@$(MKDIR) $@

clean:
	@$(RM) $(OBJ_DIR)

uninstall:
	@$(RM) $(OBJ_DIR)
	@$(RM) $(NAMEBIN)

reinstall: uninstall all

.PHONY: all uninstall clean reinstall
