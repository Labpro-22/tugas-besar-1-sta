[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/b842RB8g)

# Tugas Besar 1 IF2010 Pemrograman Berorientasi Objek

# Nimonspoli - Monopoly Game

*Nimonspoli* adalah permainan papan berbasis teks (*Command Line Interface* / CLI) yang diadaptasi dari permainan klasik Monopoly. Game ini dikembangkan menggunakan bahasa pemrograman *C++* dengan standar *C++17*.

Proyek ini menitikberatkan pada penerapan konsep *Object-Oriented Programming* (OOP), seperti enkapsulasi, inheritance, polymorphism, composition, exception handling, dan pemisahan tanggung jawab antar kelas.

---

## Fitur Utama

### Sistem Gameplay

- *Centralized Game Controller* untuk mengatur alur permainan, giliran pemain, status papan, kartu, dadu, dan kondisi akhir permainan.
- Urutan pemain diacak saat game baru dimulai.
- Mendukung 2 sampai 4 pemain.
- Sistem giliran berbasis command.
- Dadu acak dan dadu manual untuk kebutuhan testing.
- Kondisi game berakhir berdasarkan batas turn atau jumlah pemain aktif.

### Jenis Petak

- Lahan (*Street*)
- Stasiun (*RailRoad*)
- Utilitas
- Pajak:
  - Pajak Penghasilan (PPH)
  - Pajak Barang Mewah (PBM)
- Festival
- Petak kartu:
  - Kesempatan
  - Dana Umum
- Petak spesial:
  - GO / Petak Mulai
  - Penjara
  - Bebas Parkir
  - Pergi ke Penjara

### Sistem Properti

- Pembelian properti
- Pembayaran sewa
- Lelang properti
- Gadai properti
- Tebus properti
- Pembangunan rumah
- Upgrade hotel
- Color group dan aturan monopoli
- Efek festival terhadap sewa properti

### Kartu Aksi & Kemampuan

- Kartu Kesempatan
- Kartu Dana Umum
- Kartu kemampuan spesial:
  - Move Card
  - Discount Card
  - Shield Card
  - Teleport Card
  - Lasso Card
  - Demolition Card

### Save & Load

- Game dapat disimpan ke file menggunakan command `SIMPAN <nama_file>`.
- Game dapat dimuat dari file save saat program pertama kali dijalankan dengan memilih opsi `MUAT`.
- Contoh file save tersedia di folder `data/`.

### Tampilan CLI

- Tampilan papan berbasis teks.
- Tampilan akta properti.
- Tampilan daftar properti pemain.
- Log aksi permainan.

---

## Prasyarat

Pastikan sistem sudah memiliki:

- Compiler C++ dengan dukungan *C++17*
- `make`
- Terminal / command prompt

---

## Instalasi & Kompilasi

Clone repository:

```bash
git clone https://github.com/Labpro-22/tugas-besar-1-sta.git
cd tugas-besar-1-sta
```

Kompilasi program:

```bash
make
```

Executable akan dibuat di:

```bash
bin/game
```

Untuk membersihkan hasil build:

```bash
make clean
```

Untuk rebuild dari awal:

```bash
make rebuild
```

---

## Menjalankan Game

Program dapat dijalankan dengan:

```bash
./bin/game
```

Atau langsung melalui Makefile:

```bash
make run
```

Saat program meminta path konfigurasi, masukkan:

```text
config
```

Untuk memulai game baru, jawab selain `MUAT` saat ditanya opsi load/save.

Untuk memuat contoh save:

```text
Masukkan Path: config
Apakah anda mau load/save (ketik "MUAT" jika mau): MUAT
Masukkan path file save: data/contoh_save.txt
```

---

## Command Permainan

| Command | Fungsi |
| --- | --- |
| `CETAK_PAPAN` | Menampilkan papan permainan |
| `LEMPAR_DADU` | Melempar dadu secara acak |
| `ATUR_DADU <X> <Y>` | Mengatur nilai dadu secara manual |
| `CETAK_AKTA` | Menampilkan akta properti berdasarkan kode |
| `CETAK_PROPERTI <nama_user>` | Menampilkan properti milik pemain |
| `GADAI` | Menggadaikan properti yang dimiliki |
| `TEBUS` | Menebus properti yang sedang digadaikan |
| `BANGUN` | Membangun rumah atau upgrade hotel |
| `GUNAKAN_KEMAMPUAN` | Menggunakan kartu kemampuan spesial |
| `SIMPAN <nama_file>` | Menyimpan state permainan |
| `CETAK_LOG` | Menampilkan seluruh log permainan |
| `CETAK_LOG <jumlah>` | Menampilkan log terbaru sebanyak jumlah tertentu |
| `CEK` | Mengecek jumlah uang pemain dengan biaya tertentu |
| `HELP` | Menampilkan daftar command |
| `SELESAI` | Mengakhiri giliran |
| `KELUAR` | Keluar dari program |

---

## Struktur Folder

```text
.
├── config
│   ├── aksi.txt
│   ├── misc.txt
│   ├── property.txt
│   ├── railroad.txt
│   ├── special.txt
│   ├── tax.txt
│   └── utility.txt
├── data
│   ├── contoh_dinamis.txt
│   ├── contoh_save.txt
│   └── map.txt
├── docs
├── include
│   ├── core
│   ├── utils
│   └── views
├── src
│   ├── core
│   ├── utils
│   ├── views
│   └── main.cpp
├── makefile
└── README.md
```

### Penjelasan Singkat

- `config/` berisi konfigurasi dasar game.
- `data/` berisi contoh save file dan dynamic map.
- `docs/` berisi dokumen pendukung tugas besar.
- `include/` berisi file header.
- `src/` berisi implementasi program.
- `src/core/` berisi logika utama game.
- `src/utils/` berisi helper, parser config, builder, command, dan dadu.
- `src/views/` berisi tampilan CLI untuk papan dan properti.

---

## Dynamic Map

Game mendukung penggunaan dynamic map saat membuat game baru.

Jika memilih dynamic map, file map diletakkan di folder `data/`, lalu masukkan nama file saat diminta oleh program. Contoh file tersedia di:

```text
data/contoh_dinamis.txt
data/map.txt
```

---

## Kelompok SemuaTubesMilikAllah

1. 13524120 / Jonathan Alveraldo Bangun
2. 13524124 / Zahran Alvan Putra Winarko
3. 13524126 / Ramadhian Nabil Firdaus Gumay
4. 13524136 / Neswa Eka Anggara
5. 13524146 / Leonardus Brain Fatolosja
