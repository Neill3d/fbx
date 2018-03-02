#include "fbxnode.h"

#include "fbxutil.h"
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::uint32_t;
using std::uint8_t;

namespace fbx {



FBXNode::FBXNode()
{
}

FBXNode::FBXNode(const char *_name)
	:name(_name) 
{}

FBXNode::FBXNode(const char *_name, const FBXProperty &p)
: name(_name)
{
	properties.push_back(p);
}

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

	if (true == recursive)
	{
		for (auto iter = begin(children); iter != end(children); ++iter)
		{
			iter->removeProperties(recursive);
		}
	}
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
void FBXNode::addProperty(uint32_t v) { addProperty(FBXProperty(v)); }
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
	n.addProperty(v); 
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

const std::vector<FBXNode> &FBXNode::getChildren() const
{
    return children;
}

const std::vector<FBXProperty> &FBXNode::getProperties() const
{
	return properties;
}

const std::string &FBXNode::getName()
{
    return name;
}

void FBXNode::addP70int(const char *name, int32_t value)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("int");
	n.addProperty("Integer");
	n.addProperty("");
	n.addProperty((int32_t)value);
	addChild(n);
}
void FBXNode::addP70bool(const char *name, bool value)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("bool");
	n.addProperty("");
	n.addProperty("");
	n.addProperty((int32_t)value);
	addChild(n);
}
void FBXNode::addP70double(const char *name, double value)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("double");
	n.addProperty("Number");
	n.addProperty("");
	n.addProperty((double)value);
	addChild(n);
}
void FBXNode::addP70numberA(const char *name, double value)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("Number");
	n.addProperty("");
	n.addProperty("A");
	n.addProperty((double)value);
	addChild(n);
}
void FBXNode::addP70color(const char *name, double r, double g, double b)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("ColorRGB");
	n.addProperty("Color");
	n.addProperty("");
	n.addProperty((double)r);
	n.addProperty((double)g);
	n.addProperty((double)b);
	addChild(n);
}
void FBXNode::addP70colorA(const char *name, double r, double g, double b)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("Color");
	n.addProperty("");
	n.addProperty("A");
	n.addProperty((double)r);
	n.addProperty((double)g);
	n.addProperty((double)b);
	addChild(n);
}
void FBXNode::addP70vector(const char *name, double x, double y, double z)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("Vector3D");
	n.addProperty("Vector");
	n.addProperty("");
	n.addProperty((double)x);
	n.addProperty((double)y);
	n.addProperty((double)z);
	addChild(n);
}
void FBXNode::addP70vectorA(const char *name, double x, double y, double z)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("Vector");
	n.addProperty("");
	n.addProperty("A");
	n.addProperty((double)x);
	n.addProperty((double)y);
	n.addProperty((double)z);
	addChild(n);
}
void FBXNode::addP70enum(const char *name, int32_t value)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("enum");
	n.addProperty("");
	n.addProperty("");
	n.addProperty((int32_t)value);
	addChild(n);
}
void FBXNode::addP70time(const char *name, int64_t value)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("KTime");
	n.addProperty("Time");
	n.addProperty("");
	n.addProperty((int64_t)value);
	addChild(n);
}
void FBXNode::addP70string(const char *name, const std::string &value)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty("KString");
	n.addProperty("");
	n.addProperty("");
	n.addProperty(value);
	addChild(n);
}
void FBXNode::addP70Compound(const char *name, const char *type, const char *str1, const char *str2)
{
	FBXNode n("P");
	n.addProperty(name);
	n.addProperty(type);
	n.addProperty(str1);
	n.addProperty(str2);
	addChild(n);
}

} // namespace fbx
