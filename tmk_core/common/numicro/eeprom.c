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

#include "numicro_protocol.h"

#include "eeprom.h"

#ifndef DATA_FLASH_SIZE
#define DATA_FLASH_SIZE 16*1024
#endif
#define PAGE_SIZE FMC_FLASH_PAGE_SIZE

#define PAGE_EMPTY 0
#define PAGE_PARTLY 1
#define PAGE_FULL 2
#define PAGE_ERROR 2

#define EEPROM_SIZE 64
// _Static_assert(EEPORM_SIZE >= EECONFIG_SIZE)

__attribute__((aligned(4))) static uint8_t eeprom[EEPROM_SIZE]; // emulated-eprom
static uint32_t page_curr;
static uint32_t idx_curr;

#define NUM_PAGES (DATA_FLASH_SIZE / PAGE_SIZE)
uint8_t page_status[NUM_PAGES];
static uint32_t num_empty;

static uint32_t data_flash_base;
#define BASE_ADDR data_flash_base

static uint8_t eeprom_disabled;

#ifndef ASSERT
#define ASSERT(...)
#endif

#ifndef HALT
#define HALT() do { eeprom_disabled = 1; return; } while(0)
#endif

#ifdef USE_FMC_READ
static uint32_t read_dword(uint32_t adr) {
	ASSERT((adr & 3) == 0);
	return FMC_Read(adr);
}

static uint8_t read_byte(uint32_t *cache, uint32_t adr) {
	if (cache[0] != (adr & -4)) {
		cache[0] = adr & -4;
		cache[1] = read_dword(adr & -4);
	}
	return (cache[1] >> ((adr & ~- 4) * 8)) & 0xFF;
}
#else
static __INLINE uint32_t read_dword(uint32_t adr) {
	return *(volatile uint32_t *)adr;
}

static __INLINE uint8_t read_byte(uint32_t *cache, uint32_t adr) {
	(void)cache;
	return *(volatile uint8_t *)adr;
}
#endif

#define READ_BYTE(adr) read_byte(cache, adr)
#define READ_DWORD(adr) read_dword(adr)

static void read_page(uint32_t page) {
	uint32_t cache[2] = { 0 };
	uint32_t page_start = BASE_ADDR + page * PAGE_SIZE;
	uint32_t page_end = page_start + PAGE_SIZE;
	for (uint32_t i = page_start; i < page_end; ) {
		uint8_t c = READ_BYTE(i++);
		uint8_t adr = c & 0x3F; // EEPROM_SIZE - 1 == ~ - EEPROM_SIZE
		switch (c & 0xC0) { // - EEPROM_SIZE
		case 0x00:
			// byte
			ASSERT(i <= page_end - 1);
			eeprom[adr++] = READ_BYTE(i++);
			break;
		case 0x40:
			// word
			ASSERT(adr <= EEPROM_SIZE - 2);
			ASSERT(i <= page_end - 2);
			eeprom[adr++] = READ_BYTE(i++);
			eeprom[adr++] = READ_BYTE(i++);
			break;
		case 0x80:
			// dword
			ASSERT(adr <= EEPROM_SIZE - 4);
			ASSERT(i <= page_end - 4);
			eeprom[adr++] = READ_BYTE(i++);
			eeprom[adr++] = READ_BYTE(i++);
			eeprom[adr++] = READ_BYTE(i++);
			eeprom[adr++] = READ_BYTE(i++);
			break;
		case 0xC0:
			switch (c) {
			case 0xC0:
				// empty
				for (int j = 0; j < EEPROM_SIZE; j++) { eeprom[j] = 0xFF; }
				break;
			case 0xC1:
				// initial state
				ASSERT(i <= page_end - EEPROM_SIZE);
				for (int j = 0; j < EEPROM_SIZE; j++) {
					eeprom[j] = READ_BYTE(i++);
				}
			case 0xD0:
				// ignore this byte (no following byte)
				break;
			case 0xD1:
				// ignore next byte
				i += 1;
				break;
			case 0xD2:
				// ignore next 2 bytes
				i += 2;
				break;
			case 0xD3:
				// ignore next 3 bytes
				i += 3;
				break;
			case 0xD4:
				// ignore next 4 bytes
				i += 4;
				break;
			case 0xFF: {
				// no data - skipping
				// only 0xFF is expected after this.
				uint32_t last = 0;
				for (uint32_t j = i; j < page_end; j++) {
					if (READ_BYTE(j) != 0xFF) {
						last = j;
					}
				}
				if (last >= i) {
					ASSERT(last < i + 4);
					// detected interrupted write - fixing-up
					uint32_t mask = ~(((0xD0 + (last - i + 1)) ^ 0xFF) << (((i - 1) & ~- 4) * 8));
					FMC_Write((i - 1) & -4, mask);
					if (last + 1 < page_end) {
						// This page is partly used. - Next write will occur on this page.
						page_curr = page;
						idx_curr = last + 1 - page_start;
					}
				} else {
					// This page is partly used. - Next write will occur on this page.
					page_curr = page;
					idx_curr = i - 1 - page_start;
				}
				i = page_end;
			}	break;
			default:
				HALT();
			}
			break;
		}
	}
}

void eeprom_init(void) {
    // clear eeprom buffer
    for (int i=0; i < EEPROM_SIZE; i++) { eeprom[i] = 0xFF; }
	eeprom_disabled = 0;

#if __NUC123__
	data_flash_base = FMC->DFBADR;
#else
	data_flash_base = FMC_ReadDataFlashBaseAddr();
#endif


    SYS_UnlockReg();
    FMC_ENABLE_ISP();

    uint32_t page_partly = NUM_PAGES;
    uint32_t page_empty = NUM_PAGES;
    uint32_t page_error = NUM_PAGES;
    page_curr = NUM_PAGES;
    num_empty = 0;
    uint32_t num_partly = 0;
    uint32_t num_error = 0;
    // search the first page
	for (int i = 0; i < NUM_PAGES; i++) {
		if (READ_DWORD(BASE_ADDR + i * PAGE_SIZE) == 0xFFFFFFFF) {
			page_status[i] = PAGE_EMPTY;
			for (int j = 4; j < PAGE_SIZE; j += 4) {
				if (READ_DWORD(BASE_ADDR + i * PAGE_SIZE + j) != 0xFFFFFFFF) {
					// If the first dword is not used and that page is not empty it is an error.
					page_status[i] = PAGE_ERROR;
					num_error += 1;
					page_error = i;
					break;
				}
			}
			if (page_status[i] == PAGE_EMPTY) {
				num_empty += 1;
				page_empty = i;
			}
		} else if ((READ_DWORD(BASE_ADDR + (i + 1) * PAGE_SIZE - 4) >> 24) == 0xFF) {
			page_status[i] = PAGE_PARTLY;
			page_partly = i;
			num_partly += 1;
		} else {
			page_status[i] = PAGE_FULL;
		}
	}

	if (num_error > 0) {
		if (num_error > 1) {
			// This is illeagal. We try to avoid this situation.
			HALT();
		} else {
			// Handle interrupted transition or interrupted first dword write.
			ASSERT(num_error == 1);
			ASSERT(page_error < NUM_PAGES);
			ASSERT(page_status[page_error] == PAGE_ERROR);
			if (FMC_Erase(BASE_ADDR + page_error * PAGE_SIZE)) {
				// Erase error. Maybe no solution.
			}
			num_error = 0;
			num_empty += 1;
			if (num_empty == NUM_PAGES) {
				page_empty = 0;
			} else {
				page_empty = page_error;
			}
			page_status[page_empty] = PAGE_EMPTY;
		}
	}
	if (num_empty == NUM_PAGES) {
		// data flash is totally empty.
		// next write is the very first write.
		page_curr = 0;
		idx_curr = 0;
	} else if (num_partly > 1) {
		// This is illeagal. We try to avoid this situation.
		HALT();
	} else if (num_partly == 0 && num_empty == 0) {
		// This is illeagal. We try to avoid this situation.
		HALT();
	} else {
		if (num_empty == 0) {
			ASSERT(num_partly == 1);
			// Partly used page is the first page. We are gonna erase other pages.
			read_page(page_partly);
		} else {
			// Next page after empty one is the first page.
			ASSERT(num_empty != 0);
			uint32_t page_first = NUM_PAGES;
			for (int i = (page_empty + 1) % NUM_PAGES; i != page_empty; i = (i + 1) % NUM_PAGES) {
				if (page_status[i] != PAGE_EMPTY) {
					page_first = i;
					break;
				}
			}
			// Read out all page that is valid.
			for (int i = page_first; ; i = (i + 1) % NUM_PAGES) {
				if (page_status[i] == PAGE_EMPTY) {
					if (page_status[(i - 1 + NUM_PAGES) % NUM_PAGES] == PAGE_FULL) {
						// Next write will occur on this empty page
						page_curr = i;
						idx_curr = 0;
					}
					break;
				}
				read_page(i);
				if (page_status[i] == PAGE_PARTLY) {
					// There is possibility that the next page is not empty.
					// We are gonna erase the next page before this page gets used up.
					break;
				}
			}
		}
	}

    FMC_DISABLE_ISP();
    SYS_LockReg();

	ASSERT(page_curr < NUM_PAGES);
}

uint8_t  eeprom_read_byte(const uint8_t *__p) {
	uint32_t src = (uint32_t)__p;
	return eeprom[src];
}
uint16_t eeprom_read_word(const uint16_t *__p) {
	uint32_t src = (uint32_t)__p;
	uint16_t dat = eeprom[src++];
	dat |= eeprom[src] << 8;
	return dat;
}
uint32_t eeprom_read_dword(const uint32_t *__p) {
	uint32_t src = (uint32_t)__p;
	uint32_t dat = eeprom[src++];
	dat |= eeprom[src++] << 8;
	dat |= eeprom[src++] << 16;
	dat |= eeprom[src] << 24;
	return dat;
}
void     eeprom_read_block(void *__dst, const void *__src, size_t n) {
	uint8_t *dst = (uint8_t *)__dst;
	uint32_t src = (uint32_t)__src;
	for (uint32_t i = 0; i < n; i++) {
		*dst++ = eeprom[src + i];
	}
}


void transition(void) {
	ASSERT(idx_curr == 0);
	ASSERT(num_empty == 0);
	uint32_t num = 1 + EEPROM_SIZE;
	uint32_t ptr = (num - 1) & -4;
	SYS_UnlockReg();
	FMC_ENABLE_ISP();
	while (num > 4) {
		uint32_t n = ((num - 1) % 4) + 1;
		uint32_t dat = (n == 4) ? 0 : (0xFFFFFFFF << (n * 8));
		for (uint32_t i = 0; i < n; i++) {
			dat |= (eeprom[ptr - 1 + i] << (i * 8));
		}
		FMC_Write(BASE_ADDR + page_curr * PAGE_SIZE + ptr, dat);
		ptr -= 4;
		num -= n;
	}
	uint32_t dat = (num == 4) ? 0 : (0xFFFFFFFF << (num * 8));
	dat |= 0xC1;
	uint32_t n = num - 1;
	for (uint32_t i = 0; i < n; i++) {
		dat |= (eeprom[0 + i] << ((i + 1) * 8));
	}
	FMC_Write(BASE_ADDR + page_curr * PAGE_SIZE + 0, dat);
	FMC_DISABLE_ISP();
	SYS_LockReg();
	idx_curr = 1 + EEPROM_SIZE;
}

uint32_t check_transition(void) {
	if (idx_curr == 0) {
		ASSERT(num_empty >= 1);
		ASSERT(page_status[page_curr] == PAGE_EMPTY);
		// Start using new page
		num_empty -= 1;
		if (num_empty == 0) {
			// This page is the last available page. Need transition.
			transition();
			return 1;
		}
	}
	return 0;
}

void check_clearing(void) {
	if (page_status[(page_curr + 1) % NUM_PAGES] == PAGE_EMPTY) {
		return;
	}
	// Clearing needed
	for (uint32_t i = (page_curr - 1 + NUM_PAGES) % NUM_PAGES; ; i = (i - 1 + NUM_PAGES) % NUM_PAGES) {
		if (page_status[i] != PAGE_EMPTY) {
			SYS_UnlockReg();
			FMC_ENABLE_ISP();
			if (FMC_Erase(BASE_ADDR + i * PAGE_SIZE)) {
				HALT();
			}
			FMC_DISABLE_ISP();
			SYS_LockReg();
			page_status[i] = PAGE_EMPTY;
			num_empty += 1;
			break;
		}
	}
}

void check_full(void) {
	if (idx_curr == PAGE_SIZE) {
		page_status[page_curr] = PAGE_FULL;
		page_curr = (page_curr + 1) % NUM_PAGES;
		idx_curr = 0;
		ASSERT(page_status[page_curr] == PAGE_EMPTY);
	}
}


static void unaligned_write(uint32_t siz, uint8_t cmd, uint32_t val) {
	ASSERT(siz == 1 || siz == 2 || siz == 4);
	ASSERT(siz == 4 || (val & -(1 << (siz * 8))) == 0);
	uint32_t low = cmd | (val << 8);
	uint32_t high = (val >> 24);

	uint32_t shift = (idx_curr % 4) * 8;
	high = (high << shift);
	if (shift != 0) {
		high |= (low >> (32 - shift));
	}
	low <<= shift;
	uint32_t hsb = (siz + 1) * 8 + shift;
	SYS_UnlockReg();
	FMC_ENABLE_ISP();
	if (hsb <= 32) {
		if (hsb != 32) {
			low |= (0xFFFFFFFF << hsb);
		}
		if (shift != 0) {
			low |= (0xFFFFFFFF >> (32 - shift));
		}
		FMC_Write(BASE_ADDR + page_curr * PAGE_SIZE + (idx_curr & -4), low);
	} else {
		if (hsb != 64) {
			high |= (0xFFFFFFFF << (hsb - 32));
		}
		FMC_Write(BASE_ADDR + page_curr * PAGE_SIZE + (idx_curr & -4) + 4, high);
		if (shift != 0) {
			low |= (0xFFFFFFFF >> (32 - shift));
		}
		FMC_Write(BASE_ADDR + page_curr * PAGE_SIZE + (idx_curr & -4), low);
	}
	FMC_DISABLE_ISP();
	SYS_LockReg();
	idx_curr += (siz + 1);
}


static void check_free(uint32_t siz) {
	if (idx_curr > PAGE_SIZE - (siz + 1)) {
		uint32_t dat = 0;
		switch (idx_curr % 4) {
		case 0:
			dat = 0x000000D3;
			break;
		case 1:
			dat = 0x0000D2FF;
			break;
		case 2:
			dat = 0x00D1FFFF;
			break;
		case 3:
			dat = 0xD0FFFFFF;
			break;
		}

		// Not enough space in this page
		SYS_UnlockReg();
		FMC_ENABLE_ISP();
		FMC_Write(BASE_ADDR + (page_curr + 1) * PAGE_SIZE - 4, dat);
		FMC_DISABLE_ISP();
		SYS_LockReg();
		page_status[page_curr] = PAGE_FULL;
		page_curr = (page_curr + 1) % NUM_PAGES;
		ASSERT(page_status[page_curr] == PAGE_EMPTY);
		idx_curr = 0;
	}
}

static void check_last_byte(uint32_t siz, uint32_t val) {
	// Chick if the last byte in a page are going to be 0xFF.
	// If so bring it over to the next page.
	ASSERT(idx_curr + siz + 1 <= PAGE_SIZE);
	if (idx_curr + siz + 1 < PAGE_SIZE) {
		return;
	}
	if ((val >> ((siz - 1) * 8)) != 0xFF) {
		return;
	}
	
	unaligned_write(siz, 0xD0 + siz, 0);
	ASSERT(idx_curr == PAGE_SIZE);
	page_status[page_curr] = PAGE_FULL;
	page_curr = (page_curr + 1) % NUM_PAGES;
	ASSERT(page_status[page_curr] == PAGE_EMPTY);
	idx_curr = 0;
}


uint32_t update_check(uint32_t siz, uint32_t adr, uint32_t val) {
	ASSERT(adr + siz <= EEPROM_SIZE);
	uint32_t update = 0;
	for (uint32_t i = 0; i < siz; i++) {
		uint8_t b = (val >> (i * 8)) & 0xFF;
		if (eeprom[adr + i] != b) {
			update = 1;
			eeprom[adr + i] = b;
		}
	}
	return update;
}


static void eeprom_write(uint32_t adr, uint32_t val, uint32_t siz) {
	if (!update_check(siz, adr, val)) {
		return;
	}
	if (eeprom_disabled) {
		return;
	}
	check_free(siz);
	check_last_byte(siz, val);
	if (check_transition()) {
		// transition occured
		return;
	}
	check_clearing();
	uint8_t cmd = (siz >> 1) << 6;
	unaligned_write(siz, cmd | adr, val);
	check_full();
}

void eeprom_write_byte(uint8_t *__p, uint8_t __value) {
	eeprom_write((uint32_t)__p, __value, 1);
	return;
}
void eeprom_write_word(uint16_t *__p, uint16_t __value) {
	eeprom_write((uint32_t)__p, __value, 2);
	return;
}
void eeprom_write_dword(uint32_t *__p, uint32_t __value) {
	eeprom_write((uint32_t)__p, __value, 4);
	return;
}

void eeprom_write_block(const void *__src, void *__dst, size_t __n) {
	for (size_t i = 0; i < __n; i++) {
		eeprom_write_byte((uint8_t *)__dst + i, ((uint8_t *)__src)[i]);
	}
}

void eeprom_update_byte(uint8_t *__p, uint8_t __value) { eeprom_write_byte(__p, __value); }
void eeprom_update_word(uint16_t *__p, uint16_t __value) { eeprom_write_word(__p, __value); }
void eeprom_update_dword(uint32_t *__p, uint32_t __value) { eeprom_write_dword(__p, __value); }
void eeprom_update_block(const void *__src, void *__dst, size_t __n) {
	eeprom_write_block(__src, __dst, __n);
}
