#pragma once
#include <iostream>
#include <string>

class gameException : public std::exception{
protected:
  std::string message;
public:
  gameException(const std::string& msg) : message(msg) {}

  const char* what() const noexcept override {
    return message.c_str();
  }
};

// Exception saat pemain tidak punya cukupp uang untuk aksi tertentu (beli/bangun/tebus)
class UangTidakCukupException : public gameException {
public:
  UangTidakCukupException(const std::string& msg = "Uang pemain tidak cukup!") : gameException(msg) {}
};

// Exception saat syarat pembangunan (monopoli/pemerataan) atau gadai belum terpenuhi
class BukanPemilikException : public gameException {
public:
  BukanPemilikException(const std::string& msg = "Properti ini bukan milikmu atau belum dibeli!") : gameException(msg) {}
};

class StatusPropertiInvalidException : public gameException {
public:
  StatusPropertiInvalidException(const std::string& msg) : gameException(msg) {}
};

class SyaratPembangunanException : public gameException {
public:
  SyaratPembangunanException(const std::string& msg = "Aturan pembangunan tidak terpenuhi!") : gameException(msg) {}
};

class SyaratPemerataanException : public gameException {
public:
  SyaratPemerataanException(const std::string& msg = "Aturan pemerataan bangunan (max selisih 1) dilanggar!") : gameException(msg) {}
};

class TidakAdaBangunanException : public gameException {
public:
  TidakAdaBangunanException(const std::string& msg = "Tidak ada rumah atau hotel yang bisa dijual di peptak ini!") : gameException(msg) {}
};

// Exception saat properti tidak memenuhi syarat status untuk operasi yang diminta (gadai/tebus)
class SyaratPropertiInvalidException : public gameException {
public:
  SyaratPropertiInvalidException(const std::string& msg = "Syarat properti tidak terpenuhi!") : gameException(msg) {}
};

class BidTidakValidException : public gameException {
public:
  BidTidakValidException(const std::string& msg = "Bid harus lebih besar dari tawaran tertinggi saat ini!") : gameException(msg) {}
};

class LelangBelumSelesaiException : public gameException {
public:
  LelangBelumSelesaiException(const std::string& msg = "Lelang masih berlangsung!") : gameException(msg) {}
};

// Exception saat angka dadu tidak valid
class AngkaDaduTidakValidException : public gameException {
public:
  AngkaDaduTidakValidException(const std::string& msg = "Angka yang dimasukkan tidak valid! Angka yang valid (1-6).") : gameException(msg) {}
};