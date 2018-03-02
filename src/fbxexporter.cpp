
#include "fbxexporter.h"
#include "fbxdocument.h"
#include "fbxutil.h"

using namespace fbx;

///

#define _BLOCK_SENTINEL_LENGTH		13

/////////////////////////////////////////////////////////////////////////////////
//

void writerFooter(Writer &writer) {

	uint8_t footer[] = {
		0xfa, 0xbc, 0xab, 0x09,
		0xd0, 0xc8, 0xd4, 0x66, 0xb1, 0x76, 0xfb, 0x83, 0x1c, 0xf7, 0x26, 0x7e, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xe8, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x5a, 0x8c, 0x6a,
		0xde, 0xf5, 0xd9, 0x7e, 0xec, 0xe9, 0x0c, 0xe3, 0x75, 0x8f, 0x29, 0x0b
	};
	for (unsigned int i = 0; i < sizeof(footer); i++) {
		writer.write(footer[i]);
	}
}

void writerFooter2(Writer &writer, int version)
{
	writer.writeBlockSentinelData();

	// foot id
	//const char *FOOT_ID = "\xfa\xbc\xab\x09\xd0\xc8\xd4\x66\xb1\x76\xfb\x83\x1c\xf7\x26\x7e";
	const char *FOOT_ID = "\xfa\xbc\xaf\x0d\xd0\xca\xd2\x61\xb2\x74\xfc\x81\x18\xff\x2e\x72";

	writer.write(FOOT_ID);

	for (int i = 0; i < 4; ++i)
		writer.write((uint8_t)0x00);

	fpos_t ofs = writer.tell();
	size_t pad = 16 - (ofs % 16);

	for (size_t i = 0; i < pad; ++i)
		writer.write((uint8_t)'\x00');

	writer.write((uint32_t)version);

	//# unknown magic(always the same)
	for (int i = 0; i < 120; ++i)
		writer.write((uint8_t)'\x00');

	writer.write("\xf8\x5a\x8c\x6a\xde\xf5\xd9\x7e\xec\xe9\x0c\xe3\x75\x8f\x29\x0b");
}

///////////////////////////////////////////////////////////////////////////////////
//
Exporter::Exporter()
	: mIsBinary(false)
{
	mStreamBuffer = nullptr;
}

Exporter::Exporter(bool isBinary)
	: mIsBinary(isBinary)
{
	mStreamBuffer = nullptr;
}

//! a destructor
Exporter::~Exporter()
{
	Destroy();
}

//
bool Exporter::Initialize(const char *fname, bool isBinary)
{
	mFilename = fname;
	mIsBinary = isBinary;
	
	bool lSuccess = true;

	try
	{
		// buffer
		if (nullptr == mStreamBuffer)
			mStreamBuffer = new char[EXPORTER_BUFFER_SIZE];
		
		mFile.rdbuf()->pubsetbuf(mStreamBuffer, EXPORTER_BUFFER_SIZE);

		std::ios::openmode	openmode = std::ios::out;
		if (true == mIsBinary)
			openmode |= std::ios::binary;

		mFile.open(mFilename.c_str(), openmode);
		
		if (false == mFile.is_open())
			throw std::exception(mFilename.c_str());

		lSuccess = true;
	}
	catch (const std::exception &e)
	{
		lSuccess = false;
		printf("Error exporting a file: %s\n", e.what());
	}

	return lSuccess;
}

//
bool Exporter::Export(const FBXDocument &document)
{
	return (mIsBinary) ? WriteBinary(document) : WriteASCII(document);
}

bool Exporter::WriteBinary(const FBXDocument &document)
{
	bool lSuccess = true;

	//version = 7400;

	uint32_t version = document.getVersion();

	Writer writer(&mFile);
	writer.write("Kaydara FBX Binary  ");
	writer.write((uint8_t)0);
	writer.write((uint8_t)0x1A);
	writer.write((uint8_t)0);
	writer.write(version);

	uint32_t offset = 27; // magic: 21+2, version: 4

	/*
	FBXNode *pHeaderExt = findNode("FBXHeaderExtension", nullptr);
	if (nullptr != pHeaderExt)
	{
	FBXNode *pHeaderVersion = findNode("FBXHeaderVersion", pHeaderExt);
	FBXNode *pFbxVersion = findNode("FBXVersion", pHeaderExt);
	FBXNode *pEncrType = findNode("EnctyptionType", pHeaderExt);
	FBXNode *pSceneInfo = findNode("SceneInfo", pHeaderExt);

	//
	if (nullptr != pHeaderVersion)
	{
	pHeaderVersion->removeProperties(false);
	pHeaderVersion->addProperty(FBX_HEADER_VERSION);
	}
	if (nullptr != pFbxVersion)
	{
	pFbxVersion->removeProperties(false);
	pFbxVersion->addProperty(FBX_VERSION);
	}
	if (nullptr != pEncrType)
	{
	pEncrType->removeProperties(false);
	pEncrType->addProperty(0);
	}

	//
	if (nullptr != pSceneInfo)
	{
	FBXNode *pVersion = findNode("Version", pSceneInfo);
	if (nullptr != pVersion)
	{
	pVersion->removeProperties(false);
	pVersion->addProperty(FBX_SCENEINFO_VERSION);
	}
	FBXNode *pMetaData = findNode("MetaData", pSceneInfo);
	pVersion = findNode("Version", pMetaData);
	if (nullptr != pVersion)
	{
	pVersion->removeProperties(false);
	pVersion->addProperty(FBX_SCENEINFO_VERSION);
	}
	}
	}
	*/

	FBXNode *pRoot = document.getRootPtr();

	ExporterNodeBinary exportNode(this, pRoot);
	uint32_t ret = exportNode.WriteChildren(offset, false);
	lSuccess = (ret > 0);

	writerFooter2(writer, version); // , version);
	//writerFooter(writer);

	return lSuccess;
}
bool Exporter::WriteASCII(const FBXDocument &document)
{
	mFile << "; FBX 7.4.0 project file" << std::endl;
	mFile << "; Exported by Neill3d Exporter 2018" << std::endl;
	mFile << "; -----------------------------------" << std::endl;
	mFile << std::endl;

	FBXNode *pRoot = document.getRootPtr();

	ExporterNodeAscii exportNode(this, pRoot);
	uint32_t ret = exportNode.WriteChildren(0, false);

	return (ret > 0);
}

bool Exporter::Destroy()
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

uint32_t ExporterNodeBinary::getBytes(FBXNode *pNode, bool is_last)
{
	// 3 uints + len + idname
	uint32_t bytes = 13 + pNode->getName().length();

	auto &properties = pNode->getProperties();
	for (auto iter = begin(properties); iter != end(properties); ++iter)
	{
		bytes += iter->getBytes();
	}

	bytes += getBytesChildren(pNode, is_last);
	return bytes;
}

uint32_t ExporterNodeBinary::getBytesChildren(FBXNode *pNode, bool is_last)
{
	uint32_t bytes = 0;

	if (pNode->getChildrenCount() > 0)
	{
		auto &children = pNode->getChildren();

		auto lastIter = begin(children) + (children.size() - 1);
		for (auto iter = begin(children); iter != end(children); ++iter)
		{
			FBXNode *pChild = (FBXNode*) &(*iter);
			bytes += getBytes(pChild, lastIter == iter);
		}
		bytes += _BLOCK_SENTINEL_LENGTH;
	}
	else
	{
		if (0 == pNode->getPropertiesCount())
		{
			if (false == is_last)
				bytes += _BLOCK_SENTINEL_LENGTH;
		}
	}

	return bytes;
}

uint32_t ExporterNodeBinary::getBytesProperties(FBXNode *pNode)
{
	uint32_t bytes = 0;

	auto &properties = pNode->getProperties();
	for (auto iter = begin(properties); iter != end(properties); ++iter)
	{
		bytes += iter->getBytes();
	}
	return bytes;
}

uint32_t ExporterNodeBinary::Write(uint32_t start_offset, bool is_last)
{
	Writer writer(&mExporter->GetStream());

	auto pos = mExporter->GetStream().tellp().seekpos();
	if (pos != start_offset)
	{
		printf("scope lenth not reached, something is wrong (%d)\n", (start_offset - pos));
	}

	uint32_t propertyListLength = getBytesProperties(mNode);

	// 3 uint + 1 len and id
	uint32_t bytes = 13 + mNode->getName().length();
	bytes += propertyListLength;
	bytes += getBytesChildren(mNode, is_last);

	//if(bytes != getBytes(is_last)) throw std::string("bytes != getBytes()");

	uint32_t endOffset = start_offset + bytes;
	writer.write(endOffset); // endOffset
	writer.write((uint32_t)mNode->getPropertiesCount()); // numProperties
	writer.write(propertyListLength); // propertyListLength
	writer.write((uint8_t)mNode->getName().length());
	writer.write(mNode->getNamePtr());

	//std::cout << "so: " << start_offset
	//          << "\tbytes: " << bytes
	//          << "\tnumProp: " << properties.size()
	//          << "\tpropListLen: " << propertyListLength
	//          << "\tnameLen: " << name.length()
	//          << "\tname: " << name << "\n";

	bytes = 13 + mNode->getName().length() + propertyListLength;

	auto &properties = mNode->getProperties();
	for (auto iter = begin(properties); iter != end(properties); ++iter)
	{
		FBXProperty *prop = (FBXProperty*) &(*iter);
		prop->write(mExporter->GetStream());
	}

	bytes += WriteChildren(start_offset + bytes, is_last);

	pos = mExporter->GetStream().tellp().seekpos();
	if (pos != endOffset)
	{
		printf("scope lenth not reached, something is wrong (%d)\n", (endOffset - pos));
	}
	return bytes;
}

uint32_t ExporterNodeBinary::WriteChildren(uint32_t start_offset, bool is_last)
{
	Writer writer(&mExporter->GetStream());
	uint32_t bytes = 0;

	if (mNode->getChildrenCount() > 0)
	{
		auto &children = mNode->getChildren();

		auto lastIter = begin(children) + (children.size() - 1);
		for (auto iter = begin(children); iter != end(children); ++iter)
		{
			ExporterNodeBinary childNode(mExporter, (FBXNode*) &(*iter));
			bytes += childNode.Write(start_offset + bytes, (lastIter == iter));

		}
		writer.writeBlockSentinelData();
		bytes += _BLOCK_SENTINEL_LENGTH;
	}
	else
	{
		if (0 == mNode->getPropertiesCount())
		{
			if (false == is_last)
			{
				writer.writeBlockSentinelData();
				bytes += _BLOCK_SENTINEL_LENGTH;
			}
		}
	}

	return bytes;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;

	std::string strclass(begin(str) + start_pos + 12, end(str)-1);
	str = str.substr(1, start_pos-1);
	str = '\"' + strclass + "::" + str + '\"';

	//str.replace(start_pos, from.length(), to);
	return true;
}

uint32_t ExporterNodeAscii::Write(uint32_t start_offset, bool is_last)
{
	bool lSuccess = true;

	std::ofstream &stream = mExporter->GetStream();

	for (uint32_t i = 0; i < start_offset; ++i)
		stream << '\t';

	stream << mNode->getNamePtr() << ": ";

	auto &props = mNode->getProperties();

	if (props.size() > 0)
	{
		auto last = begin(props) + (props.size() - 1);

		for (auto iter = begin(props); iter != end(props); ++iter)
		{
			std::string str = iter->to_ascii(start_offset);
			if (nullptr != strstr(str.c_str(), "\\u0000\\u0001"))
			{
				replace(str, std::string("\\u0000\\u0001"), std::string("::"));
			}
			stream << str;

			if (iter != last)
				stream << ", ";
			else
				stream << " ";
		}
	}
	
	if (mNode->getChildrenCount() > 0)
	{
		stream << "{" << std::endl;
		if (false == WriteChildren(start_offset + 1, is_last))
			lSuccess = false;

		for (uint32_t i = 0; i < start_offset; ++i)
			stream << '\t';
		stream << "}" << std::endl;
	}
	else
	{
		stream << std::endl;
	}

	return (lSuccess) ? 1 : 0;
}

uint32_t ExporterNodeAscii::WriteChildren(uint32_t start_offset, bool is_last)
{
	bool lSuccess = true;
	auto &children = mNode->getChildren();

	auto last = begin(children) + (children.size() - 1);
	for (auto iter = begin(children); iter != end(children); ++iter)
	{
		ExporterNodeAscii exportNode( mExporter, (FBXNode*) &(*iter));

		if (false == exportNode.Write(start_offset, (iter == last)) )
		{
			lSuccess = false;
		}
	}

	return (lSuccess) ? 1 : 0;
}