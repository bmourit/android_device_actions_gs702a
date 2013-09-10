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

int fatboot_format(struct fat_context* fat, const unsigned char* boot12, const unsigned char* boot16, const unsigned char* boot32, const char* oem, const char* label, unsigned serial, const struct disk_geometry* geometry, int syslinux2)
{
	unsigned sector_per_cluster;
	int r;

	/* start with a reasonable sector_per_cluster to limit the FAT size */
	if (fat->h_size <= 16*1024*2) /* 16 MB */
		sector_per_cluster = 1;
	else if (fat->h_size <= 64*1024*2) /* 64 MB */
		sector_per_cluster = 2;
	else if (fat->h_size <= 256*1024*2) /* 256 MB */
		sector_per_cluster = 4;
	else if (fat->h_size <= 1024*1024*2) /* 1 GB */
		sector_per_cluster = 8;
	else if (fat->h_size <= 4*1024*1024*2) /* 4 GB */
		sector_per_cluster = 16;
	else
		sector_per_cluster = 32;

	r = -1;

	/* syslinux 2.xx doesn't support 64 and 128 sectors per cluster */
	while (sector_per_cluster <= 32 || (!syslinux2 && sector_per_cluster <= 128)) {

		if (boot12 != 0) {
			r = fat_format(fat, fat->h_size, 12, sector_per_cluster, oem, label, serial, geometry);
			if (r < 0)
				return -1;
			if (r == 0)
				break;
		}

		if (boot16 != 0) {
			r = fat_format(fat, fat->h_size, 16, sector_per_cluster, oem, label, serial, geometry);
			if (r < 0)
				return -1;
			if (r == 0)
				break;
		}

		if (boot32 != 0) {
			/* syslinux 2.xx supports only fat12 and fat16. It doesn't support fat32 */
			if (!syslinux2) {
				r = fat_format(fat, fat->h_size, 32, sector_per_cluster, oem, label, serial, geometry);
				if (r < 0)
					return -1;
				if (r == 0)
					break;
			}
		}

		sector_per_cluster *= 2;
	}

	if (r != 0) {
		error_set("Unsupported disk size.");
		return -1;
	}

	/* update the boot sector */
	if (disk_read(fat->h, fat->h_pos, fat->tmp, 1) != 0)
		return -1;

	switch (fat->info.fat_bit) {
	case 12 :
		if (fat_boot_setup(fat->tmp, boot12, fat->info.fat_bit) != 0)
			return -1;
		break;
	case 16 :
		if (fat_boot_setup(fat->tmp, boot16, fat->info.fat_bit) != 0)
			return -1;
		break;
	case 32 :
		if (fat_boot_setup(fat->tmp, boot32, fat->info.fat_bit) != 0)
			return -1;
		break;
	}

	if (disk_write(fat->h, fat->h_pos, fat->tmp, 1) != 0)
		return -1;

	return 0;
}

/**
 * Update the syslinux 3.xx boot sector and ldlinux.sys file.
 * \param sec_map Vector of sector numbers for the ldlinux.sys file. 0 is the first sector of the partition.
 * \param sec_mac Number of sectors in the vector.
 * \param size Real size of the ldlinux.sys file.
 */
int fatboot_syslinux3(struct fat_context* fat, unsigned* sec_map, unsigned sec_mac, unsigned size)
{
	unsigned char tmp[SECTOR_SIZE];
	unsigned char* data;
	unsigned i, p;
	unsigned csum;
	unsigned csum_mac;

	if (sec_mac > 65) {
		error_set("Invalid ldlinux.sys. Too big.");
		return -1;
	}

	/* read the boot sector */
	if (disk_read(fat->h, fat->h_pos, tmp, 1) != 0)
		return -1;

	/* check the signature */
	if (le_uint32_read(tmp + 0x1F8) != 0xDEADBEEF) {
		error_set("Invalid ldlinux.bss. Missing tag word.");
		return -1;
	}

	/* update the index of the first loader sector */
	le_uint32_write(tmp + 0x1F8, sec_map[0]);

	/* update the boot sector */
	if (disk_write(fat->h, fat->h_pos, tmp, 1) != 0)
		return -1;

	data = malloc(sec_mac * SECTOR_SIZE);
	if (!data) {
		error_set("Low memory.");
		return -1;
	}

	/* read all the ldlinux.sys sectors */
	for(i=0;i<sec_mac;++i) {
		if (disk_read(fat->h, fat->h_pos + sec_map[i], data + i*SECTOR_SIZE, 1) != 0)
			return -1;
	}

	/* search for 0x3eb202fe to find the patch area */
	for(i=0;i<SECTOR_SIZE;i+=4)
		if (le_uint32_read(data + i) == 0x3eb202fe)
			break;
	if (i==SECTOR_SIZE) {
		error_set("Invalid ldlinux.sys. Missing tag word.");
		return -1;
	}

	/* start of the patch area */
	p = i + 8;

	csum_mac = size / 4;

	le_uint16_write(data + p, csum_mac); /* size in complete dwords */
	le_uint16_write(data + p + 2, sec_mac - 1); /* number of sectors without the first one */

	/* clear all */
	memset(data + p + 8, 0, 64*4);

	for(i=1;i<sec_mac;++i) {
		le_uint32_write(data + p + 8 + (i-1) * 4, sec_map[i]);
	}

	le_uint32_write(data + p + 4, 0); /* zero the checksum */

	csum = 0x3eb202fe;
	for(i=0;i<csum_mac;++i)
		csum -= le_uint32_read(data + i*4);

	le_uint32_write(data + p + 4, csum); /* new checksum */

	/* rewrite only the first sector */
	if (disk_write(fat->h, fat->h_pos + sec_map[0], data, 1) != 0)
		return -1;

	free(data);

	return 0;
}

struct string_list {
	char* path;
	struct string_list* next;
};

int fatboot_copy(struct fat_context* fat, unsigned cluster, const char* image, struct string_list* exclude_list, time_t force_time)
{
	DIR* d;
	struct dirent* dd;

	d = opendir(image);
	if (!d) {
		error_set("Error opening the directory %s.", image);
		return -1;
	}

	while ((dd = readdir(d)) != 0) {
		char path[512];
		struct stat st;
		unsigned file_cluster;
		unsigned file_size;
		unsigned file_attrib;
		time_t file_time;
		struct string_list* e;

		if (strcmp(dd->d_name, ".") == 0 || strcmp(dd->d_name, "..") == 0)
			continue;

		sprintf(path, "%s/%s", image, dd->d_name);

		e = exclude_list;
		while (e) {
			if (strcmp(path, e->path) == 0)
				break;
			e = e->next;
		}
		if (e)
			continue;

		if (stat(path, &st) != 0) {
			error_set("Error reading the file %s.", path);
			return -1;
		}

		if (S_ISDIR(st.st_mode)) {
			file_size = 0;
			file_attrib = FAT_ATTRIB_DIRECTORY;
			if (force_time)
				file_time = force_time;
			else
				file_time = st.st_mtime;

			if (fat_cluster_dir(fat, cluster, &file_cluster, file_time) != 0)
				return -1;

			if (fatboot_copy(fat, file_cluster, path, exclude_list, force_time) != 0)
				return -1;
		} else if (S_ISREG(st.st_mode)) {
			file_attrib = 0;
			if (force_time)
				file_time = force_time;
			else
				file_time = st.st_mtime;

			if (fat_cluster_file(fat, path, &file_cluster, &file_size) != 0)
				return -1;
		} else {
			return -1;
		}

		if (fat_entry_add(fat, cluster, dd->d_name, file_cluster, file_size, file_attrib, file_time) != 0)
			return -1;
	}

	closedir(d);

	return 0;
}

int fatboot_copytoroot(struct fat_context* fat, const char* file, unsigned* cluster, unsigned* size, time_t force_time)
{
	struct stat st;
	unsigned file_cluster;
	unsigned file_size;
	unsigned file_attrib;
	time_t file_time;
	const char* name;
	const char* slash;

	if (stat(file, &st) != 0) {
		error_set("Error reading the file %s.", file);
		return -1;
	}

	file_attrib = FAT_ATTRIB_READONLY;
	if (force_time)
		file_time = force_time;
	else
		file_time = st.st_mtime;

	name =  strrchr(file, '/');
	slash = strrchr(file, '\\');
	if (!name || (slash && slash > name))
		name = slash;
	if (!name)
		name = file;
	else
		++name;

	if (fat_cluster_file(fat, file, &file_cluster, &file_size) != 0)
		return -1;

	if (fat_entry_add(fat, 0, name, file_cluster, file_size, file_attrib, file_time) != 0)
		return -1;

	/* report info */
	*cluster = file_cluster;
	*size = file_size;

	return 0;
}

void version() {
	printf( PACKAGE " v" VERSION " by Andrea Mazzoleni\n");
}

void usage() {
	version();

	printf("Usage: makebootfat [options] dir\n");
	printf("\n");
	printf("Options:\n");
	printf("  " SWITCH_GETOPT_LONG("-o, --output DEVICE", "-o") "  Select the output device\n");
	printf("  " SWITCH_GETOPT_LONG("-b, --boot FILE    ", "-b") "  Select the boot sector image\n");
	printf("  " SWITCH_GETOPT_LONG("-m, --mbr FILE     ", "-m") "  Select the mbr sector image\n");
	printf("  " SWITCH_GETOPT_LONG("-c, --copy FILE    ", "-c") "  Copy a file in the root directory\n");
	printf("  " SWITCH_GETOPT_LONG("-x, --exclude FILE ", "-x") "  Exclude files\n");
	printf("  " SWITCH_GETOPT_LONG("-X, --syslinux2    ", "-X") "  Enforce syslinux 2.xx limitations\n");
	printf("  " SWITCH_GETOPT_LONG("-Y, --syslinux3    ", "-Y") "  Enforce syslinux 3.xx limitations\n");
	printf("  " SWITCH_GETOPT_LONG("-Z, --zip          ", "-Z") "  Enforce ZIP-Drive compatibility if possible\n");
	printf("  " SWITCH_GETOPT_LONG("-F, --mbrfat       ", "-F") "  Change the MBR to imitate a FAT boot sector\n");
	printf("  " SWITCH_GETOPT_LONG("-L, --label LABEL  ", "-L") "  Volume label\n");
	printf("  " SWITCH_GETOPT_LONG("-O, --oem OEM      ", "-O") "  Volume oem\n");
	printf("  " SWITCH_GETOPT_LONG("-S, --serial SERIAL", "-S") "  Volume serial\n");
	printf("  " SWITCH_GETOPT_LONG("-E, --drive DRIVE  ", "-E") "  Drive BIOS number\n");
	printf("  " SWITCH_GETOPT_LONG("-v, --verbose      ", "-v") "  Verbose\n");
	printf("  " SWITCH_GETOPT_LONG("-h, --help         ", "-h") "  Help\n");
	printf("  " SWITCH_GETOPT_LONG("-V, --version      ", "-V") "  Version\n");
}

#if HAVE_GETOPT_LONG
struct option long_options[] = {
	{"label", 1, 0, 'L'},
	{"oem", 1, 0, 'O'},
	{"serial", 1, 0, 'S'},
	{"drive", 1, 0, 'E'},

	{"output", 1, 0, 'o'},

	{"boot", 1, 0, 'b'},
	{"boot-fat12", 1, 0, '1'},
	{"boot-fat16", 1, 0, '2'},
	{"boot-fat32", 1, 0, '3'},
	{"mbr", 1, 0, 'm'},
	{"copy", 1, 0, 'c'},
	{"exclude", 1, 0, 'x'},
	{"time", 1, 0, 't'},

	{"syslinux2", 0, 0, 'X'},
	{"syslinux3", 0, 0, 'Y'},
	{"zip", 0, 0, 'Z'},
	{"partition", 0, 0, 'P'},
	{"disk", 0, 0, 'D'},
	{"mbrfat", 0, 0, 'F'},

	{"verbose", 0, 0, 'v'},
	{"interative", 0, 0, 'i'},

	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'V'},
	{0, 0, 0, 0}
};
#endif

#define OPTIONS "L:O:S:E:o:ab:1:2:3:m:c:x:t:XYZPDFvihV"

void string_insert(struct string_list** list, const char* s)
{
	struct string_list* e = malloc(sizeof(struct string_list));
	/* insert at the end to keep the user specified order */
	e->path = strdup(s);
	e->next = 0;
	if (!*list) {
		*list = e;
	} else {
		struct string_list* i = *list;
		while (i->next)
			i = i->next;
		i->next = e;
	}
}

void string_destroy(struct string_list* list)
{
	struct string_list* e = list;
	while (e) {
		struct string_list* n = e->next;
		free(e->path);
		free(e);
		e = n;
	}
}

int sector_read(unsigned char* sector, const char* file)
{
	FILE* f;
	size_t s;

	f = fopen(file, "rb");
	if (!f) {
		fprintf(stderr, "Error reading the file %s", file);
		return -1;
	}

	/* read also a partial sector */
	memset(sector, 0, SECTOR_SIZE);

	s = fread(sector, 1, SECTOR_SIZE, f);
	if (s == 0) {
		fprintf(stderr, "Error reading the file %s", file);
		return -1;
	}

	fclose(f);

	return 0;
}

struct verbose_context_struct {
	unsigned total;
	unsigned counter;
};

void verbose_callback(void* void_context, int operation, unsigned pos, unsigned size)
{
	struct verbose_context_struct* context = (struct verbose_context_struct*)void_context;

	if (operation) {
		context->counter += size;
		while (context->counter >= 2048) {
			++context->total;
			context->counter -= 2048;
			printf("\rwrite %d [sectors], %d [MB]", context->total * 2048, context->total);
			fflush(stdout);
		}
	}
}

int main(int argc, char* argv[])
{
	struct string_list* exclude_list;
	struct string_list* copy_list;
	struct string_list* s;
	int c;
	struct disk_handle* h;
	int r;
	struct fat_context* fat;
	const char* label;
	const char* oem;
	unsigned serial;
	const char* file_boot12;
	const char* file_boot16;
	const char* file_boot32;
	const char* file_mbr;
	unsigned char boot12[SECTOR_SIZE];
	unsigned char boot16[SECTOR_SIZE];
	unsigned char boot32[SECTOR_SIZE];
	unsigned char mbr[SECTOR_SIZE];
	const char* output;
	int syslinux2;
	int syslinux3;
	int mbrfat;
	int verbose;
	int only_partition;
	int only_disk;
	int zip_compatibility;
	int drive;
	const unsigned syslinux3_sector_max = 65;
	unsigned syslinux3_sector_map[syslinux3_sector_max];
	int syslinux3_sector_mac;
	unsigned syslinux3_size;
	unsigned part_entry;
	unsigned fat_start;
	struct verbose_context_struct verbose_context;
	time_t force_time;

	exclude_list = 0;
	copy_list = 0;
	label = 0;
	oem = "MAKEBOOTFAT";
	serial = time(0);
	file_boot12 = 0;
	file_boot16 = 0;
	file_boot32 = 0;
	file_mbr = 0;
	output = 0;
	syslinux2 = 0;
	syslinux3 = 0;
	mbrfat = 0;
	verbose = 0;
	only_partition = 0;
	only_disk = 0;
	drive = -1;
	syslinux3_sector_mac = 0;
	syslinux3_size = 0;
	zip_compatibility = 0;
	part_entry = 0;
	verbose_context.total = 0;
	verbose_context.counter = 0;
	force_time = 0;

	opterr = 0;

	while ((c =
#if HAVE_GETOPT_LONG
		getopt_long(argc, argv, OPTIONS, long_options, 0))
#else
		getopt(argc, argv, OPTIONS))
#endif
	!= EOF) {
		switch (c) {
		case 'O' :
			oem = optarg;
			break;
		case 'L' :
			label = optarg;
			break;
		case 'S' :
			serial = strtoul(optarg, 0, 0);
			break;
		case 'E' :
			drive = strtoul(optarg, 0, 0);
			break;
		case 'b' :
			file_boot12 = optarg;
			file_boot16 = optarg;
			file_boot32 = optarg;
			break;
		case '1' :
			file_boot12 = optarg;
			break;
		case '2' :
			file_boot16 = optarg;
			break;
		case '3' :
			file_boot32 = optarg;
			break;
		case 'm' :
			file_mbr = optarg;
			break;
		case 'c' :
			string_insert(&copy_list, optarg);
			break;
		case 'x' :
			string_insert(&exclude_list, optarg);
			break;
		case 't' :
			force_time = atoi(optarg);
			break;
		case 'X' :
			syslinux2 = 1;
			break;
		case 'Y' :
			syslinux3 = 1;
			break;
		case 'Z' :
			zip_compatibility = 1;
			break;
		case 'P' :
			only_partition = 1;
			break;
		case 'D' :
			only_disk = 1;
			break;
		case 'F' :
			mbrfat = 1;
			break;
		case 'o' :
			output = optarg;
			break;
		case 'v' :
			verbose = 1;
			break;
		case 'i' :
			error_interactive(1);
			break;
		case 'h' :
			usage();
			exit(EXIT_SUCCESS);
		case 'V' :
			version();
			exit(EXIT_SUCCESS);
		default:
			fprintf(stderr, "Unknown option `%c`\n", (char)c);
			break;
		} 
	}

	if (optind + 1 != argc) {
		usage();
		exit(EXIT_FAILURE);
	}

	if (!output) {
		fprintf(stderr, "You must specify the output device.\n");
		exit(EXIT_FAILURE);
	}

	if (!file_boot12 && !file_boot16 && !file_boot32) {
		fprintf(stderr, "You must specify a boot sector image.\n");
		exit(EXIT_FAILURE);
	}

	if (file_boot12) {
		if (sector_read(boot12, file_boot12) != 0) {
			exit(EXIT_FAILURE);
		}
	}

	if (file_boot16) {
		if (sector_read(boot16, file_boot16) != 0) {
			exit(EXIT_FAILURE);
		}
	}

	if (file_boot32) {
		if (sector_read(boot32, file_boot32) != 0) {
			exit(EXIT_FAILURE);
		}
	}

	if (strcmp(output, "usb") == 0) {
		h = disk_find();
		if (h == 0) 
			goto err_msg;
	} else {
		h = disk_open(output);
		if (h == 0) 
			goto err_msg;
	}

	if (zip_compatibility) {
		if (h->geometry.size <= 1024 * 64 * 32) {
			/* A ZIP-Drive requires 32 sectors, 64 heads and the use of the last partition entry */
			h->geometry.sectors = 32; /* value required by ZIP-Drive */
			h->geometry.heads = 64; /* value required by ZIP-Drive */
			part_entry = 3; /* ZIP-Drive requires to use the last partition entry */
			h->geometry.cylinders = h->geometry.size / (h->geometry.sectors * h->geometry.heads);
			/* recompute the size, it may be a little smaller */
			h->geometry.size = h->geometry.sectors * h->geometry.heads * h->geometry.cylinders;
		}
	}

	/* Starts the filesystem at the second track. Required for maximum compatibility. */
	/* The alternative is to start it immediatially after the mbr (value 1) */
	fat_start = h->geometry.sectors;

	if (drive != -1) {
		h->geometry.drive = drive;
	}

	if (only_partition) {
		if (h->geometry.start == 0) {
			error_set("The specified device isn't a partition but a disk.");
			goto err_invalidate;
		}
	}

	if (only_disk) {
		if (h->geometry.start != 0) {
			error_set("The specified device isn't a disk but a partition.");
			goto err_invalidate;
		}
	}

	if (file_mbr) {
		unsigned size;

		if (sector_read(mbr, file_mbr) != 0)
			goto err_invalidate;

		size = h->geometry.size - fat_start;
		if (syslinux2 && size > 2097088)
			size = 2097088; /* maximum size of a fat 16 with 32 sectors per cluster */

		fat = fat_open(h, fat_start, size, &h->geometry);
	} else {
		unsigned size;

		size = h->geometry.size;
		if (syslinux2 && size > 2097088)
			size = 2097088; /* maximum size of a fat 16 with 32 sectors per cluster */

		fat = fat_open(h, 0, size, &h->geometry);
	}
	if (fat == 0) 
		goto err_invalidate;

	if (verbose) {
		printf("device_start %d [sectors]\n", h->geometry.start);
		printf("device_size %d [sectors], %d [MB]\n", h->geometry.size, h->geometry.size / 2048);
		printf("device_geometry %d/%d/%d [cylinders/heads/sectors]", h->geometry.cylinders, h->geometry.heads, h->geometry.sectors);
		if (h->geometry.sectors == 32 && h->geometry.heads == 64) {
			printf(" (ZIP-Drive compatible)");
		}
		printf("\n");
		printf("bios_drive %s\n", (h->geometry.drive & 0x80) != 0 ? "HDD" : "FDD");
	}

	r = fatboot_format(fat, file_boot12 ? boot12 : 0, file_boot16 ? boot16 : 0, file_boot32 ? boot32 : 0, oem, label, serial, &h->geometry, syslinux2);
	if (r != 0) 
		goto err_invalidate;

	if (verbose) {
		printf("fat_start %d [sectors]\n", fat->h_pos + h->geometry.start);
		printf("fat_size %d [sectors]\n", fat->h_size);
		printf("fat_bit %d\n", fat->info.fat_bit);
		printf("fat_sectorpercluster %d\n", fat->info.cluster_size);
	}

	if (file_mbr) {
		part_setup(mbr, part_entry, fat->info.fat_bit, fat->h_pos, fat->h_size, &h->geometry);

		if (mbrfat) {
			unsigned char boot[SECTOR_SIZE];

			/* read the fat boot sector */
			if (disk_read(fat->h, fat->h_pos, boot, 1) != 0)
				return -1;

			/* change the mbr to imitate the fat boot sector */
			r = part_fat_setup(mbr, boot, fat->info.fat_bit, fat->h_pos);
			if (r != 0)
				goto err_invalidate;
		}

		/* write the new mbr */
		if (disk_write(h, 0, mbr, 1) != 0)
			goto err_invalidate;
	}

	if (verbose) {
		h->ope_callback = verbose_callback;
		h->ope_context = &verbose_context;
	}

	s = copy_list;
	while (s) {
		unsigned root_cluster;
		unsigned root_size;
		unsigned l;

		r = fatboot_copytoroot(fat, s->path, &root_cluster, &root_size, force_time);
		if (r != 0)
			goto err_invalidate;

		/* detect the syslinux loader */
		if (syslinux3) {
			l = strlen(s->path);
			if (l>=11 && strcmp("ldlinux.sys", s->path + l - 11) == 0) {
				if (syslinux3_sector_mac != 0) {
					error_set("Multiple ldlinux.sys.");
					goto err_invalidate;
				}
				syslinux3_size = root_size;
				syslinux3_sector_mac = fat_sector_chain(fat, root_cluster, syslinux3_sector_map, syslinux3_sector_max);
			}
		}

		s = s->next;
	}

	r = fatboot_copy(fat, 0, argv[optind], exclude_list, force_time);
	if (r != 0) 
		goto err_invalidate;

	if (syslinux3) {
		if (syslinux3_sector_mac == 0) {
			error_set("Missing ldlinux.sys.");
			goto err_invalidate;
		}
		if (syslinux3_sector_mac < 0) {
			error_set("Invalid ldlinux.sys. Too big.");
			goto err_invalidate;
		}
		if (fatboot_syslinux3(fat, syslinux3_sector_map, syslinux3_sector_mac, syslinux3_size) != 0) {
			goto err_invalidate;
		}
	}

	r = fat_close(fat);
	if (r != 0) 
		goto err_invalidate;

	if (verbose) {
		h->ope_callback = 0;
		h->ope_context = 0;
		printf("\n");
	}

	r = disk_close(h);
	if (r != 0) 
		goto err_msg;

	string_destroy(exclude_list);
	string_destroy(copy_list);

	return EXIT_SUCCESS;

err_invalidate:
	memset(mbr, 0, SECTOR_SIZE);
	disk_write(h, 0, mbr, 1);
	disk_close(h);
err_msg:
	fprintf(stderr, "%s\n", error_get());
	exit(EXIT_FAILURE);
}

