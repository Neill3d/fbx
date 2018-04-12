
#ifndef _FBXIMPORTER__H_
#define _FBXIMPORTER__H_

#include <fstream>

namespace fbx
{

#define IMPORTER_BUFFER_SIZE	1 << 16

	// forward declaration
	class FBXDocument;

	/////////////////////////////////////////////////////////////////
	//
	class Importer
	{
	public:

		//! a constructor
		Importer();

		//! a destructor
		~Importer();

		bool Initialize(const char *filename);

		bool Import(FBXDocument &pdocument);

		bool Destroy();

	protected:

		char				*mStreamBuffer;
		std::ifstream		mFile;

		void read(std::ifstream &input);
		void read(std::string fname);
	};

};

#endif