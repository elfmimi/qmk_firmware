/*
    NuMicro Protocol - Copyright (C) 2019 Ein Terakawa

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

#include "bootloader.h"
#include "numicro_protocol.h"

#if __NUC123__

void bootloader_jump(void) {
    SYS_UnlockReg();

    // Clear all reset source bits.
    SYS_ClearResetSrc(SYS_GetResetSrc());

    SYS_ResetChip();

    SYS_LockReg();

    while (1) { }
}

#else

void bootloader_jump(void) {
    while (1) { }
}

#endif
