

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
CP:=cp
ARCHIVER:=ar

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


LIBFLAGS  = $(foreach l,$(LIBS),-Wl,-l:$(l))
LIBFLAGS += $(foreach d,$(LDRS),-L $(d))


TARGET = $(PROJECT)

BUILD_DIR := $(ROOT_PATH)/build
OUTPU_DIR := $(ROOT_PATH)/output

#OBJS = $(patsubst $(ROOT_PATH)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
OBJS = $(patsubst $(ROOT_PATH)/%,$(BUILD_DIR)/%.o,$(SRCS))

#MAIN_OBJS = $(patsubst $(ROOT_PATH)/%.cpp,$(BUILD_DIR)/%.o,$(MAIN_SRCS))
MAIN_OBJS = $(patsubst $(ROOT_PATH)/%,$(BUILD_DIR)/%.o,$(MAIN_SRCS))

#MAIN_EXES = $(patsubst $(ROOT_PATH)/%.cpp,$(OUTPU_DIR)/%,$(MAIN_SRCS))
MAIN_EXES = $(patsubst $(ROOT_PATH)/%,$(OUTPU_DIR)/%.out,$(MAIN_SRCS))


################################################################################

# Object file creation
$(BUILD_DIR)%.o : $(ROOT_PATH)%
	@echo -e $(ECHO_BOLD)$(ECHO_GREEN)"compiling" $@ "from" $< $(ECHO_RESET)
	@$(MKDIR) -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

# keep target object files
.PRECIOUS: $(MAIN_OBJS)

# Executable recipe
$(OUTPU_DIR)%.out : $(OBJS) $(BUILD_DIR)%.o
	@echo -e $(ECHO_BOLD)$(ECHO_BLUE)"Linking" $@ "from" $^ $(ECHO_RESET)
	@$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) $^ $(LIBFLAGS) $(LDFLAGS) -o $@
	$(CP) $@ ./$(notdir $@)


.PHONY: all
all: $(LIBS) $(MAIN_EXES)
	@echo -e "Done!"


.PHONY: lib
lib: $(OBJS)
	@echo -e $(ECHO_BOLD)$(ECHO_BLUE)"Making static library" $^ $(ECHO_RESET)
	$(ARCHIVER) -rcs $(BUILD_DIR)/$(TARGET).a $^

.PHONY: test
test:
	@echo "main srcs" $(MAIN_SRCS)
	@echo "main exes" $(MAIN_EXES)
	@echo "objs" $(OBJS)
	@echo "flag" $(CFLAGS)
	@echo "libflags" $(LIBFLAGS)
	@echo "libs" $(LIBS)
	@echo "test"
	#$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $(TARGET).exe


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(OUTPU_DIR)


.PHONY: run
run:
	./$(TARGET)
