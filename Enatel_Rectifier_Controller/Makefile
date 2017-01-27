
#CROSS_COMPILE = arm-linux-uclibcgnueabi-  
CROSS_COMPILE = 
CC = gcc
OUTPUT = -o 
EXE = Enatel_Rectifier_Utility
DEBUG_EXE = Enatel_Rectifier_Utility_Debug

SRC_FILE = src/enatel_rectifier_utility.c  
BIN_DIR = bin

all:
	$(CROSS_COMPILE)$(CC) $(OUTPUT) $(BIN_DIR)/$(EXE) $(DEBUG_FLAG) $(SRC_FILE)

debug:
	$(CROSS_COMPILE)$(CC) -DDEBUG_PRINTF $(OUTPUT) $(BIN_DIR)/$(DEBUG_EXE) $(DEBUG_FLAG) $(SRC_FILE)

clean:
	rm -f $(BIN_DIR)/$(EXE) $(BIN_DIR)/$(DEBUG_EXE)
