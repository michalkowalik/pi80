//
// Created by Michal Kowalik on 03.03.24.
//

#ifndef PI80_CPM_H
#define PI80_CPM_H

#include "pico/stdlib.h"


static const uint16_t stage2_cpm_start_address = 0x0080;
static const uint16_t stage2_cpm_loader_size = 0xEC;
static uint8_t stage2_cpm_loader[] = {
        0x31, 0x80, 0x00, 0x21, 0x2D, 0x01, 0xCD, 0xFA, 0x00, 0x3E, 0x32, 0x16, 0x00, 0x1E, 0x01, 0x21,
        0x00, 0xE4, 0x32, 0x6B, 0x01, 0x22, 0x6C, 0x01, 0x4A, 0xCD, 0xD9, 0x00, 0x4B, 0xCD, 0xE0, 0x00,
        0xED, 0x4B, 0x6C, 0x01, 0x21, 0x80, 0x00, 0x09, 0x22, 0x6C, 0x01, 0xED, 0x43, 0x6E, 0x01, 0xCD,
        0xE7, 0x00, 0xB7, 0x20, 0x14, 0x3A, 0x6B, 0x01, 0x3D, 0x28, 0x15, 0x32, 0x6B, 0x01, 0x1C, 0x3E,
        0x21, 0xBB, 0x20, 0xD4, 0x1E, 0x01, 0x53, 0x18, 0xCF, 0x21, 0x05, 0x01, 0xCD, 0xFA, 0x00, 0x76,
        0x21, 0x63, 0x01, 0xCD, 0xFA, 0x00, 0xC3, 0x00, 0xFA, 0x79, 0xD3, 0x0A, 0xAF, 0xD3, 0x0A, 0xC9,
        0x79, 0xD3, 0x0B, 0xAF, 0xD3, 0x0B, 0xC9, 0xAF, 0xD3, 0x09, 0x0E, 0x06, 0x06, 0x80, 0x2A, 0x6E,
        0x01, 0xED, 0xB2, 0xDB, 0x05, 0xB7, 0xC8, 0x3E, 0x01, 0xC9, 0x7E, 0xFE, 0x00, 0x28, 0x05, 0xD3,
        0x01, 0x23, 0x18, 0xF6, 0xC9, 0x0D, 0x0A, 0x46, 0x41, 0x54, 0x41, 0x4C, 0x20, 0x44, 0x49, 0x53,
        0x4B, 0x20, 0x52, 0x45, 0x41, 0x44, 0x20, 0x45, 0x52, 0x52, 0x4F, 0x52, 0x20, 0x2D, 0x20, 0x53,
        0x59, 0x53, 0x54, 0x45, 0x4D, 0x20, 0x48, 0x41, 0x4C, 0x54, 0x45, 0x44, 0x00, 0x0D, 0x0A, 0x0A,
        0x5A, 0x38, 0x30, 0x2D, 0x4D, 0x42, 0x43, 0x20, 0x43, 0x50, 0x2F, 0x4D, 0x20, 0x32, 0x2E, 0x32,
        0x20, 0x43, 0x6F, 0x6C, 0x64, 0x20, 0x4C, 0x6F, 0x61, 0x64, 0x65, 0x72, 0x20, 0x2D, 0x20, 0x53,
        0x31, 0x35, 0x30, 0x34, 0x31, 0x37, 0x0D, 0x0A, 0x4C, 0x6F, 0x61, 0x64, 0x69, 0x6E, 0x67, 0x2E,
        0x2E, 0x2E, 0x00, 0x20, 0x64, 0x6F, 0x6E, 0x65, 0x0D, 0x0A, 0x00
};

#endif //PI80_CPM_H
