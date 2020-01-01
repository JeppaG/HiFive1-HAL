#
# Copyright 2019 JG Mechatronics AB
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#****************************************************************************
 
ROOT = ..
TEST_ROOT = $(ROOT)/test
DRIVER_MOCK_ROOT = $(ROOT)/mock
CPPU_TEST_ROOT = $(HOME)/tools/cpputest

DUT_INC_PATH = $(ROOT)/inc
DUT_SRC_PATH = $(ROOT)/src

TEST_INC_PATH = $(TEST_ROOT)/inc
TEST_SRC_PATH = $(TEST_ROOT)/src

MOCK_INC_PATH = $(DRIVER_MOCK_ROOT)/inc
MOCK_SRC_PATH = $(DRIVER_MOCK_ROOT)/src

CPPU_TEST_INC_PATH = $(CPPU_TEST_ROOT)/inc
CPPU_TEST_LIB_PATH = $(CPPU_TEST_ROOT)/lib

BUILD_PATH = build
OBJ_PATH = $(BUILD_PATH)/obj

G++ = g++
LD = g++

G++_FLAGS = -O0 -g -c -fno-exceptions -fno-unwind-tables
LD_FLAGS = -g -ffreestanding -Wl,--gc-sections -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit
        
# Source file list

DUT_SOURCE = \
  spi.cpp \
  
TEST_SOURCE = \
  spiTest.cpp \
 
MOCK_SOURCE = \
  gpioMock.cpp \
  fifoOperations.cpp \
  
INCLUDE_PATH = \
  -I $(DUT_INC_PATH) \
  -I $(TEST_INC_PATH) \
  -I $(MOCK_INC_PATH) \
  -I $(CPPU_TEST_INC_PATH) \
  
$(shell mkdir -p $(BUILD_PATH) 2> /dev/null)
$(shell mkdir -p $(OBJ_PATH) 2> /dev/null)
  
OBJ = $(DUT_SOURCE:.cpp=.o) \
      $(TEST_SOURCE:.cpp=.o) \
      $(MOCK_SOURCE:.cpp=.o) \

$(OBJ_PATH)/%.o : $(DUT_SRC_PATH)/%.cpp
	$(G++) $(G++_FLAGS) $(INCLUDE_PATH) $< -o$@ 
      
$(OBJ_PATH)/%.o : $(TEST_SRC_PATH)/%.cpp
	$(G++) $(G++_FLAGS) $(INCLUDE_PATH) $< -o$@
	
$(OBJ_PATH)/%.o : $(MOCK_SRC_PATH)/%.cpp
	$(G++) $(G++_FLAGS) $(INCLUDE_PATH) $< -o$@

$(BUILD_PATH)/testRunner : $(addprefix $(OBJ_PATH)/, $(OBJ))
	$(LD) $(LD_FLAGS) $^ $(CPPU_TEST_LIB_PATH)/libCppUTest.a $(CPPU_TEST_LIB_PATH)/libCppUTestExt.a -o$@

run : $(BUILD_PATH)/testRunner
	$<
			 
clean :
	rm $(BUILD_PATH)/testRunner $(OBJ_PATH)/*.*
	
-include $(addprefix $(OBJ_PATH), $(OBJ:.o=.d))
    
.PHONY : debug

debug : ; $(info $$(addprefix $(OBJ_PATH), $(OBJ)) is [$(addprefix $(OBJ_PATH), $(OBJ))])
	
all : $(BUILD_PATH)/testRunner