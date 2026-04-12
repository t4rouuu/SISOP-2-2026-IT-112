# Laporan Resmi 
# Praktikum Sistem Operasi

---

## Identitas

| Field | Keterangan |
|-------|-----------|
| **Nama** | A.Algifari Rantiga Isdar |
| **NRP** | 5027251112 |
| **Kelas** | C |

---

# Soal 1 — Kasbon Warga Kampung Durian Runtuh

## Deskripsi Soal

Program ini dibuat untuk membantu Uncle Muthu dalam mengamankan data hutang pelanggan yang tersimpan dalam file `buku_hutang.csv`. Program berjalan secara **sequential process** menggunakan konsep parent dan child process.

Setiap proses dilakukan secara berurutan dengan memanfaatkan `fork()`, `exec()`, dan `waitpid()`, **tanpa menggunakan `system()`**.

---

## Struktur Folder

```
soal_1/
├── buku_hutang.csv
├── kasir_muthu.c
├── rahasia_muthu.zip
└── brankas_kedai/
    ├── buku_hutang.csv
    └── daftar_penunggak.txt
```

---

## Alur Program

Program `kasir_muthu.c` memiliki alur sebagai berikut:

### 1. Membuat Folder Brankas

Parent process membuat child process untuk menjalankan:

```bash
mkdir brankas_kedai
```

Folder ini digunakan sebagai tempat penyimpanan data penting.

### 2. Menyalin File CSV

Child process berikutnya menyalin file:

```bash
cp buku_hutang.csv brankas_kedai/
```

### 3. Menyaring Data "Belum Lunas"

Program mencari pelanggan dengan status `Belum Lunas` menggunakan:

```bash
grep "Belum Lunas" brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt
```

### 4. Mengompres Brankas

Folder `brankas_kedai` dikompres menjadi:

```bash
zip -r rahasia_muthu.zip brankas_kedai
```

---

## Konsep Sequential Process

Setiap langkah dilakukan secara berurutan:

1. Parent membuat child — `fork()`
2. Child menjalankan perintah — `exec()`
3. Parent menunggu child selesai — `waitpid()`

Jika suatu proses gagal, program langsung berhenti dan menampilkan pesan error.

---

## Error Handling

Jika terjadi kegagalan pada salah satu proses:

```
[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.
```

Program tidak akan melanjutkan ke langkah berikutnya.

---

## Output Berhasil

Jika semua proses berhasil:

```
[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.
```

---

## Contoh Isi Output

`daftar_penunggak.txt` berisi baris-baris dari `buku_hutang.csv` dengan status:

```
Belum Lunas
```

---

## Hasil Pengujian

| Fitur | Status |
|-------|--------|
| Pembuatan folder `brankas_kedai` | Berhasil |
| Penyalinan `buku_hutang.csv` | Berhasil |
| Penyaringan data `Belum Lunas` | Berhasil |
| Kompresi folder menjadi `.zip` | Berhasil |
| Error handling saat proses gagal | Berhasil |

---

## Kesimpulan

Program ini berhasil mengimplementasikan **sequential process** menggunakan `fork()`, `exec()`, dan `waitpid()`, mengelola file dan direktori secara otomatis, serta menangani error dengan baik — tanpa menggunakan `system()`.

---

---

# Soal 2 — The World Never Stops, Even When You Feel Tired

## Deskripsi Soal

Program daemon berbasis bahasa C yang berjalan di background untuk mensimulasikan proses yang terus berjalan tanpa henti. Program bertanggung jawab untuk melakukan logging aktivitas secara berkala, menjaga keberadaan file tertentu, serta merespons perubahan pada file tersebut.

---

## Struktur Folder

```
soal_2/
├── contract_daemon.c
├── contract.txt
└── work.log
```

---

## Cara Kerja Program

### 1. Daemon Initialization

Program diubah menjadi daemon menggunakan:

- `fork()` — membuat child process
- `setsid()` — membuat session baru
- `umask(0)` — mengatur permission
- `chdir()` — menentukan working directory
- Menutup file descriptor standar (`stdin`, `stdout`, `stderr`)

### 2. Logging Berkala

Setiap **5 detik**, program menuliskan pesan ke dalam file `work.log` dengan format:

```
still working... [status]
```

Status dipilih secara acak dari:

| Status | Keterangan |
|--------|------------|
| `[awake]` | Daemon aktif penuh |
| `[drifting]` | Daemon berjalan pasif |
| `[numbness]` | Daemon dalam kondisi idle |

### 3. Pembuatan File `contract.txt`

Saat pertama kali dijalankan, program membuat file `contract.txt` dengan isi:

```
A promise to keep going, even when unseen.
created at: <timestamp>
```

> Timestamp diambil dari waktu sistem saat program pertama kali dijalankan.

### 4. Monitoring File dengan `inotify`

Program menggunakan `inotify` untuk memonitor perubahan pada `contract.txt`.

#### a. Jika File Dihapus (`IN_DELETE`)

- Program mendeteksi event penghapusan
- File dibuat kembali dalam waktu **1–2 detik**
- Isi file yang di-restore:

```
A promise to keep going, even when unseen.
restored at: <timestamp>
```

#### b. Jika File Diubah (`IN_MODIFY`)

- Program mendeteksi event modifikasi
- Pesan berikut ditulis ke `work.log`:

```
contract violated.
```

- File di-restore ke format awal (menggunakan `restored at`)

### 5. Signal Handling

Program menangani sinyal terminasi `SIGTERM` dan `SIGINT`.

Saat daemon dihentikan, program menuliskan pesan berikut ke `work.log` sebelum berhenti:

```
We really weren't meant to be together
```

---

## Cara Menjalankan Program

### Compile Program

```bash
gcc contract_daemon.c -o contract_daemon
```

### Jalankan Daemon

```bash
./contract_daemon
```

### Hentikan Daemon

```bash
ps aux | grep contract_daemon
kill <PID>
```

---

## Hasil Pengujian

| Fitur | Status |
|-------|--------|
| Berjalan sebagai daemon | Berhasil |
| Logging setiap 5 detik | Berhasil |
| Pembuatan `contract.txt` otomatis | Berhasil |
| Restore file saat dihapus | Berhasil |
| Deteksi dan penanganan perubahan file | Berhasil |
| Pesan akhir saat daemon dihentikan | Berhasil |

---
## Kesimpulan

Program berhasil mengimplementasikan konsep **daemon process**, **file monitoring** menggunakan `inotify`, serta **signal handling** dengan baik. Semua requirement pada soal telah terpenuhi sesuai dengan spesifikasi yang diberikan

---
#  Laporan Revisi Program Daemon Monitoring File

## Deskripsi Program

Program ini merupakan daemon berbasis C yang berfungsi untuk:

- Memantau file `contract.txt`
- Menjaga isi file agar tidak diubah atau dihapus
- Mengembalikan (restore) isi file jika terjadi perubahan
- Mencatat aktivitas ke dalam file log `work.log`

Program berjalan di background menggunakan mekanisme daemon dan memanfaatkan **inotify** untuk monitoring file system.

---

## Permasalahan Awal

Pada implementasi sebelumnya, terdapat bug:

> Ketika isi file `contract.txt` diedit (misalnya mengubah kata *"unseen"*), file tidak selalu ter-restore ke kondisi semula.

---

## Penyebab

Hal ini terjadi karena program hanya menangani event `IN_MODIFY`.

Banyak text editor (nano, vim, VSCode, dll.) **tidak langsung menulis ke file asli**. Editor biasanya:

1. Menulis ke file sementara
2. Mengganti file asli (rename/overwrite)

Akibatnya, event `IN_MODIFY` saja tidak cukup untuk mendeteksi perubahan final.

---

## Solusi yang Diterapkan

### 1. Menambahkan Event `IN_CLOSE_WRITE`

Event ini dipicu ketika file selesai ditulis (*write* selesai).

```c
int wd = inotify_add_watch(fd, ".", IN_DELETE | IN_MODIFY | IN_CLOSE_WRITE);
```

### 2. Menangani Banyak Event Sekaligus

Fungsi `read()` bisa mengembalikan beberapa event sekaligus, sehingga perlu loop parsing:

```c
int i = 0;
while (i < length) {
    struct inotify_event *event = (struct inotify_event *)&buffer[i];

    // proses event

    i += sizeof(struct inotify_event) + event->len;
}
```

### 3. Menggabungkan Deteksi Event Perubahan

Perubahan file sekarang dideteksi dengan:

```c
if (event->mask & (IN_MODIFY | IN_CLOSE_WRITE)) {
    write_log("contract violated.");
    sleep(1);
    create_contract(1);
}
```

### 4. Menambahkan Delay (`sleep`)

Digunakan untuk memastikan proses penulisan file oleh editor sudah selesai:

```c
sleep(1);
```

---
# Soal 3 — One Letter for Destiny

## Deskripsi Soal

Program `angel.c` adalah program berbasis **daemon** di Linux yang secara otomatis berjalan di background. Setelah dijalankan sebagai daemon, nama proses yang tampil di sistem (via `ps aux`) harus berubah menjadi **`maya`**.

---

## Yang Harus Dibuat

### Program: `angel.c`

Program ini memiliki beberapa fitur dan command yang dapat dijalankan melalui argumen.

---

## Fitur & Command

### 1. `-daemon` — Jalankan sebagai Daemon
```bash
./angel -daemon
```
- Menjalankan program sebagai **background daemon**.
- Nama proses yang tampil di `ps aux` harus berubah menjadi **`maya`**.
- Setelah berjalan, dua fitur utama berjalan secara otomatis dan terus-menerus selama daemon hidup.

---

### 2. Fitur `secret` — Generate LoveLetter *(otomatis di dalam daemon)*

- Membuat file **`LoveLetter.txt`** di direktori saat ini.
- Setiap **10 detik**, isi file diganti dengan **satu kalimat acak** dari daftar berikut:
  1. `aku akan fokus pada diriku sendiri`
  2. `aku mencintaimu dari sekarang hingga selamanya`
  3. `aku akan menjauh darimu, hingga takdir mempertemukan kita di versi kita yang terbaik.`
  4. `kalau aku dilahirkan kembali, aku tetap akan terus menyayangimu`

---

### 3. Fitur `surprise` — Enkripsi LoveLetter *(otomatis di dalam daemon)*

- Berjalan **otomatis** setelah fitur `secret` memperbarui file.
- Mengenkripsi isi `LoveLetter.txt` menggunakan metode **Base64**.
- Fitur ini **tidak** dipanggil secara manual; berjalan sendiri di dalam daemon.

---

### 4. `-decrypt` — Dekripsi LoveLetter *(manual)*
```bash
./angel -decrypt
```
- Mengembalikan isi `LoveLetter.txt` yang sudah dienkripsi ke **bentuk aslinya** (plaintext).
- Hanya berjalan jika **dipanggil eksplisit** melalui argumen.
- Terdapat **error handling** jika file `LoveLetter.txt` tidak ditemukan.

---

### 5. `-kill` — Hentikan Daemon *(manual)*
```bash
./angel -kill
```
- Menghentikan proses daemon yang sedang berjalan.
- Hanya berjalan jika **dipanggil eksplisit** melalui argumen.
- Terdapat **error handling** jika daemon ternyata belum berjalan.

---

## Logging

Semua aktivitas program dicatat ke dalam file **`ethereal.log`**.

**Proses yang dicatat:** `secret`, `surprise`, `decrypt`, `kill`

**Format log:**
```
[dd:mm:yyyy]-[hh:mm:ss]_nama-proses_STATUS
```

**Status yang digunakan:**

| Status | Keterangan |
|--------|------------|
| `RUNNING` | Saat proses mulai dijalankan |
| `SUCCESS` | Saat proses berhasil diselesaikan |
| `ERROR` | Saat terjadi kegagalan dalam proses |

**Contoh isi log:**
```
[12:04:2025]-[10:30:00]_secret_RUNNING
[12:04:2025]-[10:30:00]_secret_SUCCESS
[12:04:2025]-[10:30:00]_surprise_RUNNING
[12:04:2025]-[10:30:00]_surprise_SUCCESS
[12:04:2025]-[10:30:05]_decrypt_RUNNING
[12:04:2025]-[10:30:05]_decrypt_ERROR
```

---

## Ringkasan Alur Program

```
./angel -daemon
    │
    ├─► Daemonize → nama proses berubah menjadi "maya"
    │
    ├─► [Loop setiap 10 detik]
    │       ├─► secret  : tulis kalimat acak ke LoveLetter.txt
    │       └─► surprise: enkripsi LoveLetter.txt dengan Base64
    │
    └─► [Berjalan terus sampai di-kill]

./angel -decrypt  → decode Base64 → tampilkan isi asli LoveLetter.txt
./angel -kill     → kirim sinyal stop ke daemon
```

---

## Demonstrasi

### soal3_1 — Kondisi Awal: Hanya ada `angel.c`

![soal3_1](soal3_1.png)

Kondisi awal direktori hanya berisi satu file yaitu `angel.c`. File `LoveLetter.txt` dan `ethereal.log` belum ada karena daemon belum dijalankan.

---

### soal3_2 — Kompilasi, Help Menu, dan Menjalankan Daemon

![soal3_2](soal3_2.png)

Proses kompilasi dengan `gcc angel.c -o angel`, lalu menjalankan `./angel` tanpa argumen yang menampilkan **help menu** berisi daftar command yang tersedia. Setelah itu daemon dijalankan dengan `./angel -daemon` dan dicek via `ps aux` — terlihat proses dengan nama **`maya`** berhasil muncul (PID 4187).

---

### soal3_3 — Isi `LoveLetter.txt` Berupa Base64 (Terenkripsi)

![soal3_3](soal3_3.png)

Saat daemon berjalan, perintah `cat LoveLetter.txt` menampilkan isi file dalam bentuk **string Base64** (terenkripsi). Ini adalah hasil kerja fitur `surprise` yang otomatis mengenkripsi setiap kalimat yang ditulis oleh fitur `secret`.

---

### soal3_4 — Decrypt: Menghasilkan Kalimat Pertama

![soal3_4](soal3_4.png)

Menjalankan `./angel -decrypt` berhasil mendekripsi `LoveLetter.txt`. Output menampilkan pesan **"Berhasil decrypt"** dan isi file kembali ke plaintext: **`aku akan fokus pada diriku sendiri`**.

---

### soal3_5 — Decrypt: Menghasilkan Kalimat Lain (Acak)

![soal3_5](soal3_5.png)

Demonstrasi decrypt kedua menunjukkan kalimat berbeda: **`aku akan menjauh darimu, hingga takdir mempertemukan kita di versi kita yang terbaik.`** — membuktikan bahwa pemilihan kalimat bersifat **acak** setiap 10 detik.

---

### soal3_6 — Decrypt: Menghasilkan Kalimat Lain (Acak)

![soal3_6](soal3_6.png)

Demonstrasi decrypt ketiga menghasilkan kalimat berbeda lagi: **`aku mencintaimu dari sekarang hingga selamanya`** — kembali membuktikan keacakan pemilihan kalimat oleh fitur `secret`.

---

### soal3_6b — Decrypt: Menghasilkan Kalimat Keempat (Acak)

![soal3_6b](soal3_6b.png)

Demonstrasi decrypt lanjutan menghasilkan kalimat keempat: **`kalau aku dilahirkan kembali, aku tetap akan terus menyayangimu`** — melengkapi bukti bahwa keempat kalimat dari daftar dapat muncul secara acak.

---

### soal3_7 — Menghentikan Daemon dan Isi `ethereal.log`

![soal3_7](soal3_7.png)

Perintah `./angel -kill` berhasil menghentikan daemon dengan output **"Daemon dihentikan"**. Pengecekan `cat ethereal.log | tail -2` menampilkan dua baris terakhir log:
```
[12:04:2026]-[06:54:14]_kill_RUNNING
[12:04:2026]-[06:54:14]_kill_SUCCESS
```
Membuktikan bahwa seluruh aktivitas program — termasuk proses kill — berhasil dicatat ke `ethereal.log`.

---

## File yang Dihasilkan

| File | Keterangan |
|------|------------|
| `angel.c` | Source code utama program |
| `LoveLetter.txt` | File yang berisi kalimat terenkripsi (Base64) |
| `ethereal.log` | File log aktivitas program |
