#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <vector>
#include <iostream>

class LogEntry {
private:
    int turn;
    std::string username;
    std::string jenisAksi;
    std::string detail;

public:
    LogEntry(int turn, std::string username, std::string jenisAksi, std::string detail);
    ~LogEntry() = default;

    int getTurn() const;
    std::string getUsername() const;
    std::string getJenisAksi() const;
    std::string getDetail() const;
};

class Logger {
private:
    std::vector<LogEntry> logs;
    
public:
    Logger();
    ~Logger();
    
    void addLog(int turn, const std::string& username, const std::string& jenisAksi, const std::string& detail);
    void cetakLogPenuh() const;
    void cetakLogTerbaru(int jumlah) const;

    const std::vector<LogEntry>& getLogs() const;
};

#endif