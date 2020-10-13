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

#ifndef __NUC123_H__
// This is a trick to avoid including qmk's headers for NUC123.h
#ifdef TIMER_H
#ifdef PS2_H
#ifdef _uart_included_h_
#include "../Include/NUC123.h"
#else
#define _uart_included_h_
#include "../Include/NUC123.h"
#undef _uart_included_h_
#endif
#else
#define PS2_H
#ifdef _uart_included_h_
#include "../Include/NUC123.h"
#else
#define _uart_included_h_
#include "../Include/NUC123.h"
#undef _uart_included_h_
#endif
#undef PS2_H
#endif
#else
#define TIMER_H
#ifdef PS2_H
#ifdef _uart_included_h_
#include "../Include/NUC123.h"
#else
#define _uart_included_h_
#include "../Include/NUC123.h"
#undef _uart_included_h_
#endif
#else
#define PS2_H
#ifdef _uart_included_h_
#include "../Include/NUC123.h"
#else
#define _uart_included_h_
#include "../Include/NUC123.h"
#undef _uart_included_h_
#endif
#undef PS2_H
#endif
#undef TIMER_H
#endif
#include "../inc/timer.h"
#include "../inc/uart.h"
#include "../inc/ps2.h"
#endif
