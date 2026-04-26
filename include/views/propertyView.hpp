#ifndef PROPERTYVIEW_HPP
#define PROPERTYVIEW_HPP

class Properti;
class User;

class PropertyView {
public:
    static void cetakAkta(Properti* properti);
    static void cetakProperti(const User& user);
};

#endif
