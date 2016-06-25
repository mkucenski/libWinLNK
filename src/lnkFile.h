// Copyright 2007 Matthew A. Kucenski
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _LNKFILE_H_
#define _LNKFILE_H_

#include "misc/windowsTypes.h"

#define LNKFILE_HEADER_ID 0x0000004c	//"L"

//Basic File Layout

//LNKFILE_HEADER
//SHELL_ITEM_LIST
//   SHELL_ITEM 1
//   SHELL_ITEM 2
//	 ...
//   SHELL_ITEM x
//FILE_LOC_INFO
//   LOCAL_VOL_INFO
//   NETWORK_VOL_INFO
//LNK_STRING Description
//LNK_STRING RelativePath
//LNK_STRING WorkingDir
//LNK_STRING CommandLine
//LNK_STRING IconFilename

typedef struct _GUID {
	DWORD		dwData1;
	WORD		wData2;
	WORD		wData3;
	BYTE		bData4[8];
} __attribute__((packed)) GUID;
#define GUID_LENGTH 16

typedef struct _LNKFILE_HEADER {
	DWORD	dwHeaderID;
	GUID	lnkGUID;
	DWORD	dwFlags;
	DWORD	dwFileAttr;
	QWORD	qwCreatedTime;
	QWORD	qwAccessedTime;
	QWORD	qwModifiedTime;
	DWORD	dwFileLength;
	DWORD	dwCustomIconIndex;
	DWORD	dwShowWndValue;
	DWORD	dwHotKey;
	DWORD	dwUnknown[2];
} __attribute__((packed)) LNKFILE_HEADER;
#define LNKFILE_HEADER_LENGTH 76

typedef struct _SHELL_ITEM_LIST {
	WORD	wListLength;
	//SHELL_ITEM ShellItemList[];
} __attribute__((packed)) SHELL_ITEM_LIST;	
#define SHELL_ITEM_LIST_LENGTH 2

typedef struct _SHELL_ITEM {
	WORD	wLength;
	//UNKNOWN
} __attribute__((packed)) SHELL_ITEM;
#define SHELL_ITEM_LENGTH 2

typedef struct _FILE_LOC_INFO {
	DWORD	dwLength;
	DWORD	dwNextStructure;
	DWORD	dwFlags;
	DWORD	dwLocalVolumeInfoOffset;
	DWORD	dwBasePathnameOffset;
	DWORD	dwNetworkVolumeInfoOffset;
	DWORD	dwRemainingPathOffset;
	//LOCAL_VOL_INFO	LocalVolumeInfo;
	//NETWORK_VOL_INFO	NetworkVolumeInfo;
} __attribute__((packed)) FILE_LOC_INFO;
#define FILE_LOC_INFO_LENGTH 28

typedef struct _LOCAL_VOL_INFO {
	DWORD	dwLength;
	DWORD	dwType;
	DWORD	dwSerialNumber;
	DWORD	dwNameOffset;
	//CHAR	Label[];
} __attribute__((packed)) LOCAL_VOL_INFO;
#define LOCAL_VOL_INFO_LENGTH 16

typedef struct _NETWORK_VOL_INFO {
	DWORD	dwLength;
	DWORD	dwUnknown1;
	DWORD	dwShareNameOffset;
	DWORD	dwUnknown2;
	DWORD	dwUnknown3;
	//CHAR	ShareName[];
} __attribute__((packed)) NETWORK_VOL_INFO;
#define NETWORK_VOL_INFO_LENGTH 20

typedef struct _LNK_STRING {
	WORD wStringLength;
	//WCHAR string[wStringLength];
} __attribute__((packed)) LNK_STRING;
#define LNK_STRING_LENGTH 2

#define LNKFLAGS_SHELLITEM_MASK		0x01
#define LNKFLAGS_FILE_DIR_MASK		0x02
#define LNKFLAGS_DESCRIPTION_MASK	0x04
#define LNKFLAGS_RELATIVEPATH_MASK	0x08
#define LNKFLAGS_WORKINGDIR_MASK	0x10
#define LNKFLAGS_CMDARGS_MASK		0x20
#define LNKFLAGS_CUSTOMICON_MASK	0x40

#define LNKATTR_READONLY_MASK		0x0001
#define LNKATTR_HIDDEN_MASK			0x0002
#define LNKATTR_SYSTEM_MASK			0x0004
#define LNKATTR_VOLUME_LABEL_MASK	0x0008
#define LNKATTR_DIRECTORY_MASK		0x0010
#define LNKATTR_ARCHIVE_MASK		0x0020
#define LNKATTR_ENCRYPTED_MASK		0x0040
#define LNKATTR_NORMAL_MASK			0x0080
#define LNKATTR_TEMP_MASK			0x0100
#define LNKATTR_SPARSE_MASK			0x0200
#define LNKATTR_REPARSE_MASK		0x0400
#define LNKATTR_COMPRESSED_MASK		0x0800
#define LNKATTR_OFFLINE_MASK		0x1000

#define FILEINFO_AVAIL_LOCAL_MASK	0x01
#define FILEINFO_AVAIL_NETWORK_MASK	0x02

#define	LOCALVOLINFO_TYPE_UNKNOWN		0
#define	LOCALVOLINFO_TYPE_NO_ROOT_DIR	1
#define	LOCALVOLINFO_TYPE_REMOVABLE		2
#define	LOCALVOLINFO_TYPE_FIXED			3
#define	LOCALVOLINFO_TYPE_REMOTE		4
#define	LOCALVOLINFO_TYPE_CDROM			5
#define	LOCALVOLINFO_TYPE_RAM			6

#endif //_LNKFILE_H_
