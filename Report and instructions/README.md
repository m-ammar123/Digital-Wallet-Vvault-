# Digital Vault (vVault) — C++ Secure File Management Server

**Course:** CS112L — Object-Oriented Programming Lab  
**Institute:** Ghulam Ishaq Khan Institute of Engineering Sciences and Technology, Topi  
**Instructor:** Sajid Ali  
**Students:** Muhammad Ammar (2025474) · Misbah Alam (2025411)

---

## Overview

Digital Vault is a self-hosted, server-based secure file management system written in **C++17**. It exposes a RESTful HTTP API that lets authenticated users upload, download, list, and delete files. All stored files are **AES-256-CBC encrypted** (via OpenSSL) and optionally **ZLIB-compressed** before being written to disk.

The server is built with:
- **Crow** — C++ micro HTTP framework (header-only, included in `include/`)
- **Boost.Asio** — asynchronous networking (`standalone` mode)
- **OpenSSL** — AES-256 encryption & SHA-256 password hashing
- **ZLIB** — file compression
- **nlohmann/json** — JSON parsing (header-only, included in `include/`)

---

## Prerequisites

| Tool / Library | Minimum Version | Notes |
|----------------|-----------------|-------|
| C++ Compiler   | GCC 10 / Clang 12 / MSVC 2019 | Must support C++17 (`-std=c++17`) |
| CMake          | 4.0+            | Build system |
| Boost          | 1.75+           | Asio headers only (no compiled libs needed) |
| OpenSSL        | 3.0+            | `libssl` + `libcrypto` |
| ZLIB           | 1.2+            | `libz` |
| Ninja (optional) | any           | Faster builds than make |

---

## Building on Windows (MSYS2 / UCRT64) — Recommended

This is the environment used during development.

### Step 1 — Install MSYS2

Download and install MSYS2 from https://www.msys2.org/. Open the **UCRT64** terminal.

### Step 2 — Install dependencies

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja \
          mingw-w64-ucrt-x86_64-boost \
          mingw-w64-ucrt-x86_64-openssl \
          mingw-w64-ucrt-x86_64-zlib
```

### Step 3 — Clone / extract the project

```bash
# Extract the submitted zip, then navigate into the project folder
cd path/to/vVault
```

### Step 4 — Configure with CMake

```bash
mkdir build && cd build
cmake .. -G Ninja \
  -DCMAKE_BUILD_TYPE=Release
```

### Step 5 — Build

```bash
ninja
# or: cmake --build . --config Release
```

The executable `vvault_server.exe` will be created inside `build/`.

---

## Building on Linux / macOS

### Step 1 — Install dependencies

**Ubuntu / Debian:**
```bash
sudo apt update
sudo apt install -y g++ cmake ninja-build \
  libboost-all-dev libssl-dev zlib1g-dev
```

**macOS (Homebrew):**
```bash
brew install cmake ninja boost openssl zlib
```

### Step 2 — Configure and build

```bash
cd vVault
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

The executable `vvault_server` will be created inside `build/`.

---

## Running the Server

```bash
# From the project root directory (important — the server looks for data/ here)
./build/vvault_server
# Windows:
./build/vvault_server.exe
```

By default the server listens on **http://localhost:8080**.  
To change the port, edit `main.cpp` and rebuild:
```cpp
app.port(8080)  // change to desired port
```

The server will automatically create a `data/` directory in the working directory on first run to store user data and encrypted files.

---

## API Reference

All endpoints accept and return `application/json`. Replace `<token>` with the session token received from `/api/login`.

### Authentication

| Method | Endpoint | Body | Description |
|--------|----------|------|-------------|
| POST | `/api/register` | `{ "username": "...", "password": "..." }` | Register a new user |
| POST | `/api/login` | `{ "username": "...", "password": "..." }` | Login — returns `{ "token": "..." }` |
| POST | `/api/logout` | `{ "token": "..." }` | Invalidate session |

### File Operations

| Method | Endpoint | Body / Params | Description |
|--------|----------|---------------|-------------|
| POST | `/api/upload` | `{ "token", "filename", "mimetype", "data" (Base64) }` | Upload & encrypt a file |
| GET | `/api/files` | Query: `?token=<token>` | List all files for the user |
| GET | `/api/download/:id` | Query: `?token=<token>` | Download & decrypt a file |
| DELETE | `/api/delete/:id` | `{ "token": "..." }` | Delete a file |
| PUT | `/api/rename/:id` | `{ "token", "newname" }` | Rename a file |

### Response Format

All responses follow the schema:
```json
{
  "success": true,
  "message": "Human-readable message",
  "data": { ... }
}
```

---

## Project Structure

```
vVault/
├── CMakeLists.txt          # CMake build configuration
├── main.cpp                # Entry point — server setup and all route definitions
├── include/
│   ├── crow/               # Crow HTTP framework (header-only)
│   ├── asio/               # Standalone Boost.Asio headers
│   └── nlohmann/
│       └── json.hpp        # nlohmann JSON (header-only)
├── data/                   # Created at runtime
│   ├── users.json          # Hashed user credentials
│   └── <username>/
│       ├── manifest.json   # File metadata store
│       └── *.enc           # AES-256 encrypted file blobs
└── build/                  # CMake build output
```

---

## Security Notes

- Passwords are stored as **SHA-256 hashes**, never in plaintext.
- Files are encrypted with **AES-256-CBC**; the encryption key is derived per-user using **PBKDF2-HMAC-SHA256**.
- A random **128-bit IV** is prepended to each encrypted file blob.
- Session tokens are **cryptographically random** 32-byte hex strings.
- For production use, enable **HTTPS** by configuring Crow's SSL support with a valid certificate.

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `libssl` / `libcrypto` not found | Set `OPENSSL_ROOT_DIR` in CMake: `-DOPENSSL_ROOT_DIR=/path/to/openssl` |
| Boost headers not found | Set `BOOST_ROOT`: `-DBOOST_ROOT=/path/to/boost` |
| Port 8080 already in use | Change `app.port(8080)` in `main.cpp` and rebuild |
| `data/` permission denied | Run the server from a directory where you have write access |
| Windows DLL not found | Copy `libssl-3-x64.dll`, `libcrypto-3-x64.dll`, `libz-1.dll` next to the `.exe` |

---

## License

This project was developed for academic purposes at GIKI as part of CS112L (OOP Lab). All rights reserved by the authors.
