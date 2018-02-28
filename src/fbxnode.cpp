#include "fbxnode.h"

#include "fbxutil.h"
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::uint32_t;
using std::uint8_t;

namespace fbx {

#define _BLOCK_SENTINEL_LENGTH		13

FBXNode::FBXNode()
{
}

FBXNode::FBXNode(const char *_name)
	:name(_name) 
{}

uint64_t FBXNode::read(std::ifstream &input, uint64_t start_offset, uint16_t version)
{
    Reader reader(&input);
    uint64_t bytes = 0;
	
	uint64_t endOffset;
	uint64_t numProperties;
	uint64_t propertyListLength;

	if (version >= 7500)
	{
		endOffset = reader.readUint64();
		numProperties = reader.readUint64();
		propertyListLength = reader.readUint64();
	}
	else
	{
		endOffset = reader.readUint32();
		numProperties = reader.readUint32();
		propertyListLength = reader.readUint32();
	}
    
    uint8_t nameLength = reader.readUint8();
    name = reader.readString(nameLength);

	int BLOCK_SENTINEL_LENGTH = (version >= 7500 ? 25 : 13);
    bytes += BLOCK_SENTINEL_LENGTH + nameLength;

    //std::cout << "so: " << start_offset
    //          << "\tbytes: " << (endOffset == 0 ? 0 : (endOffset - start_offset))
    //          << "\tnumProp: " << numProperties
    //          << "\tpropListLen: " << propertyListLength
    //          << "\tnameLen: " << std::to_string(nameLength)
    //          << "\tname: " << name << "\n";

    for(uint32_t i = 0; i < numProperties; i++) {

		FBXProperty prop(input);
        addProperty(prop);
    }
    bytes += propertyListLength;

    while(start_offset + bytes < endOffset) {
        FBXNode child;
        bytes += child.read(input, start_offset + bytes, version);

		if (false == child.isNull())
			addChild(std::move(child));
    }
    return bytes;
}

void FBXNode::removeProperties(bool recursive)
{
	properties.clear();

	for (auto child : children) child.removeProperties(recursive);
}

uint32_t FBXNode::write(std::ofstream &output, uint32_t start_offset, bool is_last)
{
    Writer writer(&output);

	auto pos = output.tellp().seekpos();
	if (pos != start_offset)
	{
		printf("scope lenth not reached, something is wrong (%d)\n", (start_offset - pos));
	}

    if(isNull()) {
        //std::cout << "so: " << start_offset
        //          << "\tbytes: 0"
        //          << "\tnumProp: 0"
        //          << "\tpropListLen: 0"
        //          << "\tnameLen: 0"
        //          << "\tname: \n";
        for(int i = 0; i < 13; i++) 
			writer.write((uint8_t) 0);
        return 13;
    }

	//removeProperties(true);

	uint32_t propertyListLength = getBytesProperties();
    
	// 3 uint + 1 len and id
	uint32_t bytes = 13 + name.length();
	bytes += propertyListLength;
	bytes += getBytesChildren(is_last);

    //if(bytes != getBytes(is_last)) throw std::string("bytes != getBytes()");

	uint32_t endOffset = start_offset + bytes;
    writer.write(endOffset); // endOffset
    writer.write((uint32_t) properties.size()); // numProperties
    writer.write(propertyListLength); // propertyListLength
    writer.write((uint8_t) name.length());
    writer.write(name);

    //std::cout << "so: " << start_offset
    //          << "\tbytes: " << bytes
    //          << "\tnumProp: " << properties.size()
    //          << "\tpropListLen: " << propertyListLength
    //          << "\tnameLen: " << name.length()
    //          << "\tname: " << name << "\n";

    bytes = 13 + name.length() + propertyListLength;

	for (auto iter = begin(properties); iter != end(properties); ++iter)
	{
		iter->write(output);
	}

	bytes += write_children(output, start_offset + bytes, is_last);

	pos = output.tellp().seekpos();
	if (pos != endOffset)
	{
		printf("scope lenth not reached, something is wrong (%d)\n", (endOffset - pos));
	}
    return bytes;
}

uint32_t FBXNode::write_children(std::ofstream &output, uint32_t start_offset, bool is_last)
{
	Writer writer(&output);
	uint32_t bytes = 0;

	if (children.size())
	{
		auto lastIter = begin(children) + (children.size() - 1);
		for (auto iter = begin(children); iter != end(children); ++iter)
		{
			bytes += iter->write(output, start_offset + bytes, (lastIter == iter));
		}
		writer.writeBlockSentinelData();
		bytes += _BLOCK_SENTINEL_LENGTH;
	}
	else
	{
		if (0 == properties.size())
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

void FBXNode::print(std::string prefix)
{
    cout << prefix << "{ \"name\": \"" << name << "\"" << (properties.size() + children.size() > 0 ? ",\n" : "\n");
    if(properties.size() > 0) {
        cout << prefix << "  \"properties\": [\n";
        bool hasPrev = false;
        for(FBXProperty prop : properties) {
            if(hasPrev) cout << ",\n";
            cout << prefix << "    { \"type\": \"" << prop.getType() << "\", \"value\": " << prop.to_string() << " }";
            hasPrev = true;
        }
        cout << "\n";
        cout << prefix << "  ]" << (children.size() > 0 ? ",\n" : "\n");

    }

    if(children.size() > 0) {
        cout << prefix << "  \"children\": [\n";
        bool hasPrev = false;
        for(FBXNode node : children) {
            if(hasPrev) cout << ",\n";
            node.print(prefix+"    ");
            hasPrev = true;
        }
        cout << "\n";
        cout << prefix << "  ]\n";
    }

    cout << prefix << "}";

}

bool FBXNode::isNull()
{
    return children.size() == 0
            && properties.size() == 0
            && name.length() == 0;
}

// primitive values
void FBXNode::addProperty(int16_t v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(bool v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(int32_t v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(float v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(double v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(int64_t v) { addProperty(FBXProperty(v)); }
// arrays
void FBXNode::addProperty(const std::vector<bool> &v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(const std::vector<int32_t> &v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(const std::vector<float> &v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(const std::vector<double> &v) { addProperty(FBXProperty(v)); }
void FBXNode::addProperty(const std::vector<int64_t> &v) { addProperty(FBXProperty(v)); }
// raw / string
void FBXNode::addProperty(const std::vector<uint8_t> &v, uint8_t type) 
{
	FBXProperty prop(v, type);
	addProperty(prop); 
}
void FBXNode::addProperty(const std::string &v) 
{
	FBXProperty prop(v);
	addProperty(prop); 
}
void FBXNode::addProperty(const char *v) 
{ 
	FBXProperty prop(v);
	addProperty(prop); 
}

void FBXNode::addProperty(FBXProperty &prop) { properties.push_back(prop); }


void FBXNode::addPropertyNode(const char *name, int16_t v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, bool v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, int32_t v) 
{ 
	FBXNode n(name); 
	//n.addProperty(v); 
	addChild(n); 
}
void FBXNode::addPropertyNode(const char *name, float v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, double v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, int64_t v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const std::vector<bool> &v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const std::vector<int32_t> &v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const std::vector<float> &v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const std::vector<double> &v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const std::vector<int64_t> &v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const std::vector<uint8_t> &v, uint8_t type) { FBXNode n(name); n.addProperty(v, type); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const std::string &v) { FBXNode n(name); n.addProperty(v); addChild(n); }
void FBXNode::addPropertyNode(const char *name, const char *v) { FBXNode n(name); n.addProperty(v); addChild(n); }

void FBXNode::addChild(FBXNode &child) { children.push_back(child); }

uint32_t FBXNode::getBytes(bool is_last) 
{    
	// 3 uints + len + idname
	uint32_t bytes = 13 + name.length();
    
	for (auto iter = begin(properties); iter != end(properties); ++iter)
	{
		bytes += iter->getBytes();
	}
	
	bytes += getBytesChildren(is_last);
    return bytes;
}

uint32_t FBXNode::getBytesChildren(bool is_last) 
{
	uint32_t bytes = 0;

	if (children.size() > 0)
	{
		auto lastIter = begin(children) + (children.size() - 1);
		for (auto iter = begin(children); iter != end(children); ++iter)
		{
			bytes += iter->getBytes(lastIter == iter);
		}
		bytes += _BLOCK_SENTINEL_LENGTH;
	}
	else
	{
		if (0 == properties.size())
		{
			if (false == is_last)
				bytes += _BLOCK_SENTINEL_LENGTH;
		}
	}
	
	return bytes;
}

uint32_t FBXNode::getBytesProperties()
{
	uint32_t bytes = 0;

	for (auto iter = begin(properties); iter != end(properties); ++iter)
	{
		bytes += iter->getBytes();
	}
	return bytes;
}


const std::vector<FBXNode> &FBXNode::getChildren()
{
    return children;
}

const std::string &FBXNode::getName()
{
    return name;
}

} // namespace fbx
