# Ethernet Simulation Project (Spring 2025)

## Overview
This project simulates Ethernet communication using UNIX domain sockets in C. It features frame-based data transmission, collision detection, and exponential backoff via a central Communication Bus Process (CBP). The implementation follows the conventions of *UNIX Network Programming* (UNP) using wrapper functions for socket operations (`Socket()`, `Bind()`, `Sendto()`, etc.).

---

## 📁 Project Structure

| File/Dir             | Description |
|----------------------|-------------|
| `cbp.c`              | Communication Bus Process: Receives frames, logs deliveries or collisions |
| `station.c`          | Station process: Reads from input and sends frame parts to CBP |
| `wrapsock.c/.h`      | Wrapper functions for socket system calls with error checking |
| `common.h`           | Shared definitions and frame structure |
| `Makefile`           | Build instructions for `cbp` and `station` binaries |
| `input/`             | Frame input files for each station |
| `logs/`              | Log output for CBP and each station |
| `README.md`          | This file |

---

## ⚙️ How to Compile
```bash
make
```

## How to Run
```bash
mkdir -p logs
./cbp &
./station 1 input/station1.txt &
./station 2 input/station2.txt &
./station 3 input/station3.txt &
./station 4 input/station4.txt &
./station 5 input/station5.txt &
./station 6 input/station6.txt &
```

## Output
- Log files are stored in `logs/` directory.
- Each station logs its frame transmission progress.
- CBP logs frame reception and collision events.

## Clean Project
```bash
make clean
```

---
This project meets the Spring 2025 Project requirements outlined in `sp-25-p.pdf`.
