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

#include "winLnkFile.h"
#include "misc/debugMsgs.h"
#include "misc/endianSwitch.h"

winLnkFile::winLnkFile(string strFilename)	:	binDataFile(strFilename),
												m_ulFileLocationOffset(0) {
	memset(&m_lnkFileHeader, 0, LNKFILE_HEADER_LENGTH);
	memset(&m_shellItemList, 0, SHELL_ITEM_LIST_LENGTH);
	memset(&m_fileLocationInfo, 0, FILE_LOC_INFO_LENGTH);
	memset(&m_localVolumeInfo, 0, LOCAL_VOL_INFO_LENGTH);
	memset(&m_networkVolumeInfo, 0, NETWORK_VOL_INFO_LENGTH);
}

winLnkFile::~winLnkFile() {
}

int winLnkFile::load() {
	return loadFileHeader() + loadShellItemList() + loadFileLocationInfo() + loadStrings();
}

int winLnkFile::loadFileHeader() {
	DEBUG_INFO("winLnkFile::loadFileHeader()...");

	int rv = -1;
	if (isOpen()) {
		if (getData(&m_lnkFileHeader, LNKFILE_HEADER_LENGTH, 0) >= 0) {
			LITTLETOHOST32(m_lnkFileHeader.dwCustomIconIndex);
			LITTLETOHOST32(m_lnkFileHeader.dwFileAttr);
			LITTLETOHOST32(m_lnkFileHeader.dwFileLength);
			LITTLETOHOST32(m_lnkFileHeader.dwFlags);
			LITTLETOHOST32(m_lnkFileHeader.dwHeaderID);
			LITTLETOHOST32(m_lnkFileHeader.dwHotKey);
			LITTLETOHOST32(m_lnkFileHeader.dwShowWndValue);
			LITTLETOHOST64(m_lnkFileHeader.qwAccessedTime);
			LITTLETOHOST64(m_lnkFileHeader.qwCreatedTime);
			LITTLETOHOST64(m_lnkFileHeader.qwModifiedTime);
			LITTLETOHOST32(m_lnkFileHeader.lnkGUID.dwData1);
			LITTLETOHOST16(m_lnkFileHeader.lnkGUID.wData2);
			LITTLETOHOST16(m_lnkFileHeader.lnkGUID.wData3);
			
			if (m_lnkFileHeader.dwHeaderID == LNKFILE_HEADER_ID) {
				rv = 0;
			} else {
				DEBUG_ERROR("winLnkFile::loadFileHeader() Invalid header ID:" << m_lnkFileHeader.dwHeaderID);
			}
		} else {
			DEBUG_ERROR("winLnkFile::loadFileHeader() Failure reading data.");
		}
	} else {
		DEBUG_ERROR("winLnkFile::loadFileHeader() File is not open.");
	}
	return rv;
}

int winLnkFile::loadShellItemList() {
	DEBUG_INFO("winLnkFile::loadShellItemList()...");

	int rv = -1;
	if (isOpen()) {
		if (getData(&m_shellItemList, SHELL_ITEM_LIST_LENGTH, LNKFILE_HEADER_LENGTH, NULL) >= 0) {
			LITTLETOHOST16(m_shellItemList.wListLength);
			rv = 0;
		} else {
			DEBUG_ERROR("winLnkFile::loadShellItemList() Failure reading data.");
		}
	} else {
		DEBUG_ERROR("winLnkFile::loadShellItemList() File is not open.");
	}	
	return rv;
}

int winLnkFile::loadFileLocationInfo() {
	DEBUG_INFO("winLnkFile::loadFileLocationInfo()...");

	int rv = -1;
	if (isOpen()) {
		m_ulFileLocationOffset = LNKFILE_HEADER_LENGTH + SHELL_ITEM_LIST_LENGTH + m_shellItemList.wListLength;
		if (getData(&m_fileLocationInfo, FILE_LOC_INFO_LENGTH, m_ulFileLocationOffset, NULL) >= 0) {
			LITTLETOHOST32(m_fileLocationInfo.dwBasePathnameOffset);
			LITTLETOHOST32(m_fileLocationInfo.dwFlags);
			LITTLETOHOST32(m_fileLocationInfo.dwLength);
			LITTLETOHOST32(m_fileLocationInfo.dwLocalVolumeInfoOffset);
			LITTLETOHOST32(m_fileLocationInfo.dwNetworkVolumeInfoOffset);
			LITTLETOHOST32(m_fileLocationInfo.dwNextStructure);
			LITTLETOHOST32(m_fileLocationInfo.dwRemainingPathOffset);
			
			DEBUG_INFO("winLnkFile::loadFileLocationInfo() File location info offset " << m_ulFileLocationOffset << ", length " << m_fileLocationInfo.dwLength);
			rv = 0;

			if (m_fileLocationInfo.dwBasePathnameOffset > 0) {
				if (getString(&m_strBasePath, m_ulFileLocationOffset + m_fileLocationInfo.dwBasePathnameOffset, 0) < 0) {
					DEBUG_WARNING("winLnkFile::loadFileLocationInfo() Failure reading base path");
				}
			}
			
			if (m_fileLocationInfo.dwRemainingPathOffset) {
				if (getString(&m_strFinalPath, m_ulFileLocationOffset + m_fileLocationInfo.dwRemainingPathOffset, 0) < 0) {
					DEBUG_WARNING("winLnkFile::loadFileLocationInfo() Failure reading remaining path");
				}
			}

			if (m_fileLocationInfo.dwLocalVolumeInfoOffset > 0) {
				DEBUG_INFO("winLnkFile::loadFileLocationInfo() Reading local volume info");
				if (getData(&m_localVolumeInfo, LOCAL_VOL_INFO_LENGTH, m_ulFileLocationOffset + m_fileLocationInfo.dwLocalVolumeInfoOffset, NULL) >= 0) {
					LITTLETOHOST32(m_localVolumeInfo.dwLength);
					LITTLETOHOST32(m_localVolumeInfo.dwNameOffset);
					LITTLETOHOST32(m_localVolumeInfo.dwSerialNumber);
					LITTLETOHOST32(m_localVolumeInfo.dwType);
					
					DEBUG_INFO("winLnkFile::loadFileLocationInfo() Reading local volume name");
					if (getString(&m_strVolumeLabel, m_ulFileLocationOffset + m_fileLocationInfo.dwLocalVolumeInfoOffset + m_localVolumeInfo.dwNameOffset, 0) < 0) {
						DEBUG_WARNING("winLnkFile::loadFileLocationInfo() Failure reading local volume name.");
					}
				} else {
					DEBUG_WARNING("winLnkFile::loadFileLocationInfo() Failure reading local volume info.");
				}
			}

			if (m_fileLocationInfo.dwNetworkVolumeInfoOffset > 0) {
				DEBUG_INFO("winLnkFile::loadFileLocationInfo() Reading network volume info");
				if (getData(&m_networkVolumeInfo, NETWORK_VOL_INFO_LENGTH, m_ulFileLocationOffset + m_fileLocationInfo.dwNetworkVolumeInfoOffset, NULL) >= 0) {
					LITTLETOHOST32(m_networkVolumeInfo.dwLength);
					LITTLETOHOST32(m_networkVolumeInfo.dwShareNameOffset);
					
					DEBUG_INFO("winLnkFile::loadFileLocationInfo() Reading network share name");
					if (getString(&m_strNetworkShare, m_ulFileLocationOffset + m_fileLocationInfo.dwNetworkVolumeInfoOffset + m_networkVolumeInfo.dwShareNameOffset, 0) < 0) {
						DEBUG_WARNING("winLnkFile::loadFileLocationInfo() Failure reading network share name.");
					}
				} else {
					DEBUG_WARNING("winLnkFile::loadFileLocationInfo() Failure reading network volume info.");
				}
			}
		} else {
			DEBUG_ERROR("winLnkFile::loadFileLocationInfo() Failure reading data.");
		}
	} else {
		DEBUG_ERROR("winLnkFile::loadFileLocationInfo() File is not open.");
	}
	return rv;
}

int winLnkFile::loadStrings() {
	DEBUG_INFO("winLnkFile::loadStrings()...");

	int rv = -1;

	if (movePos(m_ulFileLocationOffset + m_fileLocationInfo.dwLength) >= 0) {
		rv = 0;
		
		LNK_STRING stringStruct;

		if (hasDescription()) {
			DEBUG_INFO("winLnkFile::loadStrings() Reading description");
			if (getData(&stringStruct, LNK_STRING_LENGTH, NULL) >= 0) {
				LITTLETOHOST16(stringStruct.wStringLength);
				
				if (getTwoByteCharString(&m_strDescription, stringStruct.wStringLength, true) >= 0) {
					DEBUG_INFO("winLnkFile::loadStrings() Read description of length " << stringStruct.wStringLength);
				} else {
					DEBUG_ERROR("winLnkFile::loadStrings() Failure reading description string.");
					rv = -1;
				}
			} else {
				DEBUG_ERROR("winLnkFile::loadStrings() Failure reading description string length.");
				rv = -1;
			}
		}

		if (hasRelativePath()) {
			DEBUG_INFO("winLnkFile::loadStrings() Reading relative path");
			if (getData(&stringStruct, LNK_STRING_LENGTH, NULL) >= 0) {
				LITTLETOHOST16(stringStruct.wStringLength);
				
				if (getTwoByteCharString(&m_strRelativePath, stringStruct.wStringLength, true) >= 0) {
				} else {
					DEBUG_ERROR("winLnkFile::loadStrings() Failure reading relative path string.");
					rv = -1;
				}
			} else {
				DEBUG_ERROR("winLnkFile::loadStrings() Failure reading relative path string length.");
				rv = -1;
			}
		}

		if (hasWorkingDir()) {
			DEBUG_INFO("winLnkFile::loadStrings() Reading working dir");
			if (getData(&stringStruct, LNK_STRING_LENGTH, NULL) >= 0) {
				LITTLETOHOST16(stringStruct.wStringLength);
				
				if (getTwoByteCharString(&m_strWorkingDir, stringStruct.wStringLength, true) >= 0) {
				} else {
					DEBUG_ERROR("winLnkFile::loadStrings() Failure reading working dir string.");
					rv = -1;
				}
			} else {
				DEBUG_ERROR("winLnkFile::loadStrings() Failure reading working dir string length.");
				rv = -1;
			}
		}

		if (hasCommandLineArgs()) {
			DEBUG_INFO("winLnkFile::loadStrings() Reading command line args");
			if (getData(&stringStruct, LNK_STRING_LENGTH, NULL) >= 0) {
				LITTLETOHOST16(stringStruct.wStringLength);
				
				if (getTwoByteCharString(&m_strCommandLine, stringStruct.wStringLength, true) >= 0) {
				} else {
					DEBUG_ERROR("winLnkFile::loadStrings() Failure reading command line args string.");
					rv = -1;
				}
			} else {
				DEBUG_ERROR("winLnkFile::loadStrings() Failure reading command line args string length.");
				rv = -1;
			}
		}

		if (hasCustomIcon()) {
			DEBUG_INFO("winLnkFile::loadStrings() Reading custom icon");
			if (getData(&stringStruct, LNK_STRING_LENGTH, NULL) >= 0) {
				LITTLETOHOST16(stringStruct.wStringLength);
				
				if (getTwoByteCharString(&m_strIconFilename, stringStruct.wStringLength, true) >= 0) {
				} else {
					DEBUG_ERROR("winLnkFile::loadStrings() Failure reading custom icon string.");
					rv = -1;
				}
			} else {
				DEBUG_ERROR("winLnkFile::loadStrings() Failure reading custom icon string length.");
				rv = -1;
			}
		}
	} else {
		DEBUG_ERROR("winLnkFile::loadStrings() Failure seeking past file location info structure.");
	}
	
	return rv;
}

string winLnkFile::getGUID() {
	char guid[40];
	snprintf(guid, 40, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", m_lnkFileHeader.lnkGUID.dwData1, m_lnkFileHeader.lnkGUID.wData2, m_lnkFileHeader.lnkGUID.wData3, m_lnkFileHeader.lnkGUID.bData4[0], m_lnkFileHeader.lnkGUID.bData4[1], m_lnkFileHeader.lnkGUID.bData4[2], m_lnkFileHeader.lnkGUID.bData4[3], m_lnkFileHeader.lnkGUID.bData4[4], m_lnkFileHeader.lnkGUID.bData4[5], m_lnkFileHeader.lnkGUID.bData4[6], m_lnkFileHeader.lnkGUID.bData4[7]);
	return string(guid);
}

u_int64_t winLnkFile::getCreatedTime() { 
	return m_lnkFileHeader.qwCreatedTime;
}

u_int64_t winLnkFile::getAccessedTime() { 
	return m_lnkFileHeader.qwAccessedTime;
}

u_int64_t winLnkFile::getModifiedTime() { 
	return m_lnkFileHeader.qwModifiedTime;
}

string winLnkFile::getLocalVolumeSerialString() {
	char cstr[20];
	u_int32_t uiSerialNum = getLocalVolumeSerial();
	snprintf(cstr, 20, "%04X-%04X", uiSerialNum >> 16, uiSerialNum & 0xFFFF);
	return string(cstr);
}
