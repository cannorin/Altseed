﻿
#include "ace.File_Imp.h"
#include "ace.StaticFile_Imp.h"

namespace ace
{
	StaticFile_Imp::StaticFile_Imp(File_Imp* file, const astring& cacheKey, std::shared_ptr<BaseFile>& baseFile)
		: file(file)
		, cacheKey(cacheKey)
	{
		baseFile->ReadAllBytes(m_buffer);

		m_path = baseFile->GetFullPath();

		SafeAddRef(file);
	}

	StaticFile_Imp::StaticFile_Imp(File_Imp* file, const astring& cacheKey, const std::shared_ptr<BaseFile>& packedFile, PackFileInternalHeader& internalHeader, std::shared_ptr<Decryptor> decryptor)
		: file(file)
		, cacheKey(cacheKey)
	{
		packedFile->Seek(internalHeader.GetOffset());
		packedFile->ReadBytes(m_buffer, internalHeader.GetSize(), decryptor.get(), internalHeader.GetOffset());

		m_path = packedFile->GetFullPath() + internalHeader.GetFileName();

		SafeAddRef(file);
	}

	StaticFile_Imp::~StaticFile_Imp()
	{
		file->UnregisterStaticFile(cacheKey);
		SafeRelease(file);
	}
}
