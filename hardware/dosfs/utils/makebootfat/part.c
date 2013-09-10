/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 2004 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "portable.h"

#include "part.h"
#include "fat.h"
#include "error.h"

static void part_entry_set(struct partition_entry* entry, unsigned offset, unsigned size, const struct disk_geometry* geometry)
{
	unsigned h, s, c;
	unsigned start, end;

	le_uint32_write(entry->start, offset);
	le_uint32_write(entry->size, size);

	start = offset;
	end = offset + size - 1;

	s = (start % geometry->sectors) + 1;
	start /= geometry->sectors;
	h = start % geometry->heads;
	start /= geometry->heads;
	c = start;

	entry->start_head = h;
	entry->start_seccyl[0] = (s & 0x3F) | ((c >> 2) & 0xc0);
	entry->start_seccyl[1] = c & 0xFF;

	s = (end % geometry->sectors) + 1;
	end /= geometry->sectors;
	h = end % geometry->heads;
	end /= geometry->heads;
	c = end;

	entry->end_head = h;
	entry->end_seccyl[0] = (s & 0x3F) | ((c >> 2) & 0xc0);
	entry->end_seccyl[1] = c & 0xFF;
}

/**
 * Create a partition table filled with a bootable FAT partition.
 * \param entry Partition entry to use. From 0 to 3.
 * The boot code is not modified.
 */
void part_setup(unsigned char* mbr, unsigned entry, unsigned fat_bit, unsigned fat_begin, unsigned fat_size, const struct disk_geometry* geometry)
{
	struct partition_table* part = (struct partition_table*)mbr;

	/* clear the table */
	memset(part->entries, 0, sizeof(part->entries));
	
	part->entries[entry].status = 0x80;
	if (fat_bit == 12)
		part->entries[entry].type = 1; /* FAT 12 */
	else if (fat_bit == 16)
		part->entries[entry].type = 6; /* FAT 16 (>32M) */
	else if (fat_bit == 32)
		part->entries[entry].type = 0xb; /* W95 FAT 32 */
	else
		part->entries[entry].type = 0;

	assert(fat_begin + fat_size <= geometry->size);

	part_entry_set(&part->entries[entry], fat_begin, fat_size, geometry);

	part->id[0] = 0x55;
	part->id[1] = 0xAA;
}

/**
 * Change the specified MBR sector to be a FAT boot sector.
 * \param mbr MBR to change.
 * \param boot FAT boot sector to imitate.
 * \param bit FAT bit (12, 16 or 32).
 * \param pos Position of the FAT boot sector on the disk (in number of sectors).
 */
int part_fat_setup(unsigned char* mbr, const unsigned char* boot, unsigned bit, unsigned pos)
{
	struct fat_boot_sector* fat;
	unsigned size_in_sector;
	unsigned code_start;

	if ((mbr[0] != 0xEB || mbr[2] != 0x90) && (mbr[0] != 0xE9)) {
		error_set("Invalid jump signature in the mbr sector.");
		return -1;
	}

	if (mbr[510] != 0x55 || mbr[511] != 0xAA) {
		error_set("Invalid end AA55 signature in the mbr sector.");
		return -1;
	}

	if (mbr[0] == 0xEB) {
		code_start = mbr[1] + 2;
	} else {
		error_set("Unsupported jump in the mbr sector.");
		return -1;
	}

	if (bit == 32 && code_start < 0x5A) {
		error_set("Invalid code start %X in the mbr sector (for fat32 it must be at least 5A).", code_start);
		return -1;
	}

	if (code_start < 0x3E) {
		error_set("Invalid code start %X in the mbr sector (it must be at least 3E).", code_start);
		return -1;
	}

	/* copy the FAT boot sector info */
	if (bit == 32)
		memcpy(mbr + 3, boot + 3, 0x5A - 3);
	else
		memcpy(mbr + 3, boot + 3, 0x3E - 3);

	fat = (struct fat_boot_sector*)mbr;

	/* increment the number of reserved sector */
	le_uint16_write(&fat->BPB_RsvdSecCnt, le_uint16_read(&fat->BPB_RsvdSecCnt) + pos);

	/* set the number of hidden sector */
	if (le_uint32_read(&fat->BPB_HiddSec) != pos) {
		error_set("Invalid number of hidden sector in the fat boot sector, it must be %d.", pos);
		return -1;
	}
	le_uint32_write(&fat->BPB_HiddSec, 0);

	/* increment the number of total sectors */
	if (le_uint32_read(fat->BPB_TotSec32) != 0)
		size_in_sector = le_uint32_read(fat->BPB_TotSec32);
	else
		size_in_sector = le_uint16_read(fat->BPB_TotSec16);
	size_in_sector += pos;
	if (bit == 32 || size_in_sector >= 0x10000) {
		le_uint16_write(fat->BPB_TotSec16, 0);
		le_uint32_write(fat->BPB_TotSec32, size_in_sector);
	} else {
		le_uint16_write(fat->BPB_TotSec16, size_in_sector);
		le_uint32_write(fat->BPB_TotSec32, 0);
	}

	/* increment other fat32 pointers */
	if (bit == 32) {
		le_uint16_write(fat->bit.fat32.BPB_FSInfo, le_uint16_read(fat->bit.fat32.BPB_FSInfo) + pos);

		/* no backup */
		le_uint16_write(fat->bit.fat32.BPB_BkBootSec, 0); 
	}

	return 0;
}

