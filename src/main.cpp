#include <stdint.h>
#include <iostream>

#include "fbximporter.h"
#include "fbxexporter.h"
#include "fbxdocument.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    if(argc < 2) {
        std::cout << "Specify file which you want to \"copy\"" << std::endl;
        return 1;
    }

    
    fbx::FBXDocument doc;
    std::cout << "Reading " << argv[1] << std::endl;

	fbx::Importer lImporter;
	bool lSuccess = lImporter.Initialize(argv[1]);

	if (lSuccess)
	{
		lImporter.Import(doc);

		//

		fbx::Exporter	lExporter;

		std::cout << "Writing test.fbx" << std::endl;
		lExporter.Initialize("test.fbx", false);

		lExporter.Export(doc);
	}

    return 0;
}
