/*
    Copyright (C) 2017 Frank Zschockelt

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"
#   include "ch.h"
#   define wait_ms(ms) chThdSleepMilliseconds(ms)

#if HAL_USE_PAL || defined(__DOXYGEN__)
/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */

const PALConfig pal_default_config =
{
  .ioport1_pmd = 0xFFffFFff,
  .ioport2_pmd = 0xFFff0000, /* all matrix input pins */
  .ioport3_pmd = 0xf5f5f555, /* matrix output pins */
  .ioport4_pmd = 0xff55ff00, /* matrix output pins + input pins of dip switch */
};
#endif

/**
 * @brief   Early initialization code.
 * @details This initialization is performed just after reset before BSS and
 *          DATA segments initialization.
 */
#if 0
void __early_init(void)
{
}
#endif

/**
 * @brief   Late initialization code.
 * @note    This initialization is performed after BSS and DATA segments
 *          initialization and before invoking the main() function.
 */
void boardInit(void)
{
}
