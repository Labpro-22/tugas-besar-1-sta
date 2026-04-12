#include <iostream>

class gameException : public std::exception{

};

// Exception saat pemain tidak punya cukupp uang untuk aksi tertentu (beli/bangun/tebus)
class UangTidakCukupException : public gameException {};

// Exception saat syarat pembangunan (monopoli/pemerataan) atau gadai belum terpenuhi
class SyaratProppertiException : public gameException {};