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

#include "disk.h"
#include "error.h"

#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if HAVE_SCSI_SCSI_H
#include <scsi/scsi.h>
#endif

#if HAVE_MNTENT_H
#include <mntent.h>
#endif

#if HAVE_LINUX_HDREG_H
#include <linux/hdreg.h>
#endif

#if HAVE_LINUX_FD_H
#include <linux/fd.h>
#endif

/* Configuration */

#if defined(SCSI_IOCTL_PROBE_HOST) && defined(SCSI_IOCTL_GET_IDLUN) && defined(SCSI_IOCTL_GET_BUS_NUMBER)
#define USE_FIND
#endif

#if HAVE_SETMNTENT && HAVE_GETMNTENT && HAVE_ENDMNTENT
#define USE_CHECKMOUNT
#endif

/* Endian safe read/write */

unsigned le_uint16_read(const void* ptr)
{
	const unsigned char* ptr8 = (const unsigned char*)ptr;
	return (unsigned)ptr8[0] | (unsigned)ptr8[1] << 8;
}

unsigned le_uint32_read(const void* ptr)
{
	const unsigned char* ptr8 = (const unsigned char*)ptr;
	return (unsigned)ptr8[0] | (unsigned)ptr8[1] << 8 | (unsigned)ptr8[2] << 16 | (unsigned)ptr8[3] << 24;
}

void le_uint16_write(void* ptr, unsigned v)
{
	unsigned char* ptr8 = (unsigned char*)ptr;
	ptr8[0] = v & 0xFF;
	ptr8[1] = (v >> 8) & 0xFF;
}

void le_uint32_write(void* ptr, unsigned v)
{
	unsigned char* ptr8 = (unsigned char*)ptr;
	ptr8[0] = v & 0xFF;
	ptr8[1] = (v >> 8) & 0xFF;
	ptr8[2] = (v >> 16) & 0xFF;
	ptr8[3] = (v >> 24) & 0xFF;
}

#ifdef __WIN32__

/* Windows */

#include <windows.h>
#include <winioctl.h>
#include <tchar.h>
#include <setupapi.h>

/* Declaration of IOCTL_DISK_GET_PARTITION_INFO_EX */
#ifndef IOCTL_DISK_GET_PARTITION_INFO_EX

#define IOCTL_DISK_GET_PARTITION_INFO_EX CTL_CODE(IOCTL_DISK_BASE, 0x0012, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _PARTITION_INFORMATION_GPT {
	GUID PartitionType;
	GUID PartitionId;
	DWORD64 Attributes;
	WCHAR Name[36];
} PARTITION_INFORMATION_GPT;

typedef enum _PARTITION_STYLE {
	PARTITION_STYLE_MBR,
	PARTITION_STYLE_GPT,
	PARTITION_STYLE_RAW
} PARTITION_STYLE;

typedef struct _PARTITION_INFORMATION_MBR {
	BYTE PartitionType;
	BOOLEAN BootIndicator;
	BOOLEAN RecognizedPartition;
	DWORD HiddenSectors;
} PARTITION_INFORMATION_MBR;

typedef struct _PARTITION_INFORMATION_EX {
	PARTITION_STYLE PartitionStyle;
	LARGE_INTEGER StartingOffset;
	LARGE_INTEGER PartitionLength;
	DWORD PartitionNumber;
	BOOLEAN RewritePartition;
	union {
		PARTITION_INFORMATION_MBR Mbr;
		PARTITION_INFORMATION_GPT Gpt;
	};
} PARTITION_INFORMATION_EX;
#endif

/* Declaration of IOCTL_DISK_GET_DRIVE_GEOMETRY_EX */
#ifndef IOCTL_DISK_GET_DRIVE_GEOMETRY_EX

#define IOCTL_DISK_GET_DRIVE_GEOMETRY_EX CTL_CODE(IOCTL_DISK_BASE, 0x0028, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _DISK_GEOMETRY_EX {
	DISK_GEOMETRY Geometry;
	LARGE_INTEGER DiskSize;
	BYTE Data[1];
} DISK_GEOMETRY_EX;

#endif

/* Declaration of setupapi */
#ifndef SPDRP_ENUMERATOR_NAME
#define SPDRP_ENUMERATOR_NAME 0x00000016
#endif

static GUID GUID_INTERFACE_DISK = { 0x53f56307L, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } };

/**
 * Get a device property.
 */
static BOOL SetupDiGetProperty(HDEVINFO dev, SP_DEVINFO_DATA* data, DWORD prop, unsigned char** buf, DWORD* buf_size)
{
	DWORD PropertyRegDataType;
	DWORD RequiredSize;

	if (!SetupDiGetDeviceRegistryProperty(dev, data, prop, &PropertyRegDataType, 0, 0, &RequiredSize)) {
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE; 
	}

	*buf_size = RequiredSize;
	*buf = (unsigned char*)malloc(RequiredSize);
	if (!buf)
		return FALSE;

	if (!SetupDiGetDeviceRegistryProperty(dev, data, prop, &PropertyRegDataType, *buf, RequiredSize, 0))
		return FALSE;

	return TRUE;
}

/**
 * Change the state of a device.
 */
static BOOL SetupDiChangeDeviceState(HDEVINFO dev, SP_DEVINFO_DATA* data, DWORD state)
{
	SP_PROPCHANGE_PARAMS params;

	memset(&params, 0, sizeof(SP_PROPCHANGE_PARAMS));

	params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	params.StateChange = state;
	params.Scope = DICS_FLAG_CONFIGSPECIFIC;
	params.HwProfile = 0;

	if (!SetupDiSetClassInstallParams(dev, data, (PSP_CLASSINSTALL_HEADER)&params, sizeof(SP_PROPCHANGE_PARAMS))) {
		return FALSE;
	}

	if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, dev, data)) {
		return FALSE;
	}

	return TRUE;
}

/**
 * Restart a device.
 */
static BOOL SetupDiRestartDevice(HDEVINFO dev, SP_DEVINFO_DATA* data)
{
	if (!SetupDiChangeDeviceState(dev, data, DICS_PROPCHANGE))
		return FALSE;

	return TRUE;
}

/**
 * Device enumeration callback.
 */
typedef BOOL SetupDiForEachCallBack(void* void_context, HDEVINFO h, SP_DEVINFO_DATA* data, SP_DEVICE_INTERFACE_DATA* idata, SP_DEVICE_INTERFACE_DETAIL_DATA* ddata, const char* enumerator);

/**
 * Device enumerator for all the disk devices.
 */
static BOOL SetupDiForEach(SetupDiForEachCallBack* callback, void* context)
{
	unsigned index;
	HDEVINFO h;
	GUID guid = GUID_INTERFACE_DISK;

	h = SetupDiGetClassDevs(&guid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (h == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	index = 0;
	while (1) {
		unsigned char* prop;
		DWORD size;
		SP_DEVINFO_DATA data;
		SP_DEVICE_INTERFACE_DATA idata;
		SP_DEVICE_INTERFACE_DETAIL_DATA* ddata;

		idata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		if (!SetupDiEnumDeviceInterfaces(h, 0, &guid, index, &idata)) {
			if (GetLastError() != ERROR_NO_MORE_ITEMS)
				return FALSE;
			break;
		}

		if (!SetupDiGetDeviceInterfaceDetail(h, &idata, 0, 0, &size, 0)) {
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				return FALSE;
		}

		ddata = malloc(size);
		if (!ddata)
			return FALSE;

		ddata->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		data.cbSize = sizeof(SP_DEVINFO_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(h, &idata, ddata, size, 0, &data)) {
			return FALSE;
		}

		if (SetupDiGetProperty(h, &data, SPDRP_ENUMERATOR_NAME, &prop, &size)) {
			if (!callback(context, h, &data, &idata, ddata, prop))
				return FALSE;
			free(prop);
		}

		free(ddata);

		++index;
	}

	SetupDiDestroyDeviceInfoList(h);

	return TRUE;
}

/**
 * Callback for restarting the specified USB device.
 */
static BOOL CallBackUSBReconnect(void* void_context, HDEVINFO h, SP_DEVINFO_DATA* data, SP_DEVICE_INTERFACE_DATA* idata, SP_DEVICE_INTERFACE_DETAIL_DATA* ddata, const char* enumerator)
{
	if (stricmp(enumerator, "USBSTOR") == 0) {
		if (strcmp(void_context, ddata->DevicePath) == 0) {
			if (!SetupDiRestartDevice(h, data))
				return FALSE;
		}
	}

	return TRUE;
}

struct find_context {
	char device[512];
	int count;
};

/**
 * Callback for finding a USB device.
 */
static BOOL CallBackUSBFind(void* void_context, HDEVINFO h, SP_DEVINFO_DATA* data, SP_DEVICE_INTERFACE_DATA* idata, SP_DEVICE_INTERFACE_DETAIL_DATA* ddata, const char* enumerator)
{
	struct find_context* context = (struct find_context*)void_context;

	if (stricmp(enumerator, "USBSTOR") == 0) {
		++context->count;

		memset(context->device, 0, sizeof(context->device));
		strncpy(context->device, ddata->DevicePath, sizeof(context->device) - 1);
	}

	return TRUE;
}

static struct disk_handle* os_open(const char* dev)
{
	DISK_GEOMETRY dg;
	DISK_GEOMETRY_EX dge;
	PARTITION_INFORMATION pi;
	PARTITION_INFORMATION_EX pie;
	DWORD size;
	struct disk_handle* h;

	if ((GetVersion() & 0x80000000) != 0) {
		error_set("This program run only in Windows 2000/XP.");
		return 0;
	}

	h = malloc(sizeof(struct disk_handle));
	if (!h) {
		error_set("Low memory.");
		goto err;
	}

	h->ope_callback = 0;
	h->ope_context = 0;

	memset(h->device, 0, sizeof(h->device));
	strncpy(h->device, dev, sizeof(h->device) - 1);

	/* restart the usb device to flush the cache */
	/* this is required because the LOCK doesn't work on disks but only on volumes */
	if (!SetupDiForEach(CallBackUSBReconnect, h->device)) {
		error_set("Error %d restarting the device %s.", GetLastError(), h->device);
		goto err_free;
	}

	h->handle = CreateFile(h->device, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, 0);
	if (h->handle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_ACCESS_DENIED) {
			error_set("You must be Administrator to access the device %s.", h->device);
			goto err_free;
		} else {
			error_set("Error %d accessing the device %s.", GetLastError(), h->device);
			goto err_free;
		}
	}

	if (!DeviceIoControl(h->handle, FSCTL_LOCK_VOLUME , 0, 0, 0, 0, &size, 0)) {
		if (GetLastError() == ERROR_INVALID_FUNCTION) {
			/* the LOCK function is supported only on volumes and not on disks */
			/* ignore the error */
		} else if (GetLastError() == ERROR_ACCESS_DENIED) {
			error_set("The device %s is busy. Close all the applications and retry.", h->device);
			goto err_close;
		} else {
			error_set("Error %d locking the volume %s.", GetLastError(), h->device);
			goto err_close;
		}
	}

	/* get the geometry */
	if (!DeviceIoControl(h->handle, IOCTL_DISK_GET_DRIVE_GEOMETRY, 0, 0, &dg, sizeof(dg), &size, 0)) {
		if (GetLastError() == ERROR_INVALID_FUNCTION) {
			/* IOCTL_DISK_GET_DRIVE_GEOMETRY is supported only on disks and not on volumes */
			if (!DeviceIoControl(h, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, 0, 0, &dge, sizeof(dge), &size, 0)) {
				if (GetLastError() == ERROR_INVALID_FUNCTION) {
					/* IOCTL_DISK_GET_DRIVE_GEOMETRY_EX is supported only in Windows XP */
					error_set("You need Windows XP to format this partition.");
					goto err_close;
				} else {
					error_set("Error %d getting the device geometry.", GetLastError());
					goto err_close;
				}
			}
			dg = dge.Geometry;
		} else {
			error_set("Error %d getting the device geometry.", GetLastError());
			goto err_close;
		}
	}

	/* get the size */
	if (!DeviceIoControl(h->handle, IOCTL_DISK_GET_PARTITION_INFO, 0, 0, &pi, sizeof(pi), &size, 0)) {
		if (GetLastError() == ERROR_INVALID_FUNCTION) {
			if (!DeviceIoControl(h, IOCTL_DISK_GET_PARTITION_INFO_EX, 0, 0, &pie, sizeof(pie), &size, 0)) {
				if (GetLastError() == ERROR_INVALID_FUNCTION) {
					error_set("You need Windows XP to format this partition.");
					goto err_close;
				} else {
					error_set("Error %d getting the device size.", GetLastError());
					goto err_close;
				}
			}
			pi.StartingOffset = pie.StartingOffset;
			pi.PartitionLength = pie.PartitionLength;
		} else {
			error_set("Error %d getting the device size.", GetLastError());
			goto err_close;
		}
	}

	if (pi.PartitionLength.QuadPart == 0 || (pi.PartitionLength.QuadPart % SECTOR_SIZE) != 0) {
		error_set("Invalid device size.");
		goto err_close;
	}

	h->geometry.size = pi.PartitionLength.QuadPart / SECTOR_SIZE;
	h->geometry.start = pi.StartingOffset.QuadPart / SECTOR_SIZE;
	h->geometry.sectors = dg.SectorsPerTrack;
	h->geometry.heads = dg.TracksPerCylinder;
	h->geometry.cylinders = dg.Cylinders.QuadPart;
	if (dg.MediaType == FixedMedia || dg.MediaType == RemovableMedia) {
		h->geometry.drive = 0x80;
	} else {
		h->geometry.drive = 0x0;
	}

	return h;

err_close:
	CloseHandle(h->handle);
err_free:
	free(h);
err:
	return 0;
}

static struct disk_handle* os_find(void)
{
	struct find_context context;

	context.count = 0;

	if ((GetVersion() & 0x80000000) != 0) {
		error_set("This program run only in Windows 2000/XP.");
		return 0;
	}

	if (!SetupDiForEach(CallBackUSBFind, &context)) {
		error_set("Error searching for usb disks.");
		return 0;
	}

	if (context.count > 1) {
		error_set("Please insert ONLY ONE usb disk.");
		return 0;
	}

	if (context.count == 0) {
		error_set("Please insert one usb disk.");
		return 0;
	}

	return os_open(context.device);
}

static int os_close(struct disk_handle* h)
{
	BOOL r;
	DWORD size;

	/* signal the change of filesystem */
	if (!DeviceIoControl(h->handle, FSCTL_DISMOUNT_VOLUME , 0, 0, 0, 0, &size, 0)) {
		/* the DISMOUNT function is supported only on volumes and not on disks */
		/* ignore error */
	}

	/* unlock the volume */
	if (!DeviceIoControl(h->handle, FSCTL_UNLOCK_VOLUME , 0, 0, 0, 0, &size, 0)) {
		/* the UNLOCK function is supported only on volumes and not on disks */
		/* ignore error */
	}

	r = CloseHandle(h->handle);
	if (!r) {
		error_set("Error %d closing the device %s.", GetLastError(), h->device);
		return -1;
	}

	/* restart the usb device to commit all the changes and to reload the filesystem */
	/* this is required because the LOCK doesn't work on disks but only on volumes */
	if (!SetupDiForEach(CallBackUSBReconnect, h->device)) {
		error_set("Error %d restarting the device %s.", GetLastError(), h->device);
		return -1;
	}

	/* do it two times, in some Windows XP it seems required to ensure a correct device restart */
	if (!SetupDiForEach(CallBackUSBReconnect, h->device)) {
		error_set("Error %d restarting the device %s.", GetLastError(), h->device);
		return -1;
	}

	free(h);

	return 0;
}

static int os_read(struct disk_handle* h, unsigned pos, void* data, unsigned size)
{
	LARGE_INTEGER off;
	DWORD info;
	ULONG lp;
	LONG hp;

	off.QuadPart = SECTOR_SIZE * (LONGLONG)pos;
	lp = off.LowPart;
	hp = off.HighPart;
	lp = SetFilePointer(h->handle, lp, &hp, FILE_BEGIN);
	if (off.LowPart != lp || off.HighPart != hp) {
		error_set("Error %d reading the device.", GetLastError());
		return -1;
	}

	if (!ReadFile(h->handle, data, size * SECTOR_SIZE, &info, 0)) {
		error_set("Error %d reading the device.", GetLastError());
		return -1;
	}
	if (info != size * SECTOR_SIZE) {
		error_set("Error %d reading the device.", GetLastError());
		return -1;
	}

	if (h->ope_callback)
		h->ope_callback(h->ope_context, 0, pos, size);

	return 0;
}

static int os_write(struct disk_handle* h, unsigned pos, const void* data, unsigned size)
{
	LARGE_INTEGER off;
	DWORD info;
	ULONG lp;
	LONG hp;

	off.QuadPart = SECTOR_SIZE * (LONGLONG)pos;
	lp = off.LowPart;
	hp = off.HighPart;
	lp = SetFilePointer(h->handle, lp, &hp, FILE_BEGIN);
	if (off.LowPart != lp || off.HighPart != hp) {
		error_set("Error %d writing the device.", GetLastError());
		return -1;
	}

	if (!WriteFile(h->handle, data, size * SECTOR_SIZE, &info, 0)) {
		error_set("Error %d writing the device.", GetLastError());
		return -1;
	}
	if (info != size * SECTOR_SIZE) {
		error_set("Error %d writing the device.", GetLastError());
		return -1;
	}

	if (h->ope_callback)
		h->ope_callback(h->ope_context, 1, pos, size);

	return 0;
}

#else

/* Linux */

#ifdef USE_FIND
/**
 * Check if the SCSI device is a USB Mass Storage and if it's present.
 */
static int os_usbmassstorage(const char* dev, int* host, int* channel, int* id, int* lun)
{
	int r, h;
	char hostname[64];
	int idlun[2];
	int bus_number;
	char buf[SECTOR_SIZE];

	h = open(dev, O_RDONLY | O_NONBLOCK);
	if (h < 0) {
		goto err;
	}

	*(int*)hostname = 63;
	r = ioctl(h, SCSI_IOCTL_PROBE_HOST, hostname);
	if (r < 0) {
		goto err_close;
	}

	/* exclude not USB mass storage device */
	if (strstr(hostname, "USB Mass Storage") == 0) {
		goto err_close;
	}

	r = ioctl(h, SCSI_IOCTL_GET_IDLUN, &idlun);
	if (r < 0) {
		goto err_close;
	}

	*channel = (idlun[0] >> 16) & 0xff;
	*lun = (idlun[0] >> 8 ) & 0xff;
	*id = idlun[0] & 0xff;

	/* exclude secondary devices */
	if (*lun != 0) {
		goto err_close;
	}

	r = ioctl(h, SCSI_IOCTL_GET_BUS_NUMBER, &bus_number);
	if (r < 0) {
		goto err_close;
	}

	*host = bus_number;

	/* try a read, this exclude removed device on the 2.4 Linux kernel */
	r = read(h, buf, SECTOR_SIZE);
	if (r != SECTOR_SIZE) {
		goto err_close;
	}

	return 0;

err_close:
	close(h);
err:
	return -1;
}
#endif

#ifdef USE_CHECKMOUNT
/**
 * Check that the device is not mounted.
 */
static int os_checkmount(const char* dev)
{
	FILE* f;
	struct mntent* m;

	f = setmntent("/etc/mtab", "r");
	if (!f) {
		error_set("Error accessing /etc/mtab. %s.", strerror(errno));
		return -1;
	}

	while ((m = getmntent(f)) != 0) {
		if (strncmp(m->mnt_fsname, dev, strlen(dev)) == 0) {
			error_set("You cannot operate on device %s because is mounted as %s.", m->mnt_fsname, m->mnt_dir);
			return -1;
		}
	}

	endmntent(f);

	return 0;
}
#endif

static struct disk_handle* os_open(const char* dev)
{
	off_t o;
	struct hd_geometry hg;
	struct floppy_struct fg;
	struct disk_handle* h;

	h = malloc(sizeof(struct disk_handle));
	if (!h) {
		error_set("Low memory.");
		goto err;
	}

	h->ope_callback = 0;
	h->ope_context = 0;

	memset(h->device, 0, sizeof(h->device));
	strncpy(h->device, dev, sizeof(h->device) - 1);

#ifdef USE_CHECKMOUNT
	if (os_checkmount(h->device) != 0) {
		goto err_free;
	}
#endif

	h->handle = open(h->device, O_RDWR);
	if (h->handle == -1) {
		error_set("Error opening the device %s. %s.", h->device, strerror(errno));
		goto err_free;
	}

	o = lseek(h->handle, 0, SEEK_END);
	if (o == -1) {
		error_set("Error seeking the device %s. %s.", h->device, strerror(errno));
		goto err_close;
	}

	if (o == 0 || (o % SECTOR_SIZE) != 0) {
		error_set("Invalid device size.");
		goto err_close;
	}

	h->geometry.size = o / SECTOR_SIZE;

	memset(&hg, 0, sizeof(hg));
	memset(&fg, 0, sizeof(fg));

	if (ioctl(h->handle, HDIO_GETGEO, &hg) >= 0) {
		h->geometry.start = hg.start;
		h->geometry.sectors = hg.sectors;
		h->geometry.heads = hg.heads;
		h->geometry.cylinders = hg.cylinders;
		h->geometry.drive = 0x80;
	} else if (ioctl(h->handle, FDGETPRM, &fg) >= 0) {
		h->geometry.start = 0;
		h->geometry.sectors = fg.sect;
		h->geometry.heads = fg.head;
		h->geometry.cylinders = fg.track;
		h->geometry.drive = 0x0;
	} else {
		h->geometry.start = 0;
		/* fallback to standard values */
		if (h->geometry.size <= 32*64*1024) {
			/* ZIP-Disk geometry */
			h->geometry.sectors = 32;
			h->geometry.heads = 64;
		} else if (h->geometry.size <= 32*256*1024) {
			/* ZIP-Disk like geometry */
			h->geometry.sectors = 32;
			h->geometry.heads = 256;
		} else {
			/* maximum size geometry */
			h->geometry.sectors = 63;
			h->geometry.heads = 256;
		}
		/* compute the cylinders number */
		h->geometry.cylinders = h->geometry.size / (h->geometry.sectors * h->geometry.heads);
		h->geometry.drive = 0x0;
		/* recompute the size */
		h->geometry.size = h->geometry.sectors * h->geometry.heads * h->geometry.cylinders;
	}

	return h;

err_close:
	close(h->handle);
err_free:
	free(h);
err:
	return 0;
}

static struct disk_handle* os_find(void)
{
#ifdef USE_FIND
	int i;
	char device[64];
	int count;
	int eaccess;

	eaccess = 0;
	count = 0;
	for(i=0;i<16;++i) {
		int r;
		char buf[64];
		int host;
		int channel;
		int id;
		int lun;

		sprintf(buf, "/dev/sd%c", (char)i + 'a');

		r = os_usbmassstorage(buf, &host, &channel, &id, &lun);
		if (r != 0) {
			if (errno == EACCES)
				eaccess = 1;
			continue;
		}

		++count;
		strcpy(device, buf);
	}

	if (count > 1) {
		error_set("Please insert ONLY ONE usb disk.");
		return 0;
	}

	if (count == 0) {
		if (eaccess) {
			error_set("Please insert one usb disk and ensure to be root.");
		} else {
			error_set("Please insert one usb disk.");
		}
		return 0;
	}

	return os_open(device);
#else
	error_set("USB disk detection not supported in this system.");
	return 0;
#endif
}

static int os_close(struct disk_handle* h)
{
	int r;

	r = close(h->handle);
	if (r == -1) {
		error_set("Error closing the device. %s.", strerror(errno));
		return -1;
	}

	/* commit all the changes */
//	sync();

	free(h);

	return 0;
}

static int os_read(struct disk_handle* h, unsigned pos, void* data, unsigned size)
{
	off_t o;
	ssize_t s;

	o = SECTOR_SIZE * (off_t)pos;

	s = pread(h->handle, data, size * SECTOR_SIZE, o);

	if (s != size * SECTOR_SIZE) {
		error_set("Error reading the device. %s.", strerror(errno));
		return -1;
	}

	if (h->ope_callback)
		h->ope_callback(h->ope_context, 0, pos, size);

	return 0;
}

static int os_write(struct disk_handle* h, unsigned pos, const void* data, unsigned size)
{
	off_t o;
	ssize_t s;

	o = SECTOR_SIZE * (off_t)pos;

	s = pwrite(h->handle, data, size * SECTOR_SIZE, o);

	if (s != size * SECTOR_SIZE) {
		error_set("Error writing the device. %s.", strerror(errno));
		return -1;
	}

	if (h->ope_callback)
		h->ope_callback(h->ope_context, 1, pos, size);

	return 0;
}

#endif

/* Cache */

static struct disk_cache* cache_alloc(unsigned pos, const void* data, unsigned size)
{
	struct disk_cache* c;

	c = malloc(sizeof(struct disk_cache));
	if (!c) {
		error_set("Low memory.");
		return 0;
	}

	c->pos = pos;
	c->size = size;
	c->data = malloc(size * SECTOR_SIZE);
	if (!c->data) {
		free(c);
		error_set("Low memory.");
		return 0;
	}

	memcpy(c->data, data, size * SECTOR_SIZE);

	return c;
}

static void cache_free(struct disk_cache* c)
{
	free(c->data);
	free(c);
}

static int cache_flush(struct disk_handle* h)
{
	unsigned char* buf;
	struct disk_cache* c;
	int r;

	if (!h->cache_list)
		return 0;

	buf = malloc((h->cache_end - h->cache_begin) * SECTOR_SIZE);
	if (!buf) {
		error_set("Low memory.");
		return -1;
	}

	c = h->cache_list;
	while (c) {
		struct disk_cache* n;

		memcpy(buf + (c->pos - h->cache_begin) * SECTOR_SIZE, c->data, c->size * SECTOR_SIZE);

		n = c->next;
		cache_free(c);
		c = n;
	}

	h->cache_list = 0;

	r = os_write(h, h->cache_begin, buf, h->cache_end - h->cache_begin);
	if (r != 0) {
		free(buf);
		return -1;
	}

	free(buf);

	return 0;
}

static int cache_is_overlap(struct disk_handle* h, unsigned pos, unsigned size)
{
	if (!h->cache_list
		|| pos + size <= h->cache_begin
		|| pos >= h->cache_end)
		return 0;
	else
		return 1;
}

static int cache_is_end(struct disk_handle* h, unsigned pos, unsigned size)
{
	return h->cache_list
		&& pos == h->cache_end
		&& (h->cache_end - h->cache_begin) < 2048; /* no more than 1 MB of cache */
}

struct disk_handle* disk_open(const char* dev)
{
	struct disk_handle* h = os_open(dev);
	if (!h)
		return 0;

	h->cache_list = 0;

	return h;
}

struct disk_handle* disk_find(void)
{
	struct disk_handle* h = os_find();
	if (!h)
		return 0;

	h->cache_list = 0;

	return h;
}

int disk_close(struct disk_handle* h)
{
	int r;

	r = cache_flush(h);
	if (r != 0)
		return -1;

	r = os_close(h);
	if (r != 0)
		return -1;

	return 0;
}

int disk_read(struct disk_handle* h, unsigned pos, void* data, unsigned size)
{
	if (cache_is_overlap(h, pos, size)) {
		if (cache_flush(h) != 0)
			return -1;
	}

	return os_read(h, pos, data, size);
}

int disk_write(struct disk_handle* h, unsigned pos, const void* data, unsigned size)
{
	struct disk_cache* c = cache_alloc(pos, data, size);
	if (!c)
		return -1;

	if (cache_is_end(h, pos, size)) {
		c->next = h->cache_list;
		h->cache_list = c;
		h->cache_end = pos + size;
		return 0;
	}

	if (cache_flush(h) != 0) {
		cache_free(c);
		return -1;
	}

	c->next = 0;
	h->cache_list = c;
	h->cache_begin = pos;
	h->cache_end = pos + size;

	return 0;
}

