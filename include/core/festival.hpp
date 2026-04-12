#pragma once

#include "properti.hpp"


class Festival {
private:
    Properti* targetProperti;
    int multiplier;     
    int durasiTersisa;  

public:
    Festival();
    ~Festival();

    std::string getType() const;
};