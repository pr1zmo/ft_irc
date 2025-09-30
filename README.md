# ft_irc - Internet Relay Chat Server

## 📌 Project Overview
**ft_irc** is a custom implementation of an IRC (Internet Relay Chat) server, written in **C++98**.  
It allows multiple clients to connect using a real IRC client, authenticate, join channels, send messages, and interact in real-time.  

This project was part of the 42 curriculum and is designed to deepen understanding of:
- **Network programming** (sockets, TCP/IP)
- **Non-blocking I/O and multiplexing** (`epoll`)
- **Standard internet protocols**
- **Robust server design and error handling**

IRC is one of the earliest real-time text-based communication protocols, and building an IRC server from scratch provides strong insights into how modern chat systems operate.

---

## ⚙️ Features

### ✅ Mandatory
- Multiple clients can connect simultaneously.
- Authentication with password, nickname, and username.
- Channels with operators and regular users.
- Private messaging and channel-wide messaging.
- Channel operator commands:
  - `KICK` – remove a user from a channel.
  - `INVITE` – invite a user to a channel.
  - `TOPIC` – change/view the channel’s topic.
  - `MODE` – manage channel settings:
    - `i` → invite-only mode
    - `t` → topic restricted to operators
    - `k` → set/remove channel key (password)
    - `o` → grant/remove operator privileges
    - `l` → set/remove user limit

### 🎁 Bonus (implemented and working!)
- **File transfer** between clients.
- **Chat bot** integrated into the server.

---

## 🛠️ Technologies Used & Learned
- **C++98**: low-level programming with strict compliance to the standard.
- **Socket programming**: managing connections over TCP/IP (IPv4 & IPv6).
- **Multiplexing**: efficient client handling using `epoll()` (with alternatives supported).
- **Non-blocking I/O**: ensuring the server never hangs on client operations.
- **System calls**: `socket`, `bind`, `listen`, `accept`, `recv`, `send`, `fcntl`, etc.
- **Makefile**: for project compilation and management.

---

## 🚀 Getting Started

### 1. Clone the repo
```bash
git clone https://github.com/yourusername/ft_irc.git
cd ft_irc
```

### 2. Compile
```bash
make
```

### 3. Run the server
```bash
./ircserv <port> <password>
```
- **port** → the port number to listen on (e.g., `6667`).
- **password** → password required by clients to connect.

### 4. Connect using an IRC client
Example using `irssi`:
```bash
irssi -c localhost -p 6667 -w <password>
```

---

## 🧪 Testing
You can also test basic functionality using `nc`:
```bash
nc 127.0.0.1 6667
```

---

## 📖 Lessons Learned
Through this project, I gained experience in:
- Writing **robust server software** that can handle multiple clients gracefully.
- Debugging **concurrent networking issues**.
- Implementing **real-world protocols** from scratch.
- Designing **clean, modular C++ code** without external libraries.

---

## 👥 Authors
- [zelbassa](https://github.com/pr1zmo)
- [mbouras](https://github.com/medibrs)
- [adouiyeh](https://github.com/adouiyeh)
