#include "io.hpp"

class dynamicValue{
public:
    int index;
    std::string jenis;
    std::string kode;
};

class mapDynamic {
private:
    std::vector<dynamicValue> map;
public:
    mapDynamic() {}
    ~mapDynamic() {}

    std::vector<dynamicValue> getMap() const {
        return map;
    }   

    void parseFile(const std::string& filename, const configBase& config);
};