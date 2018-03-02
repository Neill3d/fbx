#ifndef FBXDOCUMENT_H
#define FBXDOCUMENT_H

#include "fbxnode.h"

namespace fbx {

class FBXDocument
{
public:
    FBXDocument();
    
	// create some basic document nodes
    void createHeader();
	void createGlobalSettings();
	void createDocuments();
	void createReferences();
	void createDefinitions();

	FBXNode		*findNode(const char *name, const FBXNode *parent);

	FBXNode		&getRoot() const
	{
		return (FBXNode&) root;
	}
	FBXNode		*getRootPtr() const
	{
		return (FBXNode*) &root;
	}

    //std::vector<FBXNode> nodes;

    std::uint32_t getVersion() const;
    void print();

	int64_t generate_uid() { return ++last_uid; }

protected:
    std::uint32_t version;
	int64_t last_uid;

	FBXNode			root;
};

} // namespace fbx

#endif // FBXDOCUMENT_H
