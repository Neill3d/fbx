
#ifndef _FBXEXPORTER__H_
#define _FBXEXPORTER__H_

#include <fstream>
#include <stdint.h>

namespace fbx
{

	//////////////////////
	// some forward declaration
	class FBXDocument;
	class FBXNode;
	class FBXProperty;

#define EXPORTER_BUFFER_SIZE		1 << 16

	///////////////////////////////////////////////////////////////////////////////////
	// Exporter class

	class Exporter;

	//////////////////////////////////////////////////////////////
	// ExporterNode
	struct ExporterNode
	{
	public:
		// a constructor
		ExporterNode(Exporter *pExporter, FBXNode *pNode)
			: mExporter(pExporter)
			, mNode(pNode)
		{}

		virtual uint32_t Write(uint32_t start_offset, bool is_last) {
			return 0;
		}
		virtual uint32_t WriteChildren(uint32_t start_offset, bool is_last) {
			return 0;
		}

	protected:
		Exporter *mExporter;
		FBXNode	*mNode;
	};

	//////////////////////////////////////////////////////////////
	// ExporterNodeBinary
	struct ExporterNodeBinary : public ExporterNode
	{
	public:
		//! a constructor
		ExporterNodeBinary(Exporter *pExporter, FBXNode *pNode)
			: ExporterNode(pExporter, pNode)
		{}

		//
		virtual uint32_t Write(uint32_t start_offset, bool is_last) override;
		virtual uint32_t WriteChildren(uint32_t start_offset, bool is_last) override;

	protected:

		static uint32_t getBytes(FBXNode *pNode, bool is_last);
		static uint32_t getBytesChildren(FBXNode *pNode, bool is_last);
		static uint32_t getBytesProperties(FBXNode *pNode);
	};

	//////////////////////////////////////////////////////////////////
	// ExporterNodeAscii
	struct ExporterNodeAscii : public ExporterNode
	{
	public:
		// a constructor
		ExporterNodeAscii(Exporter *pExporter, FBXNode *pNode)
			: ExporterNode(pExporter, pNode)
		{}

		//
		virtual uint32_t Write(uint32_t start_offset, bool is_last) override;
		virtual uint32_t WriteChildren(uint32_t start_offset, bool is_last) override;
	};

	struct ExporterProperty
	{

	};

	//////////////////////////////////////////////////////////////////////////////////
	// Exporter
	class Exporter
	{
	public:
		//! a constructor
		Exporter();
		//
		Exporter(bool isBinary);

		//! a destructor
		~Exporter();

		//
		bool Initialize(const char *filename, bool isBinary);

		//
		bool Export(const FBXDocument &document);

		bool Destroy();

		std::ofstream &GetStream()
		{
			return mFile;
		}

	protected:

		char			*mStreamBuffer;
		std::ofstream	mFile;

		std::string		mFilename;
		bool			mIsBinary;

		bool WriteBinary(const FBXDocument &document);
		bool WriteASCII(const FBXDocument &document);
	};


};

#endif