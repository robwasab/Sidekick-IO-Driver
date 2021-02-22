

CFLAGS += \
	-Wall \
	-O0 \
	-L .\
	-g


CC:=g++
LD:=ld
OCPY:=objcopy
ODUMP:=objdump
SZ:=size
MKDIR:=mkdir
MV:=mv

# puts each function and each data item into its own section
#-ffunction-sections \
#-fdata-sections

# routes debug symbols to a different directory at compile time
#-fdebug-prefix-map=$(REPO_ROOT)= \

# this is for baremetal applications
#-ffreestanding \


LDFLAGS += \
	-Wl,--gc-sections \
	-Wl,-Map=$(BUILD_DIR)/$(PROJECT).map


ECHO_BOLD="\e[1m"
ECHO_RESET="\e[0m"
ECHO_GREEN="\e[32m"
ECHO_MAGENTA="\e[95m"
ECHO_BLUE="\e[94m"

#-Wl,--print-memory-usage \

CFLAGS += $(foreach i,$(INCLUDES),-I$(i))
CFLAGS += $(foreach d,$(DEFINES),-D$(d))

TARGET = $(PROJECT)

BUILD_DIR := $(ROOT_PATH)/build
OUTPU_DIR := $(ROOT_PATH)/output

OBJS = $(patsubst $(ROOT_PATH)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

MAIN_OBJS = $(patsubst $(ROOT_PATH)/%.cpp,$(BUILD_DIR)/%.o,$(MAIN_SRCS))

MAIN_EXES = $(patsubst $(ROOT_PATH)/%.cpp,$(OUTPU_DIR)/%,$(MAIN_SRCS))


################################################################################


$(BUILD_DIR)%.o : $(ROOT_PATH)%.cpp
	@echo -e $(ECHO_BOLD)$(ECHO_GREEN)"compiling" $@ "from" $< $(ECHO_RESET)
	@$(MKDIR) -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)


$(OUTPU_DIR)%: $(OBJS) $(BUILD_DIR)%.o
	@echo -e $(ECHO_BOLD)$(ECHO_BLUE)"Linking" $@ "from" $^ $(ECHO_RESET)
	@$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	$(MV) $@ ./$(notdir $@)


all: $(MAIN_EXES)
	@echo -e "Done!"


.PHONY: test
test: $(OBJS)
	@echo "srcs" $(SRCS)
	@echo "objs" $(OBJS)
	@echo "flag" $(CFLAGS)
	@echo "test"
	#$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $(TARGET).exe

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(OUTPU_DIR)

.PHONY: run
run:
	./$(TARGET)
