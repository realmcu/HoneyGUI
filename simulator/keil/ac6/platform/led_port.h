/*
 * Copyright (c) 2017 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __LED_PORT_H__
#define __LED_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* The goal of this file is to create a common LED API for different targets in order
 * create common example applications.
 */

/* Note: Currently this API doesn't handle the case when a peripheral is
 *       mapped to secure and non-secure region as well, but could be extended
 *       to select secure or non-secure one.
 */

/**
 * \brief Get the number of bits of LED port,
 *        in the current platform, that can be
 *        read and written in one chunk.
 *
 * \return Available number of bits of LED port.
 */
unsigned int get_led_port_bit_length(void);

/**
 * \brief Initializes LED device if needed
 */
void led_port_init(void);

/**
 * \brief Set the value of the LED port.
 *
 * \param[in] led Value of the LED port will be set.
 *                Every bit represents one physical LED.
 *
 * \return 0 if succeeded, 1 otherwise
 */
unsigned int set_led_port(unsigned int led_mask);

/**
 * \brief Get the value of the LED port.
 *
 * \return Value of the LED port
 *         Every bit represents one physical LED.
 */
unsigned int get_led_port(void);


#ifdef __cplusplus
}
#endif
#endif /* __LED_PORT_H__ */
