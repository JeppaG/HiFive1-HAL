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
 * gpioMock.hpp
 *
 *  Created on: 24 Nov 2019
 *      Author: jeppa
 */

#ifndef GPIO_MOCK_HPP_
#define GPIO_MOCK_HPP_

#include "gpio.hpp"

class GpioMock : public Gpio
{
public:
	GpioMock ();

	void setAsOutput ();

	void setAsInput ();

	void setAsIoFunction0 ();

	void setAsIoFunction1 ();

	void vEnableRiseIrq ();

	void vDisableRiseIrq ();

	void set ();

	void clear ();

	virtual ~GpioMock();

};

#endif /* GPIO_HPP_ */
