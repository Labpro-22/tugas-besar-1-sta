#ifndef BANGUN_COMMAND_HELPER_HPP
#define BANGUN_COMMAND_HELPER_HPP

#include "core/petak.hpp"
#include <string>
#include <vector>

class BuildGroup {
private:
    std::string warna;
    std::vector<Street*> streets;
    std::vector<Street*> eligible;

public:
    BuildGroup(std::string warna, std::vector<Street*> streets, std::vector<Street*> eligible);

    const std::string& getWarna() const;
    const std::vector<Street*>& getStreets() const;
    const std::vector<Street*>& getEligible() const;
};

class BangunCommandHelper {
public:
    static std::string formatUangCommand(int uang);
    static std::string displayWarna(const std::string& warna);
    static std::string statusBangunan(Street* street);
    static std::vector<Street*> getStreetGroup(const std::vector<PetakProperti*>& petakGroup);
    static void printStreetBuildStatus(Street* street);
};

#endif
