#include "../../include/core/board.hpp"


Board::Board() {
    // Default: 40
    this->size = 40;
    // Isi papan dengan nullptr sementara agar tidak error saat di-loop
    for (int i = 0; i < size; ++i) {
        kotak.push_back(nullptr); 
    }
}

Board::Board(int size) : size(size) {
    // Isi papan dengan nullptr sementara agar tidak error saat di-loop
    for (int i = 0; i < size; ++i) {
        kotak.push_back(nullptr); 
    }
}

Board::~Board() {
    for (auto p : kotak) {
        delete p;
    }
    kotak.clear();
}

void Board::buildBoard(const std::string& configFilename) {
    std::ifstream file(configFilename);
    
    // Proteksi jika file tidak ditemukan
    if (!file.is_open()) {
        std::cerr << "ERROR: Gagal membuka file konfigurasi papan (" << configFilename << ")!\n";
        std::cerr << "Pastikan file .txt ada di folder yang sama dengan file .exe\n";
        return;
    }

    std::string line;
    // Baca file baris per baris
    while (std::getline(file, line)) {
        // Skip baris kosong atau baris komentar yang diawali '#'
        if (line.empty() || line[0] == '#') continue; 

        std::stringstream ss(line);
        int index;
        std::string type;
        
        // Ekstrak Indeks dan Tipe Petak
        ss >> index >> type;

        // Validasi agar index tidak keluar batas vector (0-39)
        if (index < 0 || index >= size) continue;

        // Jika sebelumnya ada isinya (dummy), hapus dulu agar tidak memory leak
        if (kotak[index] != nullptr) {
            delete kotak[index];
            kotak[index] = nullptr;
        }

        // Buat objek berdasarkan tipe di txt
        if (type == "PBM") {
            float flat;
            ss >> flat; // Ekstrak nilai flat dari txt
            kotak[index] = new PetakPBM(flat);
        } 
        else if (type == "PPH") {
            float flat, percent;
            ss >> flat >> percent; // Ekstrak flat dan persentase
            kotak[index] = new PetakPPH(flat, percent);
        } 
        else if (type == "FESTIVAL") {
            kotak[index] = new PetakFestival();
        } 
        else if (type == "LAHAN") {
            std::string warna;
            ss >> warna; // Ekstrak warna lahan
            kotak[index] = new PetakLahan(warna);
        } 
        else if (type == "STASIUN") {
            kotak[index] = new PetakStasiun();
        } 
        else if (type == "UTILITAS") {
            kotak[index] = new PetakUtilitas();
        } 
        else if (type == "SPESIAL") {
            kotak[index] = new PetakSpesial();
        } 
        else if (type == "KARTU") {
            kotak[index] = new PetakKartu();
        }

        if (kotak[index] != nullptr) {
            kotak[index]->setIndex(index);
        }
    }

    file.close();
}

Petak* Board::getPetakAt(int index) const {
    if (index >= 0 && index < size) {
        return kotak[index];
    }
    return nullptr;
}

int Board::getSize() const { 
    return size;
}
int Board::getGoIndex() const { 
    for (auto it = kotak.begin(); it != kotak.end(); ++it) {
        if (*it != nullptr && (*it)->getName() == "go") {
            return static_cast<int>(it - kotak.begin());
        }
    }
    return -1;
}
int Board::getPenjaraIndex() const {
    // [!] HardCode
    for (auto it = kotak.begin(); it != kotak.end(); ++it) {
        if (*it != nullptr && (*it)->getName() == "penjara") {
            return static_cast<int>(it - kotak.begin());
        }
    }
    return -1;
}