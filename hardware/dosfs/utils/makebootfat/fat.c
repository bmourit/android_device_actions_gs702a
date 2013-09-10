/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 2004, 2005 Andrea Mazzoleni
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

#include "fat.h"
#include "part.h"
#include "error.h"

static void fat_table_set(struct fat_context* fat, unsigned cluster, unsigned val)
{
	assert(cluster != 0 && cluster != 1 && cluster < fat->info.cluster_num + 2);

	if (fat->info.fat_bit == 32) {
		unsigned char* off = fat->table + cluster * 4;
		le_uint32_write(off, (le_uint32_read(off) & 0xF0000000) | (val & 0x0FFFFFFF));
	} else if (fat->info.fat_bit == 16) {
		unsigned char* off = fat->table + cluster * 2;
		le_uint16_write(off, val & 0xFFFF);
	} else if (fat->info.fat_bit == 12) {
		unsigned char* off = fat->table + cluster + cluster / 2;
		if ((cluster & 1) == 0) {
			off[0] = val & 0xFF;
			off[1] = (off[1] & 0xF0) | ((val >> 8) & 0x0F);
		} else {
			off[0] = (off[0] & 0x0F) | ((val << 4) & 0xF0);
			off[1] = (val >> 4) & 0xFF;
		}
	}
}

static unsigned fat_table_get(struct fat_context* fat, unsigned cluster)
{
	unsigned r;

	assert(cluster != 0 && cluster != 1 && cluster < fat->info.cluster_num + 2);

	if (fat->info.fat_bit == 32) {
		unsigned char* off = fat->table + cluster * 4;
		r = le_uint32_read(off) & 0x0FFFFFFF;
		if (r >= 0x0FFFFFF8)
			r = FAT_TABLE_EOC;
		if (r == 0x0FFFFFF7)
			r = FAT_TABLE_BAD;
	} else if (fat->info.fat_bit == 16) {
		unsigned char* off = fat->table + cluster * 2;
		r = le_uint16_read(off);
		if (r >= 0xFFF8)
			r = FAT_TABLE_EOC;
		if (r == 0xFFF7)
			r = FAT_TABLE_BAD;
	} else if (fat->info.fat_bit == 12) {
		unsigned char* off = fat->table + cluster + cluster / 2;
		if ((cluster & 1) == 0) {
			r = off[0] | (off[1] & 0x0F) << 8;
		} else {
			r = (off[0] >> 4) | (off[1] << 4);
		}
		if (r >= 0x0FF8)
			r = FAT_TABLE_EOC;
		if (r == 0x0FF7)
			r = FAT_TABLE_BAD;
	} else {
		assert(0);
	}

	return r;
}

struct fat_context* fat_open(struct disk_handle* h, unsigned pos, unsigned size, const struct disk_geometry* geometry)
{
	struct fat_context* fat;

	/* internal checks */
	if (sizeof(struct fat_direntry) != FAT_DIRENTRY_SIZE) {
		error_set("Invalid structure size.");
		return 0;
	}

	if (sizeof(struct fat_longentry) != FAT_DIRENTRY_SIZE) {
		error_set("Invalid structure size.");
		return 0;
	}

	if (sizeof(struct fat_boot_sector) != SECTOR_SIZE) {
		error_set("Invalid structure size.");
		return 0;
	}

	if (sizeof(struct fat_fsinfo) != SECTOR_SIZE) {
		error_set("Invalid structure size.");
		return 0;
	}

	if (sizeof(struct partition_table) != SECTOR_SIZE) {
		error_set("Invalid structure size.");
		return 0;
	}

	fat = malloc(sizeof(struct fat_context));
	if (!fat) {
		error_set("Low memory.");
		return 0;
	}

	fat->h = h;
	fat->h_pos = pos;
	fat->h_size = size;

	fat->table = 0;

	memset(&fat->info, 0, sizeof(fat->info));

	fat->geometry = *geometry;

	fat->first_free = 2;

	return fat;
}

int fat_close(struct fat_context* fat)
{
	struct fat_boot_sector boot;

	/* read the boot sector */
	if (disk_read(fat->h, fat->h_pos, &boot, 1) != 0)
		return -1;

	/* update the fat table */
	if (fat->table) {
		unsigned i;

		for(i=0;i<fat->info.fat_num;++i) {
			if (disk_write(fat->h, fat->h_pos + fat->info.fat_pos + i * fat->info.fat_size, fat->table, fat->info.fat_size) != 0)
				return -1;
		}

		if (fat->info.fat_bit == 32
			&& le_uint16_read(boot.bit.fat32.BPB_FSInfo) != 0) {
			struct fat_fsinfo fsinfo;
			unsigned freecount;
			unsigned firstfree;

			freecount = 0xFFFFFFFF;
			firstfree = 0xFFFFFFFF;
			for(i=2;i<fat->info.cluster_num + 2;++i) {
				if (fat_table_get(fat, i) == FAT_TABLE_FREE) {
					if (freecount == 0xFFFFFFFF)
						freecount = 1;
					else
						++freecount;
					if (firstfree == 0xFFFFFFFF)
						firstfree = i;
				}
			}

			if (disk_read(fat->h, fat->h_pos + le_uint16_read(boot.bit.fat32.BPB_FSInfo), &fsinfo, 1) != 0)
				return -1;

			le_uint32_write(fsinfo.FSI_Free_Count, freecount);
			le_uint32_write(fsinfo.FSI_Nxt_Free, firstfree);

			/* update the fsinfo sector */
			if (disk_write(fat->h, fat->h_pos + le_uint16_read(boot.bit.fat32.BPB_FSInfo), &fsinfo, 1) != 0)
				return -1;
		}

		free(fat->table);
	}

	if (fat->info.fat_bit == 32
		&& le_uint16_read(boot.bit.fat32.BPB_BkBootSec) != 0) {

		if (disk_write(fat->h, fat->h_pos + le_uint16_read(boot.bit.fat32.BPB_BkBootSec), &boot, 1) != 0)
			return -1;
	}

	free(fat);

	return 0;
}

/**
 * Formatted data to write.
 * This structure contains all the required data to write on a volume
 * to make it formatted with a FAT.
 */
struct fat_format {
	unsigned char bootraw[SECTOR_SIZE*32]; /**< Boot sectors data. */
	unsigned bootraw_pos; /**< Boot sectors position (in sectors). */
	unsigned bootraw_size; /**< Boot sectors size (in sectors). */
	unsigned char fatraw[SECTOR_SIZE]; /**< FAT sectors data. */
	unsigned fatraw_pos; /**< FAT sectors position (in sectors). */
	unsigned fatraw_size; /**< FAT sectors size (in sectors). */
	unsigned char rootraw[SECTOR_SIZE*128]; /**< Root sectors data. */
	unsigned rootraw_pos; /**< Root sectors position (in sectors). */
	unsigned rootraw_size; /**< Root sectors size (in sectors). */
};

/**
 * Check if the FAT size is correct.
 * \param fat_type Type of the FAT (12, 16, 32).
 * \param data_sec Number of data sectors.
 * \param fat_sec Number of fat scetors.
 * \param sector_per_cluster Sectors per cluster (1, 2, 4, 8, 16, 32, 64).
 * \return
 * - ==0 Ok.
 * - !=0 Error.
 */
static int fat_format_check(unsigned bit, unsigned data_sec, unsigned fat_sec, unsigned sector_per_cluster)
{
	/* check on fat size */
	if (bit == 12) {
		unsigned tst_fat_ent = data_sec / sector_per_cluster + 2;
		unsigned tst_fat_sec = (tst_fat_ent * 3 + (SECTOR_SIZE*2) - 1) / (SECTOR_SIZE*2);
		if (fat_sec < tst_fat_sec)
			return -1;
	} else if (bit == 16) {
		unsigned tst_fat_ent = data_sec / sector_per_cluster + 2;
		unsigned tst_fat_sec = (tst_fat_ent * 2 + SECTOR_SIZE - 1) / SECTOR_SIZE;
		if (fat_sec < tst_fat_sec)
			return -1;
	} else {
		unsigned tst_fat_ent = data_sec / sector_per_cluster + 2;
		unsigned tst_fat_sec = (tst_fat_ent * 4 + SECTOR_SIZE - 1) / SECTOR_SIZE;
		if (fat_sec < tst_fat_sec)
			return -1;
	}

	return 0;
}

static void strfcpy(char* dst, const char* s, char fill, unsigned l)
{
	unsigned i;

	for(i=0;i<l && s[i];++i)
		dst[i] = s[i];

	for(;i<l;++i)
		dst[i] = fill;
}

/**
 * Format a volume.
 * \param format Formatted data to write on the disk.
 * \param size_in_sector Size of the volume in number of sectors.
 * \param bit Type of the FAT (12, 16 or 32).
 * \param sector_per_cluster Sectors per cluster (1, 2, 4, 8, 16, 32, 64).
 * \param oem OEM name to use. Max 8 chars.
 * \param label Label to use. Max 11 chars.
 * \param serial Serial of the volume.
 * \return
 * - ==0 Ok.
 * - !=0 Error.
 */
static int fat_format_compute(struct fat_format* format, struct fat_info* info, unsigned size_in_sector, unsigned bit, unsigned sector_per_cluster, const char* oem, const char* label, unsigned serial, const struct disk_geometry* geometry, unsigned hidden)
{
	struct fat_boot_sector* boot;
	unsigned root_sec, tot_sec, fat_ent, data_sec, fat_sec;
	unsigned data_clus;
	unsigned char* fat;

	if (sector_per_cluster * SECTOR_SIZE > 32*1024)
		return -1;
	
	/* boot sector */
	boot = (struct fat_boot_sector*)format->bootraw;

	memset(boot, 0, SECTOR_SIZE);

	/* fake jump, it must be present to have a valid boot sector */
	boot->BS_JumpBoot[0] = 0xeb;
	boot->BS_JumpBoot[1] = 0x58;
	boot->BS_JumpBoot[2] = 0x90;

	strfcpy(boot->BS_OemName, oem, ' ', 8);

	le_uint16_write(&boot->BPB_BytesPerSec, SECTOR_SIZE);
	boot->BPB_SecPerClus = sector_per_cluster;
	if (bit == 32) 
		le_uint16_write(&boot->BPB_RsvdSecCnt, 32);
	else
		le_uint16_write(&boot->BPB_RsvdSecCnt, 1);
	boot->BPB_NumFATs = 1;
	if (bit == 32) {
		le_uint16_write(&boot->BPB_RootEntCnt, 0);
	} else {
		le_uint16_write(&boot->BPB_RootEntCnt, 512);
	}
	if (bit == 32 || size_in_sector >= 0x10000) {
		le_uint16_write(&boot->BPB_TotSec16, 0);
		le_uint32_write(&boot->BPB_TotSec32, size_in_sector);
	} else {
		le_uint16_write(&boot->BPB_TotSec16, size_in_sector);
		le_uint32_write(&boot->BPB_TotSec32, 0);
	}
	boot->BPB_Media = 0xF8; /* the media value is usually unused */

	root_sec = le_uint16_read(&boot->BPB_RootEntCnt) / 16;
	if (le_uint16_read(&boot->BPB_TotSec16))
		tot_sec = le_uint16_read(&boot->BPB_TotSec16);
	else
		tot_sec = le_uint32_read(&boot->BPB_TotSec32);

	/* compute the fat size (approximate in excess) */
	data_sec = tot_sec - le_uint16_read(&boot->BPB_RsvdSecCnt) - root_sec;
	fat_ent = data_sec / boot->BPB_SecPerClus + 2;

	if (bit == 12) {
		fat_sec = (fat_ent * 3 + (SECTOR_SIZE*2) - 1) / (SECTOR_SIZE*2);
	} else if (bit == 16) {
		fat_sec = (fat_ent * 2 + SECTOR_SIZE - 1) / SECTOR_SIZE;
	} else {
		fat_sec = (fat_ent * 4 + SECTOR_SIZE - 1) / SECTOR_SIZE;
	}

	/* reduce the fat size to the minimum */
	while (fat_sec > 0 && fat_format_check(bit, data_sec - (fat_sec-1) * boot->BPB_NumFATs, fat_sec - 1, boot->BPB_SecPerClus) == 0)
		--fat_sec;

	/* check on the fat size */
	if (fat_format_check(bit, data_sec - fat_sec * boot->BPB_NumFATs, fat_sec, boot->BPB_SecPerClus) != 0)
		return -1;

	/* set the fat size */
	if (bit != 32) {
		le_uint16_write(&boot->BPB_FATSz16, fat_sec);
	} else {
		le_uint16_write(&boot->BPB_FATSz16, 0);
		le_uint32_write(&boot->bit.fat32.BPB_FATSz32, fat_sec);
	}

	/* recompute the correct value of data_sec */
	data_sec = tot_sec - le_uint16_read(&boot->BPB_RsvdSecCnt) - fat_sec * boot->BPB_NumFATs - root_sec;
	
	/* check on fat type */
	data_clus = data_sec / boot->BPB_SecPerClus;
	if (data_clus < 4085) {
		if (bit != 12)
			return -1;
	} else if (data_clus < 65525) {
		if (bit != 16)
			return -1;
	} else {
		if (bit != 32)
			return -1;
	}

	/* check on fat size */
	if (bit == 12) {
		unsigned tst_fat_ent = data_sec / boot->BPB_SecPerClus + 2;
		unsigned tst_fat_sec = (tst_fat_ent * 3 + (SECTOR_SIZE*2) - 1) / (SECTOR_SIZE*2);
		if (fat_sec < tst_fat_sec)
			return -1;
	} else if (bit == 16) {
		unsigned tst_fat_ent = data_sec / boot->BPB_SecPerClus + 2;
		unsigned tst_fat_sec = (tst_fat_ent * 2 + SECTOR_SIZE - 1) / SECTOR_SIZE;
		if (fat_sec < tst_fat_sec)
			return -1;
	} else {
		unsigned tst_fat_ent = data_sec / boot->BPB_SecPerClus + 2;
		unsigned tst_fat_sec = (tst_fat_ent * 4 + SECTOR_SIZE - 1) / SECTOR_SIZE;
		if (fat_sec < tst_fat_sec)
			return -1;
	}

	le_uint16_write(&boot->BPB_SecPerTrk, geometry->sectors);
	le_uint16_write(&boot->BPB_NumHeads, geometry->heads);
	le_uint32_write(&boot->BPB_HiddSec, hidden);

	if (bit != 32) {
		boot->bit.fat16.BS_DrvNum = geometry->drive;
		boot->bit.fat16.BS_Reserved1 = 0;
		boot->bit.fat16.BS_BootSig = 0x29;
		le_uint32_write(&boot->bit.fat16.BS_VolID, serial);
		memset(boot->bit.fat16.BS_VolLab, ' ', 11);
		if (label)
			strfcpy(boot->bit.fat16.BS_VolLab, label, ' ', 11);
		if (bit == 16)
			strfcpy(boot->bit.fat16.BS_FilSysType, "FAT16", ' ', 8);
		else
			strfcpy(boot->bit.fat16.BS_FilSysType, "FAT12", ' ', 8);
	} else {
		le_uint16_write(&boot->bit.fat32.BPB_ExtFlags, 0);
		le_uint16_write(&boot->bit.fat32.BPB_FSVer, 0);
		le_uint32_write(&boot->bit.fat32.BPB_RootClus, 2);
		le_uint16_write(&boot->bit.fat32.BPB_FSInfo, 1);
		le_uint16_write(&boot->bit.fat32.BPB_BkBootSec, 6);
		memset(boot->bit.fat32.BPB_Reserved, 0, 12);
		boot->bit.fat32.BS_DrvNum = geometry->drive;
		boot->bit.fat32.BS_Reserved1 = 0;
		boot->bit.fat32.BS_BootSig = 0x29;
		le_uint32_write(&boot->bit.fat32.BS_VolID, serial);
		memset(boot->bit.fat32.BS_VolLab, ' ', 11);
		if (label)
			strfcpy(boot->bit.fat32.BS_VolLab, label, ' ', 11);
		strfcpy(boot->bit.fat32.BS_FilSysType, "FAT32", ' ', 8);
	}

	format->bootraw[510] = 0x55;
	format->bootraw[511] = 0xAA;

	/* clear reserved sector */
	memset(format->bootraw + SECTOR_SIZE, 0, (le_uint16_read(boot->BPB_RsvdSecCnt) - 1) * SECTOR_SIZE);

	if (bit == 32) {
		/* set fsinfo */
		struct fat_fsinfo* fsinfo = (struct fat_fsinfo*)(format->bootraw + le_uint16_read(&boot->bit.fat32.BPB_FSInfo)*SECTOR_SIZE);

		memset(fsinfo, 0, SECTOR_SIZE);

		le_uint32_write(&fsinfo->FSI_LeadSig, 0x41615252);
		memset(fsinfo->FSI_Reserved1, 0, 480);
		le_uint32_write(&fsinfo->FSI_StrucSig, 0x61417272);
		le_uint32_write(&fsinfo->FSI_Free_Count, 0xFFFFFFFF);
		le_uint32_write(&fsinfo->FSI_Nxt_Free, 0xFFFFFFFF);
		memset(fsinfo->FSI_Reserved2, 0, 12);
		le_uint32_write(&fsinfo->FSI_TrailSig, 0xAA550000);

		/* set the backup of the boot sectors */
		memcpy(format->bootraw + SECTOR_SIZE * le_uint16_read(&boot->bit.fat32.BPB_BkBootSec), format->bootraw, SECTOR_SIZE * 2);
	}

	format->bootraw_pos = 0;
	format->bootraw_size = le_uint16_read(&boot->BPB_RsvdSecCnt);

	/* fat */
	fat = format->fatraw;

	format->fatraw_pos = le_uint16_read(&boot->BPB_RsvdSecCnt);
	format->fatraw_size = 1;

	memset(fat, 0, SECTOR_SIZE);
	
	if (bit == 12) {
		fat[0] = boot->BPB_Media;
		fat[1] = 0xFF;
		fat[2] = 0xFF;
	} else if (bit == 16) {
		fat[0] = boot->BPB_Media;
		fat[1] = 0xFF;
		fat[2] = 0xFF;
		fat[3] = 0xFF;
	} else {
		fat[0] = boot->BPB_Media;
		fat[1] = 0xFF;
		fat[2] = 0xFF;
		fat[3] = 0xFF;
		fat[4] = 0xFF;
		fat[5] = 0xFF;
		fat[6] = 0xFF;
		fat[7] = 0xFF;
	}

	if (bit != 32) {
		/* root */
		struct fat_direntry* root = (struct fat_direntry*)format->rootraw;

		format->rootraw_pos = le_uint16_read(&boot->BPB_RsvdSecCnt) + fat_sec * boot->BPB_NumFATs;
		format->rootraw_size = root_sec;

		memset(root, 0, root_sec * SECTOR_SIZE);

		if (label) {
			strfcpy(root->DE_Name, label, ' ', 11);
			root->DE_Attributes = 0x08;
		}
	} else {
		/* dir */
		struct fat_direntry* dir = (struct fat_direntry*)format->rootraw;
		unsigned dir_sec = boot->BPB_SecPerClus; /* smallest size possible */

		format->rootraw_pos = le_uint16_read(&boot->BPB_RsvdSecCnt) + fat_sec * boot->BPB_NumFATs;
		format->rootraw_size = dir_sec;

		memset(dir, 0, dir_sec * SECTOR_SIZE);

		if (label) {
			strfcpy(dir->DE_Name, label, ' ', 11);
			dir->DE_Attributes = 0x08;
		}

		fat[8] = 0xFF;
		fat[9] = 0xFF;
		fat[10] = 0xFF;
		fat[11] = 0xFF;
	}

	if (info) {
		info->fat_bit = bit;
		info->fat_num = boot->BPB_NumFATs;
		info->fat_pos = le_uint16_read(&boot->BPB_RsvdSecCnt);
		info->fat_size = fat_sec;
		if (bit != 32)
			info->data_pos = le_uint16_read(&boot->BPB_RsvdSecCnt) + fat_sec * boot->BPB_NumFATs + root_sec;
		else
			info->data_pos = le_uint16_read(&boot->BPB_RsvdSecCnt) + fat_sec * boot->BPB_NumFATs;
		info->data_size = data_sec;
		info->cluster_num = data_clus;
		info->cluster_size = sector_per_cluster;
		info->root_pos = le_uint16_read(&boot->BPB_RsvdSecCnt) + fat_sec * boot->BPB_NumFATs;
		if (bit != 32) {
			info->root_size = root_sec;
		} else {
			info->root_size = boot->BPB_SecPerClus;
		}
	}

	return 0;
}

/**
 * Format the disk.
 * \return
 * - < 0 Error.
 * - == 0 Success.
 * - > 0 Invalid argument.
 */
int fat_format(struct fat_context* fat, unsigned size, unsigned bit, unsigned sector_per_cluster, const char* oem, const char* label, unsigned serial, const struct disk_geometry* geometry)
{
	struct fat_format format;
	int r;

	r = fat_format_compute(&format, &fat->info, size, bit, sector_per_cluster, oem, label, serial, geometry, fat->h_pos + geometry->start);
	if (r != 0)
		return 1;

	/* setup the new fat table */
	free(fat->table);
	fat->table = malloc(fat->info.fat_size * SECTOR_SIZE);
	if (!fat->table) {
		error_set("Low memory.");
		return -1;
	}

	/* set the table data */
	memset(fat->table, 0, fat->info.fat_size * SECTOR_SIZE);
	memcpy(fat->table + (format.fatraw_pos - fat->info.fat_pos) * SECTOR_SIZE, format.fatraw, format.fatraw_size * SECTOR_SIZE);

	/* write the boot sectors */
	r = disk_write(fat->h, fat->h_pos + format.bootraw_pos, format.bootraw, format.bootraw_size);
	if (r != 0)
		return -1;

	/* write the root sectors */
	r = disk_write(fat->h, fat->h_pos + format.rootraw_pos, format.rootraw, format.rootraw_size);
	if (r != 0)
		return -1;

	return 0;
}

static unsigned fat_table_allocate(struct fat_context* fat, unsigned cluster)
{
	/* allocate a new cluster */
	unsigned i;

	for(i=fat->first_free;i<fat->info.cluster_num + 2;++i)
		if (fat_table_get(fat, i) == FAT_TABLE_FREE)
			break;

	fat->first_free = i;

	if (i == fat->info.cluster_num + 2) {
		error_set("Disk full.");
		return 0;
	}

	if (cluster != FAT_TABLE_EOC)
		fat_table_set(fat, cluster, i);

	fat_table_set(fat, i, FAT_TABLE_EOC);

	return i;
}

static unsigned fat_cluster_to_sector(struct fat_context* fat, unsigned cluster)
{
	assert(cluster >= 2 && cluster < fat->info.cluster_num + 2);

	return fat->info.data_pos + (cluster - 2) * fat->info.cluster_size;
}

static int fat_cluster_read(struct fat_context* fat, unsigned cluster, void* data)
{
	return disk_read(fat->h, fat->h_pos + fat_cluster_to_sector(fat, cluster), data, fat->info.cluster_size);
}

static int fat_cluster_write(struct fat_context* fat, unsigned cluster, const void* data)
{
	return disk_write(fat->h, fat->h_pos + fat_cluster_to_sector(fat, cluster), data, fat->info.cluster_size);
}

/**
 * Setup the short name for a dir entry.
 * \param entry Entry to setup.
 * \param file_name File name.
 * \return
 *  - ==1 A long name is required.
 *  - ==0 A long name isn't required.
 */
static int fat_name_set(struct fat_direntry* entry, const char* file_name)
{
	const char* dot;
	unsigned i;
	int need_long = 0;

	dot = strchr(file_name, '.');

	memset(entry->DE_Name, ' ', 11);

	if (dot) {
		unsigned l = dot - file_name;
		if (l > 8) {
			l = 8;
			need_long = 1;
		}
		memcpy(entry->DE_Name, file_name, l);

		++dot;

		l = strlen(dot);
		if (l > 3) {
			l = 3;
			need_long = 1;
		} else if (l == 0) {
			/* dot without extension */
			need_long = 1;
		}

		memcpy(entry->DE_Name + 8, dot, l);
	} else {
		unsigned l = strlen(file_name);
		if (l > 8) {
			l = 8;
			need_long = 1;
		}

		memcpy(entry->DE_Name, file_name, l);
	}

	if (!need_long) {
		for(i=0;i<11;++i) {
			if (islower(entry->DE_Name[i])) {
				need_long = 1;
				break;
			}
		}
	}

	for(i=0;i<11;++i)
		entry->DE_Name[i] = toupper(entry->DE_Name[i]);

	return need_long;
}

/**
 * Setup the long and short name for a dir entry.
 * \param entry Vector of directory entries. At least 21 entries are required.
 * \param file_name File name.
 * \return Number of directory entries used.
 */
static unsigned fat_long_set(struct fat_direntry* entry, const char* file_name)
{
	unsigned l;
	unsigned n;
	unsigned i;
	unsigned char checksum;

	if (!fat_name_set(entry, file_name)) {
		/* long name not required */
		return 1;
	}

	l = strlen(file_name);

	/* max length is 255 chars */
	if (l > 255)
		l = 255;

	n = (l + 12) / 13;

	memcpy(entry + n, entry, FAT_DIRENTRY_SIZE);

	checksum = 0;
	for(i=0;i<11;++i) {
		checksum = (checksum >> 1) | (checksum << 7);
		checksum += entry[n].DE_Name[i];
	}

	for(i=0;i<n;++i) {
		unsigned short ws[13];
		unsigned k;
		struct fat_longentry* longentry = (struct fat_longentry*)&entry[n-i-1];

		for(k=0;k<13;++k) {
			unsigned j = k + i * 13;
			if (j == l)
				ws[k] = 0;
			else if (j > l)
				ws[k] = 0xFFFF;
			else
				ws[k] = (unsigned char)file_name[j]; /* fake unicode conversion */
		}

		longentry->DE_Ordinal = i + 1;
		if (i+1 == n)
			longentry->DE_Ordinal |= 0x40; /* mark the last entry */
		le_uint16_write(longentry->DE_Name0, ws[0]);
		le_uint16_write(longentry->DE_Name1, ws[1]);
		le_uint16_write(longentry->DE_Name2, ws[2]);
		le_uint16_write(longentry->DE_Name3, ws[3]);
		le_uint16_write(longentry->DE_Name4, ws[4]);
		longentry->DE_Attributes = FAT_ATTRIB_LONGNAME;
		longentry->DE_Reserved = 0;
		longentry->DE_CheckSum = checksum;
		le_uint16_write(longentry->DE_Name5, ws[5]);
		le_uint16_write(longentry->DE_Name6, ws[6]);
		le_uint16_write(longentry->DE_Name7, ws[7]);
		le_uint16_write(longentry->DE_Name8, ws[8]);
		le_uint16_write(longentry->DE_Name9, ws[9]);
		le_uint16_write(longentry->DE_Name10, ws[10]);
		le_uint16_write(longentry->DE_ClusterL, 0);
		le_uint16_write(longentry->DE_Name11, ws[11]);
		le_uint16_write(longentry->DE_Name12, ws[12]);
	}

	return n + 1;
}

/**
 * Create a new file.
 * \param fat FAT context.
 * \param file File to copy.
 * \param root_cluster Cluster of the parent dir. 0 for root directory.
 * \param cluster Pointer to store the first allocated cluster.
 * \param size Pointer to store the file size.
 */
int fat_cluster_file(struct fat_context* fat, const char* file, unsigned* cluster, unsigned* size)
{
	FILE* f;
	struct stat st;
	unsigned c;
	unsigned s;

	f = fopen(file, "rb");
	if (!f) {
		error_set("Error reading the file %s.", file);
		goto err;
	}

	if (fstat(fileno(f), &st) != 0) {
		error_set("Error reading the file %s.", file);
		goto err_close;
	}

	s = st.st_size;
	*size = st.st_size;

	c = FAT_TABLE_EOC;
	*cluster = FAT_TABLE_EOC;

	while (s) {
		unsigned run = fat->info.cluster_size * SECTOR_SIZE;
		if (run > s)
			run = s;

		if (c == FAT_TABLE_EOC) {
			c = fat_table_allocate(fat, c);
			if (!c)
				goto err_close;
			*cluster = c;
		} else {
			c = fat_table_allocate(fat, c);
			if (!c)
				goto err_close;
		}

		if (fread(fat->tmp, run, 1, f) != 1) {
			error_set("Error reading the file %s.", file);
			goto err_close;
		}

		if (run < fat->info.cluster_size * SECTOR_SIZE)
			memset(fat->tmp + run, 0, fat->info.cluster_size * SECTOR_SIZE - run);

		if (fat_cluster_write(fat, c, fat->tmp) != 0)
			goto err_close;

		s -= run;
	}

	fclose(f);

	return 0;

err_close:
	fclose(f);
err:
	return -1;
}

/**
 * Get the cluster chain to the end.
 * \param fat FAT context.
 * \param cluster First cluster. Returned also in cluster_map[0].
 * \param cluster_map Vector to store the cluster numbers.
 * \param cluster_max Max size of the vector.
 * \return Elements in the vector or negative on error.
 */
int fat_cluster_chain(struct fat_context* fat, unsigned cluster, unsigned* cluster_map, unsigned cluster_max)
{
	unsigned mac;

	mac = 0;

	while (cluster != FAT_TABLE_EOC) {
		if (cluster == FAT_TABLE_FREE || cluster == FAT_TABLE_BAD)
			return -1;

		if (mac >= cluster_max)
			return -1;

		cluster_map[mac] = cluster;
		++mac;

		cluster = fat_table_get(fat, cluster);
	}

	return mac;
}

/**
 * Get the cluster chain to the end.
 * \param fat FAT context.
 * \param cluster First cluster.
 * \param sector_map Vector to store the sector numbers.
 * \param sector_max Max size of the vector.
 * \return Elements in the vector or negative on error.
 */
int fat_sector_chain(struct fat_context* fat, unsigned cluster, unsigned* sector_map, unsigned sector_max)
{
	unsigned mac;

	mac = 0;

	while (cluster != FAT_TABLE_EOC) {
		unsigned i;

		if (cluster == FAT_TABLE_FREE || cluster == FAT_TABLE_BAD)
			return -1;

		for(i=0;i<fat->info.cluster_size;++i) {
			if (mac >= sector_max)
				return -1;

			sector_map[mac] = fat_cluster_to_sector(fat, cluster) + i;
			++mac;
		}

		cluster = fat_table_get(fat, cluster);
	}

	return mac;
}

static void fat_date(unsigned char* w, time_t t)
{
	struct tm* tm;
	int dos_year;
	int dos_month;
	int dos_day;

	tm = gmtime(&t);

	if (!tm) {
		le_uint16_write(w, 0);
		return;
	}

	dos_day = tm->tm_mday;
	dos_month = tm->tm_mon + 1;
	dos_year = tm->tm_year + 1900 - 1980;

	if (dos_day < 0 || dos_day > 31
		|| dos_month < 1 || dos_month > 12
		|| dos_year < 0 || dos_year > 119) {
		le_uint16_write(w, 0);
		return;
	}

	le_uint16_write(w, dos_day | (dos_month << 5) | (dos_year << 9));
}

static void fat_time(unsigned char* w, time_t t)
{
	struct tm* tm;
	int dos_hour;
	int dos_min;
	int dos_sec;

	tm = gmtime(&t);

	if (!tm) {
		le_uint16_write(w, 0);
		return;
	}

	dos_sec = tm->tm_sec / 2;
	if (dos_sec == 30)
		dos_sec = 29;
	dos_min = tm->tm_min;
	dos_hour = tm->tm_hour;

	if (dos_sec < 0 || dos_sec > 29
		|| dos_min < 0 || dos_min > 59
		|| dos_hour < 0 || dos_hour > 23) {
		le_uint16_write(w, 0);
		return;
	}

	le_uint16_write(w, dos_sec | (dos_min << 5) | (dos_hour << 11));
}

/**
 * Create a new directory.
 * \param fat FAT context.
 * \param root_cluster Cluster of the parent dir. 0 for root directory.
 * \param cluster Pointer to store the first allocated cluster.
 */
int fat_cluster_dir(struct fat_context* fat, unsigned root_cluster, unsigned* cluster, time_t time)
{
	unsigned c;
	struct fat_direntry* entry;

	c = fat_table_allocate(fat, FAT_TABLE_EOC);
	if (!c)
		return -1;

	/* clear the cluster */
	memset(fat->tmp, 0, fat->info.cluster_size * SECTOR_SIZE);

	entry = (struct fat_direntry*)fat->tmp;

	strfcpy(entry[0].DE_Name, ".", ' ', 11);
	entry[0].DE_Attributes = FAT_ATTRIB_DIRECTORY;
	entry[0].DE_Reserved = 0;
	entry[0].DE_CrtTimeTenth = 0;
	le_uint16_write(entry[0].DE_CrtTime, 0);
	le_uint16_write(entry[0].DE_CrtDate, 0);
	le_uint16_write(entry[0].DE_LstAccDate, 0);
	le_uint16_write(entry[0].DE_ClusterH, c >> 16);
	fat_time(entry[0].DE_WrtTime, time);
	fat_date(entry[0].DE_WrtDate, time);
	le_uint16_write(entry[0].DE_ClusterL, c & 0xFFFF);
	le_uint32_write(entry[0].DE_FileSize, 0);
	strfcpy(entry[1].DE_Name, "..", ' ', 11);
	entry[1].DE_Attributes = FAT_ATTRIB_DIRECTORY;
	entry[1].DE_Reserved = 0;
	entry[1].DE_CrtTimeTenth = 0;
	le_uint16_write(entry[1].DE_CrtTime, 0);
	le_uint16_write(entry[1].DE_CrtDate, 0);
	le_uint16_write(entry[1].DE_LstAccDate, 0);
	le_uint16_write(entry[1].DE_ClusterH, root_cluster >> 16);
	fat_time(entry[1].DE_WrtTime, time);
	fat_date(entry[1].DE_WrtDate, time);
	le_uint16_write(entry[1].DE_ClusterL, root_cluster & 0xFFFF);
	le_uint32_write(entry[1].DE_FileSize, 0);

	if (fat_cluster_write(fat, c, fat->tmp) != 0)
		return -1;

	*cluster = c;

	return 0;
}

static int fat_chain_read(struct fat_context* fat, unsigned cluster, unsigned char** data, unsigned* data_size)
{
	if (cluster == 0 && fat->info.root_pos > fat->info.data_pos) {
		/* get the cluster position of the root directory */
		cluster = (fat->info.root_pos - fat->info.data_pos) / fat->info.cluster_size + 2;
	}

	if (cluster == 0) {
		*data_size = fat->info.root_size;
		*data = malloc(fat->info.root_size * SECTOR_SIZE);
		if (!*data) {
			error_set("Low memory.");
			return -1;
		}

		if (disk_read(fat->h, fat->h_pos + fat->info.root_pos, *data, fat->info.root_size) != 0)
			return -1;
	} else {
		unsigned c, i;

		i = 0;
		c = cluster;
		while (c != FAT_TABLE_EOC) {
			c = fat_table_get(fat, c);
			++i;
		}

		*data_size = i * fat->info.cluster_size;
		*data = malloc(i * fat->info.cluster_size * SECTOR_SIZE);
		if (!*data) {
			error_set("Low memory.");
			return -1;
		}

		c = cluster;
		i = 0;
		while (c != FAT_TABLE_EOC) {
			if (fat_cluster_read(fat, c, *data + i * fat->info.cluster_size * SECTOR_SIZE) != 0)
				return -1;

			c = fat_table_get(fat, c);

			++i;
		}
	}

	return 0;
}

static int fat_chain_write(struct fat_context* fat, unsigned cluster, unsigned char* data, unsigned data_size)
{
	if (cluster == 0 && fat->info.root_pos > fat->info.data_pos) {
		/* get the cluster position of the root directory */
		cluster = (fat->info.root_pos - fat->info.data_pos) / fat->info.cluster_size + 2;
	}

	if (cluster == 0) {
		if (data_size != fat->info.root_size) {
			error_set("Root directory full.");
			return -1;
		}

		if (disk_write(fat->h, fat->h_pos + fat->info.root_pos, data, fat->info.root_size) != 0)
			return -1;
	} else {
		unsigned c, i;

		i = 0;
		c = cluster;

		if (fat_cluster_write(fat, c, data + i * fat->info.cluster_size * SECTOR_SIZE) != 0)
			return -1;

		++i;

		while (i * fat->info.cluster_size < data_size) {
			unsigned next = fat_table_get(fat, c);

			if (next == FAT_TABLE_EOC)
				next = fat_table_allocate(fat, c);

			c = next;
			if (!c)
				return -1;

			if (fat_cluster_write(fat, c, data + i * fat->info.cluster_size * SECTOR_SIZE) != 0)
				return -1;

			++i;
		}
	}

	return 0;
}

/**
 * Add an entry in a directory.
 * \param fat FAT context.
 * \param dir_cluster First directory cluster. 0 for root directory.
 * \param file_name File name.
 * \param file_cluster File first cluster.
 * \param file_size File size.
 * \param file_attrib File attributes.
 * \param file_time File modification time.
 */
int fat_entry_add(struct fat_context* fat, unsigned dir_cluster, const char* file_name, unsigned file_cluster, unsigned file_size, unsigned file_attrib, time_t file_time)
{
	struct fat_direntry longentry[21]; /* 20 for long name + 1 for short name */
	struct fat_direntry* entry;
	unsigned n, i;
	unsigned char* data;
	unsigned data_size;
	unsigned entry_count;

	n = fat_long_set(longentry, file_name);

	entry = longentry + n - 1;

	entry->DE_Attributes = file_attrib;
	entry->DE_Reserved = 0;
	entry->DE_CrtTimeTenth = 0;
	le_uint16_write(entry->DE_CrtTime, 0);
	le_uint16_write(entry->DE_CrtDate, 0);
	le_uint16_write(entry->DE_LstAccDate, 0);
	le_uint16_write(entry->DE_ClusterH, file_cluster >> 16);
	fat_time(entry->DE_WrtTime, file_time);
	fat_date(entry->DE_WrtDate, file_time);
	le_uint16_write(entry->DE_ClusterL, file_cluster & 0xFFFF);
	le_uint32_write(entry->DE_FileSize, file_size);

	if (fat_chain_read(fat, dir_cluster, &data, &data_size) != 0)
		return -1;

	entry = (struct fat_direntry*)data;

	entry_count = data_size * (SECTOR_SIZE / FAT_DIRENTRY_SIZE);

	for(i=0;i<entry_count;++i) {
		if (entry[i].DE_Name[0] == 0) {
			break;
		}
	}

	/* add new cluster */
	while (i + n > entry_count) {
		data = realloc(data, (data_size + fat->info.cluster_size) * SECTOR_SIZE);
		if (!data) {
			error_set("Low memory.");
			return -1;
		}

		entry = (struct fat_direntry*)data;

		/* clear the new space */
		memset(data + data_size * SECTOR_SIZE, 0, fat->info.cluster_size * SECTOR_SIZE);

		data_size += fat->info.cluster_size;

		entry_count += fat->info.cluster_size * (SECTOR_SIZE / FAT_DIRENTRY_SIZE);
	}

	memcpy(&entry[i], longentry, n * FAT_DIRENTRY_SIZE);

	if (fat_chain_write(fat, dir_cluster, data, data_size) != 0)
		return -1;

	free(data);

	return 0;
}

/**
 * Change the boot sector including the specified boot code.
 * \param boot Boot sector to change.
 * \param code Boot sector containing the boot code.
 * \param bit FAT bit (12, 16 or 32).
 */
int fat_boot_setup(unsigned char* boot, const unsigned char* code, unsigned bit)
{
	unsigned code_start;

	/* check the provided boot sector */
	if ((code[0] != 0xEB || code[2] != 0x90) && (code[0] != 0xE9)) {
		error_set("Invalid jump signature in the boot sector.");
		return -1;
	}

	if (code[0] == 0xEB) {
		code_start = code[1] + 2;
	} else {
		error_set("Unsupported jump in the boot sector.");
		return -1;
	}

	if (bit == 32 && code_start < 0x5A) {
		error_set("Invalid code start %X in the boot sector (for fat32 it must be at least 5A).", code_start);
		return -1;
	}

	if (code_start < 0x3E) {
		error_set("Invalid code start %X in the boot sector (it must be at least 3E).", code_start);
		return -1;
	}

	/* copy the code */
	memcpy(boot, code, 3); /* jump */
	memcpy(boot + code_start, code + code_start, 510 - code_start); /* code */

	return 0;
}

