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
 
BUILD_PATH = build
OBJ_PATH = $(BUILD_PATH)/obj
APP_SOURCE_PATH = application/src/
DRV_SOURCE_PATH = driver/src/
DRV_ASM_PATH = driver/asm/

GCC_PATH = ~/tools/riscv32-custom_/bin/
G++ = $(GCC_PATH)riscv32-unknown-elf-g++
LD = $(GCC_PATH)riscv32-unknown-elf-g++

G++_FLAGS = -O0 -g -c -fno-exceptions -fno-unwind-tables -MD -specs=nano.specs
LD_FLAGS = -g -ffreestanding -Wl,--gc-sections -specs=nano.specs \
           -nostartfiles -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit \
           -Wl,-T,riscv32_sifive_e.ld -Xlinker -Map=$(BUILD_PATH)/blink.map
        
# Source file list

APP_SOURCE = \
  echo.cpp \
  
DRIVER_SOURCE = \
  minimumCppRuntime.cpp \
  interrupt.cpp \
  clock.cpp \
  timer.cpp \
  gpio.cpp \
  uart.cpp \
  spi.cpp \
  synchronize_harts.cpp \
  
DRIVER_ASM = \
  entry.S \
  crt0.S \
  exception.S \
  
INCLUDE_PATH = \
  -I driver/inc \
  -I application/inc \
  
$(shell mkdir -p $(BUILD_PATH))
$(shell mkdir -p $(OBJ_PATH))
  
OBJ = $(APP_SOURCE:.cpp=.o) \
      $(DRIVER_SOURCE:.cpp=.o) \
      $(DRIVER_ASM:.S=.o)

$(OBJ_PATH)/%.o : $(APP_SOURCE_PATH)%.cpp
	$(G++) $(G++_FLAGS) $(INCLUDE_PATH) $< -o$@ 
      
$(OBJ_PATH)/%.o : $(DRV_SOURCE_PATH)%.cpp
	$(G++) $(G++_FLAGS) $(INCLUDE_PATH) $< -o$@
	
$(OBJ_PATH)/%.o : $(DRV_ASM_PATH)%.S
	$(G++) $(G++_FLAGS) $(INCLUDE_PATH) $< -o$@
	 
$(BUILD_PATH)/echo.elf : $(addprefix $(OBJ_PATH)/, $(OBJ))
	$(LD) $(LD_FLAGS) $^ -o$@
		 
clean :
	rm $(BUILD_PATH)/echo.elf $(OBJ_PATH)/*.*
	
-include $(addprefix $(OBJ_PATH), $(OBJ:.o=.d))
    
.PHONY : debug

debug : ; $(info $$(addprefix $(OBJ_PATH), $(OBJ)) is [$(addprefix $(OBJ_PATH), $(OBJ))])
	
all : $(BUILD_PATH)/echo.elf