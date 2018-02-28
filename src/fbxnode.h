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

    std::uint64_t read(std::ifstream &input, uint64_t start_offset, uint16_t version);
    std::uint32_t write(std::ofstream &output, uint32_t start_offset, bool is_last);
	std::uint32_t write_children(std::ofstream &output, uint32_t start_offset, bool is_last);
    void print(std::string prefix="");
    bool isNull();

    void addProperty(int16_t);
    void addProperty(bool);
    void addProperty(int32_t);
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

    void addChild(FBXNode &child);
    uint32_t getBytes(bool is_last);
	uint32_t getBytesChildren(bool is_last);
	uint32_t getBytesProperties();

    const std::vector<FBXNode> &getChildren();
    const std::string &getName();

	void removeProperties(bool recursive);
private:
    std::vector<FBXNode> children;
    std::vector<FBXProperty> properties;
    std::string name;
};

} // namespace fbx

#endif // FBXNODE_H
