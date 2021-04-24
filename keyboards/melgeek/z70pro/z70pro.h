#pragma once

#include "keymap.h"

#define XXX KC_NO

// ANSI
#define LAYOUT_ansi( \
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D,      K0F, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,      K1E, K1F, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B,      K2D,      K2F, \
    K30,      K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,      K3D, K3E, K3F, \
    K40, K41,      K43,      K45,                K49, K4A,           K4D, K4E, K4F \
) \
{ \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, XXX, XXX }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1E, XXX, XXX }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2B, K2A, XXX, K2D, XXX, XXX }, \
    { K30, XXX, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3D, XXX, XXX, XXX }, \
    { K40, K41, XXX, K43, XXX, K45, XXX, XXX, XXX, K49, K4A, XXX, XXX, XXX, XXX, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, K0F, K1F, K3E, K2F, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, K3F, K4D, K4E, K4F, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX } \
}

// Split 3u-3u
#define LAYOUT_split_70( \
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,      K1E, K1F, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B,      K2D,      K2F, \
    K30,      K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,      K3D, K3E, K3F, \
    K40, K41,      K43,      K45,      K47,      K49, K4A, K4B,      K4D, K4E, K4F \
) \
{ \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, XXX }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1E, XXX, XXX }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2D, XXX, XXX, XXX }, \
    { K30, XXX, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3D, XXX, XXX, XXX }, \
    { K40, K41, XXX, K43, XXX, K45, XXX, K47, XXX, K49, K4A, K4B, XXX, XXX, XXX, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, K0F, K1F, K3E, K2F, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, K3F, K4D, K4E, K4F, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX } \
}

// Split 2u-2.25u
#define LAYOUT_split_71( \
    K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, K0F, \
    K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,      K1E, K1F, \
    K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B,      K2D,      K2F, \
    K30,      K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B,      K3D, K3E, K3F, \
    K40, K41, K42, K43,      K45,      K47,      K49, K4A, K4B,      K4D, K4E, K4F \
) \
{ \
    { K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, XXX }, \
    { K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1E, XXX, XXX }, \
    { K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2D, XXX, XXX, XXX }, \
    { K30, XXX, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3D, XXX, XXX, XXX }, \
    { K40, K41, K42, K43, XXX, K45, XXX, K47, XXX, K49, K4A, K4B, XXX, XXX, XXX, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, K0F, K1F, K3E, K2F, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, K3F, K4D, K4E, K4F, XXX }, \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX, XXX } \
}
