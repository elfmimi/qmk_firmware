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

#ifndef _NUMICRO_PROTOCOL_H_
#define _NUMICRO_PROTOCOL_H_

#ifdef __NUC123__
#include "NUC123.h"
#define GPIO_MODE_INPUT GPIO_PMD_INPUT
#define GPIO_MODE_OUTPUT GPIO_PMD_OUTPUT
#define GPIO_MODE_OPEN_DRAIN GPIO_PMD_OPEN_DRAIN
#define GPIO_MODE_QUASI GPIO_PMD_QUASI
#endif

#endif  //_NUMICRO_PROTOCOL_H_
