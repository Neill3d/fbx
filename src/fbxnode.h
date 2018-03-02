#ifndef FBXNODE_H
#define FBXNODE_H

#include "fbxproperty.h"
#include <cstdint>

namespace fbx {

class FBXNode
{
public:
    FBXNode();
    FBXNode(const char *name);
	FBXNode(const char *name, const FBXProperty &p);

    std::uint64_t read(std::ifstream &input, uint64_t start_offset, uint16_t version);
    
    void print(std::string prefix="");
    bool isNull();

    void addProperty(int16_t);
    void addProperty(bool);
    void addProperty(int32_t);
	void addProperty(uint32_t);
    void addProperty(float);
    void addProperty(double);
    void addProperty(int64_t);
    void addProperty(const std::vector<bool>&);
    void addProperty(const std::vector<int32_t>&);
    void addProperty(const std::vector<float>&);
    void addProperty(const std::vector<double>&);
    void addProperty(const std::vector<int64_t>&);
    void addProperty(const std::vector<uint8_t>&, uint8_t type);
    void addProperty(const std::string&);
    void addProperty(const char*);
    void addProperty(FBXProperty&);

    void addPropertyNode(const char *name, int16_t);
	void addPropertyNode(const char *name, bool);
	void addPropertyNode(const char *name, int32_t);
	void addPropertyNode(const char *name, float);
	void addPropertyNode(const char *name, double);
	void addPropertyNode(const char *name, int64_t);
	void addPropertyNode(const char *name, const std::vector<bool>&);
	void addPropertyNode(const char *name, const std::vector<int32_t>&);
	void addPropertyNode(const char *name, const std::vector<float>&);
	void addPropertyNode(const char *name, const std::vector<double>&);
	void addPropertyNode(const char *name, const std::vector<int64_t>&);
	void addPropertyNode(const char *name, const std::vector<uint8_t>&, uint8_t type);
	void addPropertyNode(const char *name, const std::string&);
	void addPropertyNode(const char *name, const char*);

	void addP70int(const char *name, int32_t value);
	void addP70bool(const char *name, bool value);
	void addP70double(const char *name, double value);
	void addP70numberA(const char *name, double value);
	void addP70color(const char *name, double r, double g, double b);
	void addP70colorA(const char *name, double r, double g, double b);
	void addP70vector(const char *name, double r, double g, double b);
	void addP70vectorA(const char *name, double r, double g, double b);
	void addP70enum(const char *name, int32_t value);
	void addP70time(const char *name, int64_t value);
	void addP70string(const char *name, const std::string &value);
	void addP70Compound(const char *name, const char *type, const char *str1, const char *str2);

    void addChild(FBXNode &child);

	const size_t getChildrenCount() const {
		return children.size();
	}
	const size_t getPropertiesCount() const {
		return properties.size();
	}
    const std::vector<FBXNode> &getChildren() const;
	const std::vector<FBXProperty> &getProperties() const;
    const std::string &getName();
	const char *getNamePtr() const {
		return name.c_str();
	}

	void removeProperties(bool recursive);
private:
    std::vector<FBXNode> children;
    std::vector<FBXProperty> properties;
    std::string name;
};

} // namespace fbx

#endif // FBXNODE_H
