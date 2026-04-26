#include "utils/mapDynamic.hpp"

void mapDynamic::parseFile(const std::string& filename, const configBase& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Gagal membuka file: " + filename);
    } 

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        dynamicValue value;
        if (!(iss >> value.index >> value.jenis >> value.kode)) {
            continue; // Lewati baris yang tidak sesuai format
        }
        if (value.jenis == "STREET" || value.jenis == "RAILROAD" || value.jenis == "UTILITY") {
            // Validasi kode untuk properti
            auto it = std::find_if(config.getPropertyConfig().begin(), config.getPropertyConfig().end(),
                        [&](const auto& prop) {
                            return prop.getKode() == value.kode;
            });
            if (it == config.getPropertyConfig().end()) {
                throw std::runtime_error("Kode tidak ditemukan di config: " + value.kode);
            }
        } 
        map.push_back(value);
    }

    file.close();
}

