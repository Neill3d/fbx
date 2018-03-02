
#include "fbximporter.h"
#include "fbxdocument.h"
#include "fbxutil.h"

using namespace fbx;

///////////////////////////////////////////////////////////////////////////
//

bool checkMagic(Reader &reader)
{
	std::string magic("Kaydara FBX Binary  ");
	for (char c : magic) {
		if (reader.readUint8() != c) return false;
	}
	if (reader.readUint8() != 0x00) return false;
	if (reader.readUint8() != 0x1A) return false;
	if (reader.readUint8() != 0x00) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//

Importer::Importer()
{
	mStreamBuffer = nullptr;
}

Importer::~Importer()
{
	Destroy();
}

bool Importer::Initialize(const char *filename)
{
	bool lSuccess = true;
	
	// buffer
	if (nullptr == mStreamBuffer)
		mStreamBuffer = new char[IMPORTER_BUFFER_SIZE];

	mFile.rdbuf()->pubsetbuf(mStreamBuffer, IMPORTER_BUFFER_SIZE);

	mFile.open(filename, std::ios::in | std::ios::binary);
	
	if (false == mFile.is_open())
		lSuccess = false;

	return lSuccess;
}

bool Importer::Import(FBXDocument &document)
{
	bool lSuccess = true;

	Reader reader(&mFile);
	
	try
	{

		mFile >> std::noskipws;
		if (!checkMagic(reader)) 
			throw std::exception("Not a FBX file");

		// uint32_t 
		//version = 7300;
		
		uint32_t version = reader.readUint32();
		uint32_t maxVersion = 7500;

		if (version > maxVersion)
		{
			throw std::exception("Unsupported FBX version");
		}

		uint64_t start_offset = 27; // magic: 21+2, version: 4
		FBXNode &root = document.getRoot();

		while (true)
		{
			FBXNode node;
			start_offset += node.read(mFile, start_offset, version);
			if (node.isNull()) break;
			root.addChild(node);
		};
	}
	catch (std::exception &e)
	{
		lSuccess = false;
		printf("error while import a document - %s\n", e.what());
	}

	return lSuccess;
}

bool Importer::Destroy()
{
	if (mFile.is_open())
		mFile.close();

	if (nullptr != mStreamBuffer)
	{
		delete[] mStreamBuffer;
		mStreamBuffer = nullptr;
	}

	return true;
}

