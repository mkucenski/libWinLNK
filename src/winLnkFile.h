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

//#define _DEBUG_

#ifndef _WINLNKFILE_H_
#define _WINLNKFILE_H_

#include "lnkFile.h"
#include "libbinData/src/binDataFile.h"
#include <string>
#include <fstream>
using namespace std;

#include <sys/types.h>

class winLnkFile : public binDataFile {
	public:
		winLnkFile(string strFilename);
		~winLnkFile();
		
		int load();
		
		string getGUID();
		
		bool isReadOnly() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_READONLY_MASK) > 0; };
		bool isHidden() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_HIDDEN_MASK) > 0; };
		bool isSystem() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_SYSTEM_MASK) > 0; };
		bool isVolumeLabel() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_VOLUME_LABEL_MASK) > 0; };
		bool isDirectory() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_DIRECTORY_MASK) > 0; };
		bool isArchive() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_ARCHIVE_MASK) > 0; };
		bool isEncrypted() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_ENCRYPTED_MASK) > 0; };
		bool isNormal() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_NORMAL_MASK) > 0; };
		bool isTemporary() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_TEMP_MASK) > 0; };
		bool isSparseFile() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_SPARSE_MASK) > 0; };
		bool hasReparsePointData() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_REPARSE_MASK) > 0; };
		bool isCompressed() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_COMPRESSED_MASK) > 0; };
		bool isOffline() { return (m_lnkFileHeader.dwFileAttr & LNKATTR_OFFLINE_MASK) > 0; };
		
		u_int64_t getCreatedTime();
		u_int64_t getAccessedTime();
		u_int64_t getModifiedTime();
		
		unsigned long getFileLength() { return m_lnkFileHeader.dwFileLength; };
		u_int32_t getIconNumber() { return m_lnkFileHeader.dwCustomIconIndex; };
		u_int32_t getHotKey() { return m_lnkFileHeader.dwHotKey; };
		u_int32_t getShowWindowValue() { return m_lnkFileHeader.dwShowWndValue; };
		
		bool isAvailableLocal() { return (m_fileLocationInfo.dwFlags & FILEINFO_AVAIL_LOCAL_MASK) > 0; };
		bool isAvailableNetwork() { return (m_fileLocationInfo.dwFlags & FILEINFO_AVAIL_NETWORK_MASK) > 0; };
		string getBasePath() { return m_strBasePath; };
		string getFinalPath() { return m_strFinalPath; };
		u_int32_t getLocalVolumeType() { return m_localVolumeInfo.dwType; };
		u_int32_t getLocalVolumeSerial() { return m_localVolumeInfo.dwSerialNumber; };
		string getLocalVolumeSerialString();
		string getLocalVolumeName() { return m_strVolumeLabel; };
		string getNetworkShare() { return m_strNetworkShare; };
		
		string getDescription() { return m_strDescription; };
		string getRelativePath() { return m_strRelativePath; };
		string getWorkingDir() { return m_strWorkingDir; };
		string getCommandLine() { return m_strCommandLine; };
		string getIconFilename() { return m_strIconFilename; };

	private:
		int loadFileHeader();
		LNKFILE_HEADER m_lnkFileHeader;
		
		int loadShellItemList();
		SHELL_ITEM_LIST m_shellItemList;
		
		int loadFileLocationInfo();
		FILE_LOC_INFO m_fileLocationInfo;
		u_int32_t m_ulFileLocationOffset;
		string m_strBasePath;
		string m_strFinalPath;
		LOCAL_VOL_INFO m_localVolumeInfo;
		string m_strVolumeLabel;
		NETWORK_VOL_INFO m_networkVolumeInfo;
		string m_strNetworkShare;

		int loadStrings();
		string m_strDescription;
		string m_strRelativePath;
		string m_strWorkingDir;
		string m_strCommandLine;
		string m_strIconFilename;

		bool hasDescription() { return (m_lnkFileHeader.dwFlags & LNKFLAGS_DESCRIPTION_MASK) > 0; };
		bool hasRelativePath() { return (m_lnkFileHeader.dwFlags & LNKFLAGS_RELATIVEPATH_MASK) > 0; };
		bool hasWorkingDir() { return (m_lnkFileHeader.dwFlags & LNKFLAGS_WORKINGDIR_MASK) > 0; };
		bool hasCommandLineArgs() { return (m_lnkFileHeader.dwFlags & LNKFLAGS_CMDARGS_MASK) > 0; };
		bool hasCustomIcon() { return (m_lnkFileHeader.dwFlags & LNKFLAGS_CUSTOMICON_MASK) > 0; };
};

#endif //_WINLNKFILE_H_
