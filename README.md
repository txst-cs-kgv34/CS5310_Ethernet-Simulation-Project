# 🧪 Ethernet Simulation Project (Spring 2025)

## 🎯 Goal
Simulate Ethernet-style communication between multiple stations over a shared communication bus (CBP), featuring frame-based delivery, collision detection, and Binary Exponential Backoff (BEBO) logic using UNIX domain sockets in C.

---

## 📦 Project Structure

| File/Dir             | Description |
|----------------------|-------------|
| `cbp.c`              | 💬 Central Communication Bus Process: receives frames, logs deliveries and collisions |
| `station.c`          | 📡 Station process: reads input and transmits frames in 2 parts |
| `wrapsock.c/.h`      | 🛠️ Safe wrapper functions for socket operations (`Socket()`, `Bind()` etc.) |
| `common.h`           | 📚 Shared constants and `FramePart` structure |
| `Makefile`           | 🧱 Build instructions for all executables |
| `input/`             | 📥 Text input files listing frames to send |
| `logs/`              | 📄 Generated log files for each station and the CBP |
| `README.md`          | 📝 This file |

---

## 📐 Sample Input File Format
```
Frame 1, To Station 3
Frame 2, To Station 2
Frame 3, To Station 6
```

Each station reads from its corresponding `input/stationprocess<ID>.txt` file.

---

## ⚙️ Build Instructions
```bash
make
```

---

## 🚀 How to Run

### 1. Create the logs directory
```bash
mkdir -p logs
```

### 2. Start the Communication Bus Process
```bash
./cbp &
```

### 3. Launch the station processes (example for 8 stations)
```bash
./station 1 input/stationprocess1.txt &
./station 2 input/stationprocess2.txt &
./station 3 input/stationprocess3.txt &
./station 4 input/stationprocess4.txt &
./station 5 input/stationprocess5.txt &
./station 6 input/stationprocess6.txt &
./station 7 input/stationprocess7.txt &
./station 8 input/stationprocess8.txt &
```

---

## 📊 Output

- Log files will be stored in the `logs/` directory:
  - `cbp.log` records **received parts**, **transfers**, and **collisions**
  - `stationprocess<ID>.log` logs **sent frames** and **BEBO wait times**
- You'll observe realistic collisions and retransmissions with BEBO.

---

## 🧼 Cleanup
```bash
make clean
```

---

## ✅ Compliance
This project fully satisfies the Spring 2025 Ethernet Simulation requirements outlined in `sp-25-p.pdf`.

---
