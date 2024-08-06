NAME = solitare.a
NAMEBIN = solitare

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc
LIB_DIR = libmx

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:%.c=%.o)))
INC_FILES = $(wildcard $(INC_DIR)/*.h)
# LIB_FILES = $(wildcard $(LIB_DIR)/$(LIB_DIR).a)
# LIB_INC_FILES = $(wildcard $(LIB_DIR)/$(INC_DIR))

# -g -O3

CC = clang
CFLAGS = -std=c11 $(addprefix -W, all extra error pedantic)
AR = ar
AFLAGS = rcs
MAKE = make -C

MKDIR = mkdir -p
RM = rm -rf

all: $(NAMEBIN) clean

$(NAMEBIN): $(OBJ_FILES)
#	@$(MAKE) $(LIB_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
#	@$(CC) $(CFLAGS) $^ -L$(LIB_DIR) -lmx -o $@

$(OBJ_FILES): | $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_FILES)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)
#	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR) -I $(LIB_INC_FILES)

$(OBJ_DIR):
	@$(MKDIR) $@

clean:
	@$(RM) $(OBJ_DIR)
	@$(RM) $(LIB_FILES)

uninstall:
	@$(RM) $(OBJ_DIR)
	@$(RM) $(NAMEBIN)

reinstall: uninstall all

.PHONY: all uninstall clean reinstall

