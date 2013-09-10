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

#ifndef __PART_H
#define __PART_H

#include "disk.h"

/**
 * Partition entry.
 */
struct partition_entry {
	unsigned char status; /**< Boot status. */
	unsigned char start_head; /**< Starting head. */
	unsigned char start_seccyl[2]; /**< Starting sector cylinder. */
	unsigned char type; /**< Type. */
	unsigned char end_head; /**< Ending head. */
	unsigned char end_seccyl[2]; /**< Ending sector cylinder. */
	unsigned char start[4]; /**< Start linear sector. */
	unsigned char size[4]; /**< Size in linear sector. */
};

/**
 * Partition table.
 */
struct partition_table {
	unsigned char bootcode[0x1BE]; /**< Boot code. */
	struct partition_entry entries[4]; /**< Partition entries. */
	unsigned char id[2]; /**< Partition ID 0xAA55. */
};

void part_setup(unsigned char* mbr, unsigned entry, unsigned fat_bit, unsigned fat_begin, unsigned fat_size, const struct disk_geometry* geometry);
int part_fat_setup(unsigned char* mbr, const unsigned char* boot, unsigned bit, unsigned pos);

#endif

