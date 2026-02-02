# ft_irc

An IRC server implementation in C++98, compatible with standard IRC clients.

## Requirements

- C++ compiler with C++98 support
- Linux (uses epoll for I/O multiplexing)
- Make

## Building

```bash
# Build the server
make

# Build the bot (bonus)
make bonus

# Clean object files
make clean

# Full clean (objects + binaries)
make fclean

# Rebuild
make re
```

## Usage

### Server

```bash
./ircserv <port> <password>
```

- `port`: Port number to listen on (must be >= 1024)
- `password`: Server password required for client connections

Example:
```bash
./ircserv 6667 mypassword
```

### Bot (Bonus)

```bash
./ircbot
```

The bot reads its configuration from `src/bot/bot.conf`.

#### Bot Configuration

Edit `src/bot/bot.conf`:

```ini
channels = #general,#dev

[server]
host = 127.0.0.1
port = 6667
password = mypassword

[identity]
nickname = Bot42
username = Bot42
realname = Automated IRC Service Bot

[logging]
enabled = true
log_file = bot.logs
```

## Supported Commands

### Registration

| Command | Description |
|---------|-------------|
| `PASS <password>` | Authenticate with server password |
| `NICK <nickname>` | Set or change nickname |
| `USER <user> <mode> * :<realname>` | Set username and realname |

### Session

| Command | Description |
|---------|-------------|
| `PING <token>` | Client keepalive |
| `PONG <token>` | Response to server PING |
| `QUIT [:message]` | Disconnect from server |

### Messaging

| Command | Description |
|---------|-------------|
| `PRIVMSG <target> :<text>` | Send message to user or channel |
| `NOTICE <target> :<text>` | Send notice (no auto-reply) |

### Channels

| Command | Description |
|---------|-------------|
| `JOIN <#channel> [key]` | Join or create a channel |
| `PART <#channel> [:reason]` | Leave a channel |
| `TOPIC <#channel>` | View channel topic |
| `TOPIC <#channel> :<text>` | Set channel topic |
| `LIST` | List available channels |
| `KICK <#channel> <nick> [:reason]` | Remove user from channel (operator only) |
| `INVITE <nick> <#channel>` | Invite user to channel (operator only) |

### Channel Modes

```
MODE <#channel> [+|-]<modes> [arguments]
```

| Mode | Description |
|------|-------------|
| `i` | Invite-only channel |
| `t` | Topic restricted to operators |
| `k <key>` | Channel password |
| `l <limit>` | User limit |
| `o <nick>` | Grant/revoke operator status |

## Connecting with an IRC Client

Any standard IRC client should work. Examples:

**irssi:**
```
/connect 127.0.0.1 6667 mypassword
```

**netcat (testing):**
```bash
nc 127.0.0.1 6667
PASS mypassword
NICK testnick
USER testuser 0 * :Test User
```

## Architecture

- Uses epoll for handling multiple client connections
- Non-blocking I/O
- Single-threaded event loop
- Signal handling for graceful shutdown (SIGINT, SIGTERM, SIGQUIT)

## File Structure

```
src/           - Server source files
src/bot/       - IRC bot source files (bonus)
includes/      - Header files
tests/         - Python test suite
docs/          - Documentation
```

## Testing

Python test files are located in `tests/`. The test suite covers:

- Client registration
- Channel operations
- Private messaging
- Channel modes
- Quit and cleanup behavior

## License

This project was developed as part of the 42 school curriculum.
