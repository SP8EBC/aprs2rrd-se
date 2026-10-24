# Standalone makefile to build and run AprsWXData_test (Boost.Test, header-only variant)
#
# usage:
#   make -f AprsWXData_test.mk         - build and run the test
#   make -f AprsWXData_test.mk build   - build only
#   make -f AprsWXData_test.mk clean   - remove build artifacts

CXX      := g++
CXXFLAGS := -std=c++17 -O0 -g3 -Wall -fmessage-length=0 -MMD -MP
LDLIBS   := -lboost_regex -lboost_date_time

SRC_DIR   := src
BUILD_DIR := build_test/AprsWXData_test
TARGET    := $(BUILD_DIR)/AprsWXData_test

SRCS := $(SRC_DIR)/AprsWXData_test.cpp \
        $(SRC_DIR)/AprsWXData.cpp \
        $(SRC_DIR)/AprsWXDataPositionless.cpp \
        $(SRC_DIR)/Telemetry.cpp \
        $(SRC_DIR)/AprsPacket.cpp

OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all build run clean

all: run

build: $(TARGET)

# test writes its log into ./test_reports, so it must be run from the project root
run: $(TARGET)
	@mkdir -p test_reports
	./$(TARGET) ; status=$$? ; \
	echo "log: test_reports/aprswxdata_test.log" ; \
	exit $$status

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
