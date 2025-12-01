################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All Target
# This 'all' target must be defined BEFORE any other include as make may detect
# anything there as a default target. For more information go here:
#
# https://stackoverflow.com/questions/63453579/makefile-stops-building-files-after-first-o-in-out-of-source-build


# 	The problem is that you are including the dependencies before you define the all rule:
# 		-include $(DEPS)
# 		all : $(TARGET)
# 	If you don't specify a particular target to build on the command line (e.g., 
#	if you don't run make all) then make chooses the first explicit target in 
#	the makefile (and any included makefiles!!) as the target to build.
# 	I assume that the dependency definitions in the $(DEPS) variable define main.o 
#	as a target and since that comes before all, it's the only thing that's run by default.

all: APRS2RRD

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := APRS2RRD
BUILD_ARTIFACT_EXTENSION :=
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# Main-build Target
main-build: APRS2RRD

# Tool invocations
APRS2RRD: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L/lib -L/lib/x86_64-linux-gnu -L/lib/i386-linux-gnu -L/usr/lib -o "APRS2RRD" $(OBJS) $(USER_OBJS) $(LIBS) -lmysqlpp -lfmt -lxerces-c-3.2 -lmysqlclient -lconfig -lconfig++ -lcurl -lpthread -lboost_filesystem -lboost_regex -lboost_thread -lboost_system -lboost_unit_test_framework -lboost_date_time -lGraphicsMagick++ -lGraphicsMagick -lGraphicsMagickWand
	@echo 'Finished building target: $@'
	@echo ' '

 #Each subdirectory must supply rules for building sources it contributes
./src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++17 -DUSE_XERCES_3_2 -DXUBUNTU2404 -DMAGICKCORE_QUANTUM_DEPTH=16 -I/usr/include/mysql++ -I/usr/include/GraphicsMagick/ -I../lib/nlohman_json_3_11_3/include/ -I/usr/include/xercesc -I/usr/include/mysql -I/usr/include/curl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) APRS2RRD
	-@echo ' '

.PHONY: all APRS2RRD clean dependents


-include ../makefile.targets
