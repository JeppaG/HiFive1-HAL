/*
 * Copyright 2019 JG Mechatronics AB
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************
 *
 * chip.cpp
 *
 *  Created on: 25 Nov 2019
 *      Author: jeppa
 */

#include "spi.hpp"

/* Set the pointers to the spi registers base addresses to the addresses indicated in the FE310-G000 chip manual */
//const uintptr_t SpiImp::spi1BaseAddress = 0x10024000u;
SpiImp::spiRegisterType* const SpiImp::spi1 = reinterpret_cast<SpiImp::spiRegisterType*>( 0x10024000u );



