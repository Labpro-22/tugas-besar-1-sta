#include "../../include/core/logger.hpp"

LogEntry::LogEntry(int turn, std::string username, std::string jenisAksi, std::string detail)
    : turn(turn), username(username), jenisAksi(jenisAksi), detail(detail) {}

int LogEntry::getTurn() const { return turn; }
std::string LogEntry::getUsername() const { return username; }
std::string LogEntry::getJenisAksi() const { return jenisAksi; }
std::string LogEntry::getDetail() const { return detail; }

Logger::Logger() {}
Logger::~Logger() {}

void Logger::addLog(int turn, const std::string& username, const std::string& jenisAksi, const std::string& detail) {
    LogEntry entry(turn, username, jenisAksi, detail);
    logs.push_back(entry);
}

void Logger::cetakLogPenuh() const {
    std::cout << "\n=== Log Transaksi Penuh ===\n";
    for (size_t i = 0; i < logs.size(); ++i) {
        const auto& log = logs[i];
        std::cout << "[Turn " << log.getTurn() << "] " << log.getUsername() 
                  << " | " << log.getJenisAksi() << " | " << log.getDetail() << "\n";
    }
}

void Logger::cetakLogTerbaru(int jumlah) const {
    std::cout << "\n=== Log Transaksi (" << jumlah << " Terakhir) ===\n";
    
    size_t start = 0;
    if (logs.size() > static_cast<size_t>(jumlah)) {
        start = logs.size() - jumlah;
    }
    
    for (size_t i = start; i < logs.size(); ++i) {
        const auto& log = logs[i];
        std::cout << "[Turn " << log.getTurn() << "] " << log.getUsername() 
                  << " | " << log.getJenisAksi() << " | " << log.getDetail() << "\n";
    }
}

const std::vector<LogEntry>& Logger::getLogs() const {
    return logs;
}