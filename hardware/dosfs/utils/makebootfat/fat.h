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

#ifndef __FAT_H
#define __FAT_H

#include "disk.h"

#define FAT_CLUSTER_SIZE (SECTOR_SIZE*128)

#define FAT_TABLE_FREE 0
#define FAT_TABLE_EOC 0xFFFFFFFF
#define FAT_TABLE_BAD 0xFFFFFFF7

/**
 * FAT information.
 */
struct fat_info {
	unsigned fat_bit; /**< Number of bit in the fat. */
	unsigned fat_num; /**< Number of fat table. */
	unsigned fat_pos; /**< Fat table position (in sectors). */
	unsigned fat_size; /**< Fat size (in sectors). */
	unsigned data_pos; /**< Data position (in sectors). */
	unsigned data_size; /**< Data size (in sectors). */
	unsigned cluster_num; /**< Number of clusters. */
	unsigned cluster_size; /**< Number of sectors per cluster. */
	unsigned root_pos; /**< Root dir position (in sectors). */
	unsigned root_size; /**< Root dir size (in sectors). */
};

/**
 * FAT internal context.
 */
struct fat_context {
	struct disk_handle* h; ///< Handle of the device.
	unsigned h_pos; ///< Offset in the device (in sectors).
	unsigned h_size; ///< Size in the device (in sectors).

	unsigned char* table; /**< Fat table. */

	unsigned char tmp[FAT_CLUSTER_SIZE]; /**< Temporary data. */

	struct fat_info info; /**< Fat information. */

	struct disk_geometry geometry; /**< Disk geometry. */

	unsigned first_free; /**< First free cluster. */
};

struct fat_boot_sector {
	unsigned char BS_JumpBoot[3];
	char BS_OemName[8];
	unsigned char BPB_BytesPerSec[2];
	unsigned char BPB_SecPerClus;
	unsigned char BPB_RsvdSecCnt[2];
	unsigned char BPB_NumFATs;
	unsigned char BPB_RootEntCnt[2];
	unsigned char BPB_TotSec16[2];
	unsigned char BPB_Media;
	unsigned char BPB_FATSz16[2];
	unsigned char BPB_SecPerTrk[2];
	unsigned char BPB_NumHeads[2];
	unsigned char BPB_HiddSec[4];
	unsigned char BPB_TotSec32[4];
	union {
		struct {
			unsigned char BS_DrvNum;
			unsigned char BS_Reserved1;
			unsigned char BS_BootSig;
			unsigned char BS_VolID[4];
			char BS_VolLab[11];
			char BS_FilSysType[8];
			unsigned char BS_Code[448];
		} fat16;
		struct {
			unsigned char BPB_FATSz32[4];
			unsigned char BPB_ExtFlags[2];
			unsigned char BPB_FSVer[2];
			unsigned char BPB_RootClus[4];
			unsigned char BPB_FSInfo[2];
			unsigned char BPB_BkBootSec[2];
			unsigned char BPB_Reserved[12];
			unsigned char BS_DrvNum;
			unsigned char BS_Reserved1;
			unsigned char BS_BootSig;
			unsigned char BS_VolID[4];
			char BS_VolLab[11];
			char BS_FilSysType[8];
			unsigned char BS_Code[420];
		} fat32;
	} bit;
	unsigned char BS_Sign[2];
};

struct fat_fsinfo {
	unsigned char FSI_LeadSig[4];
	unsigned char FSI_Reserved1[480];
	unsigned char FSI_StrucSig[4];
	unsigned char FSI_Free_Count[4];
	unsigned char FSI_Nxt_Free[4];
	unsigned char FSI_Reserved2[12];
	unsigned char FSI_TrailSig[4];
};

#define FAT_ATTRIB_READONLY 0x01 
#define FAT_ATTRIB_HIDDEN 0x02 
#define FAT_ATTRIB_SYSTEM 0x04 
#define FAT_ATTRIB_VOLUME 0x08 
#define FAT_ATTRIB_DIRECTORY 0x10
#define FAT_ATTRIB_ARCHIVE 0x20
#define FAT_ATTRIB_LONGNAME (FAT_ATTRIB_READONLY | FAT_ATTRIB_HIDDEN | FAT_ATTRIB_SYSTEM | FAT_ATTRIB_VOLUME)

#define FAT_DIRENTRY_SIZE 32

/**
 * FAT directory entry.
 */
struct fat_direntry {
	char DE_Name[11];
	unsigned char DE_Attributes;
	unsigned char DE_Reserved;
	unsigned char DE_CrtTimeTenth;
	unsigned char DE_CrtTime[2];
	unsigned char DE_CrtDate[2];
	unsigned char DE_LstAccDate[2];
	unsigned char DE_ClusterH[2];
	unsigned char DE_WrtTime[2];
	unsigned char DE_WrtDate[2];
	unsigned char DE_ClusterL[2];
	unsigned char DE_FileSize[4];
};

/**
 * FAT directory entry for long names.
 */
struct fat_longentry {
	unsigned char DE_Ordinal;
	unsigned char DE_Name0[2];
	unsigned char DE_Name1[2];
	unsigned char DE_Name2[2];
	unsigned char DE_Name3[2];
	unsigned char DE_Name4[2];
	unsigned char DE_Attributes;
	unsigned char DE_Reserved;
	unsigned char DE_CheckSum;
	unsigned char DE_Name5[2];
	unsigned char DE_Name6[2];
	unsigned char DE_Name7[2];
	unsigned char DE_Name8[2];
	unsigned char DE_Name9[2];
	unsigned char DE_Name10[2];
	unsigned char DE_ClusterL[2];
	unsigned char DE_Name11[2];
	unsigned char DE_Name12[2];
};

struct fat_context* fat_open(struct disk_handle* h, unsigned pos, unsigned size, const struct disk_geometry* geometry);
int fat_close(struct fat_context* fat);
int fat_format(struct fat_context* fat, unsigned size, unsigned bit, unsigned sector_per_cluster, const char* oem, const char* label, unsigned serial, const struct disk_geometry* geometry);
int fat_entry_add(struct fat_context* fat, unsigned dir_cluster, const char* file_name, unsigned file_cluster, unsigned file_size, unsigned file_attrib, time_t file_time);
int fat_cluster_dir(struct fat_context* fat, unsigned root_cluster, unsigned* cluster, time_t time);
int fat_cluster_file(struct fat_context* fat, const char* file, unsigned* cluster, unsigned* size);
int fat_cluster_chain(struct fat_context* fat, unsigned cluster, unsigned* cluster_map, unsigned cluster_max);
int fat_sector_chain(struct fat_context* fat, unsigned cluster, unsigned* sector_map, unsigned sector_max);
int fat_boot_setup(unsigned char* boot, const unsigned char* code, unsigned bit);

#endif

