# ft_irc - Internet Relay Chat Server

## 📌 Project Overview
**ft_irc** is a custom implementation of an IRC (Internet Relay Chat) server, written in **C++98**.  
It allows multiple clients to connect using standard IRC clients (like irssi, HexChat, or nc), authenticate, join channels, send messages, and interact in real-time.  

This project was built to comply with the 42 curriculum standards, focusing on:
- **Network programming** (TCP/IP sockets, IPv4).
- **Non-blocking I/O and I/O Multiplexing** (using `epoll`).
- **RFC Compliance** (partial implementation of RFC 1459/2812).
- **Robust server design** (Signal handling, graceful shutdowns, memory management).

---

## ⚙️ Features

### ✅ Core Functionality
- **Multi-client Support**: Handles multiple simultaneous connections via non-blocking file descriptors.
- **Authentication**: Password verification (`PASS`), Nickname registration (`NICK`), and User setup (`USER`).
- **Graceful Shutdown**: Handles signals (`SIGINT`, `SIGQUIT`) to close connections and free memory properly.

### 📡 Supported Commands
The server supports the following IRC commands:

| Category | Commands | Description |
|----------|----------|-------------|
| **Auth** | `PASS`, `NICK`, `USER` | Connection registration and authentication. |
| **Channel** | `JOIN`, `PART` | Join or leave specific channels. |
| **Channel** | `TOPIC` | View or change the channel topic. |
| **Channel** | `NAMES`, `LIST` | View users in a channel or list available channels. |
| **Channel** | `INVITE`, `KICK` | Invite users or forcibly remove them. |
| **Modes** | `MODE` | Set channel privileges and restrictions (see below). |
| **Comms** | `PRIVMSG`, `NOTICE` | Send private messages or channel messages. |
| **Server** | `PING`, `PONG` | Keep-alive mechanism to detect lost connections. |
| **Server** | `OPER`, `QUIT` | Operator privileges and disconnection. |

### 🛡️ Channel Modes (`MODE`)
- `i` : **Invite-only** channel.
- `t` : **Topic** restricted to channel operators.
- `k` : **Key** (password) required to join.
- `o` : Give/take **Operator** privileges.
- `l` : Set **User limit** for the channel.

---

## 🎁 Bonus Features

### 📂 File Transfer (DCC)
The server supports **Direct Client-to-Client (DCC)** file transfer.
- **Mechanism**: The server acts as a transparent relay for DCC handshake messages sent via `PRIVMSG`.
- **How it works**: Clients exchange IP/Port information through the server, then establish a direct TCP connection between themselves to transfer files.
- **Usage**: Works out-of-the-box with modern clients (e.g., `/dcc send <nick> <file>` in irssi).

### 🤖 IRC Bot
An integrated automated bot is included in the project (`src/bot/`).
- The bot can connect to the server, join channels, and interact with users.
- Provides utility or fun responses to specific commands.

---

## 🛠️ Technologies & Design
- **Language**: C++98 (Strict standard compliance).
- **Architecture**: Event-driven using `epoll()` (Linux).
- **Pattern**: Command Pattern (via `Executioner.cpp`) to decouple command parsing from execution logic.
- **Socket API**: `socket`, `bind`, `listen`, `accept`, `recv`, `send`.
- **Utils**: Custom parsing logic for IRC messages (handling `\r\n`, partial packets, and buffering).

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
*   **port**: The port number to listen on (e.g., `6667`).
*   **password**: The connection password.

### 4. Connect using an IRC client
**Using `irssi`:**
```bash
irssi -c localhost -p 6667 -w <password>
```

**Using `netcat` (raw TCP):**
```bash
nc localhost 6667
PASS <password>
NICK <nickname>
USER <username> 0 * :<realname>
```

---

## 👥 Authors
- [zelbassa](https://github.com/pr1zmo)
- [mbouras](https://github.com/medibrs)
- [adouiyeh](https://github.com/adouiyeh)
```