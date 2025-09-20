#pragma once
/**
 * Commands.hpp
 *
 * Declarations for IRC-style command handling using the Command pattern.
 * 
 * This header intentionally contains only declarations (no heavy includes)
 * to keep compile times reasonable even when included widely. Implement each
 * command in its own .cpp (recommended), or a few grouped .cpps.
 */
 
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <optional>

namespace irc {

// Forward declarations for types your server likely already defines.
// Replace/extend these to match your codebase.
class Session;      // represents a client connection (socket, send queue, etc.)
class ServerState;  // global shared state (channels, users, config)
class Logger;       // logging utility (optional)

/**
 * Lightweight context passed to every command.
 * Add what you need (auth, rate limits, permissions, etc.).
 */
struct CommandContext {
    ServerState&     server;
    Session&         session;
    Logger*          log;           // nullable
    std::string_view rawLine;       // raw line as received (without CRLF)
    std::string_view commandName;   // uppercased command verb (e.g., "JOIN")
};

/**
 * Simple result type for a command execution.
 * You can expand with numeric reply codes, message batches, etc.
 */
struct CommandResult {
    bool ok = true;
    std::optional<int> numeric;      // e.g., RFC1459 numeric reply (optional)
    std::string message;             // human/debug text (optional)
};

/**
 * Base interface for all commands.
 */
class ICommand {
public:
    virtual ~ICommand() = default;

    /**
     * Execute the command.
     * @param ctx   Context (server, session, raw line, etc.)
     * @param args  Tokenized args (already split, without the command verb)
     * @return      CommandResult (success/failure + optional info)
     */
    virtual CommandResult execute(const CommandContext& ctx,
                                  const std::vector<std::string>& args) = 0;

    /**
     * Canonical uppercase name of the command (e.g., "JOIN").
     */
    virtual std::string_view name() const = 0;

    /**
     * Optional: short, human-readable usage string.
     */
    virtual std::string_view usage() const { return {}; }
};

/**
 * Registry that maps command names to factories and helps dispatch.
 * Typical usage:
 *   CommandRegistry reg;
 *   reg.registerCommand<JoinCommand>();
 *   auto cmd = reg.make("JOIN");
 *   if (cmd) cmd->execute(ctx, args);
 */
class CommandRegistry {
public:
    using Factory = std::function<std::unique_ptr<ICommand>()>;

    bool registerFactory(std::string nameUpper, Factory factory) {
        // normalize to uppercase key
        for (auto& c : nameUpper) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
        return factories_.emplace(std::move(nameUpper), std::move(factory)).second;
    }

    template <typename CommandT>
    bool registerCommand() {
        auto tmp = CommandT{};
        auto key = std::string(tmp.name());
        for (auto& c : key) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
        return registerFactory(std::move(key), [] { return std::make_unique<CommandT>(); });
    }

    std::unique_ptr<ICommand> make(std::string nameUpper) const {
        for (auto& c : nameUpper) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
        if (auto it = factories_.find(nameUpper); it != factories_.end()) {
            return (it->second)();
        }
        return nullptr;
    }

    bool contains(std::string_view nameUpper) const {
        std::string key{nameUpper};
        for (auto& c : key) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));
        return factories_.find(key) != factories_.end();
    }

private:
    std::unordered_map<std::string, Factory> factories_;
};

/* ==============================
 * Command Class Declarations
 * ==============================
 * Each command implements:
 *   - name(): returns uppercase command name
 *   - usage(): short usage hint (optional)
 *   - execute()
 *
 * Implementations go in .cpp files, e.g., PassCommand.cpp, NickCommand.cpp, etc.
 */

class PassCommand : public ICommand {
public:
    std::string_view name() const override { return "PASS"; }
    std::string_view usage() const override { return "PASS <password>"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class NickCommand : public ICommand {
public:
    std::string_view name() const override { return "NICK"; }
    std::string_view usage() const override { return "NICK <nickname>"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class UserCommand : public ICommand {
public:
    std::string_view name() const override { return "USER"; }
    std::string_view usage() const override { return "USER <username> <mode> * <realname>"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class OperCommand : public ICommand {
public:
    std::string_view name() const override { return "OPER"; }
    std::string_view usage() const override { return "OPER <name> <password>"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class QuitCommand : public ICommand {
public:
    std::string_view name() const override { return "QUIT"; }
    std::string_view usage() const override { return "QUIT [:message]"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class JoinCommand : public ICommand {
public:
    std::string_view name() const override { return "JOIN"; }
    std::string_view usage() const override { return "JOIN <channel>{,<channel>} [<key>{,<key>}]"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class PartCommand : public ICommand {
public:
    std::string_view name() const override { return "PART"; }
    std::string_view usage() const override { return "PART <channel>{,<channel>} [:message]"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class PrivMsgCommand : public ICommand {
public:
    std::string_view name() const override { return "PRIVMSG"; }
    std::string_view usage() const override { return "PRIVMSG <receiver>{,<receiver>} :<text>"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class NoticeCommand : public ICommand {
public:
    std::string_view name() const override { return "NOTICE"; }
    std::string_view usage() const override { return "NOTICE <nickname>|<channel> :<text>"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

class PingCommand : public ICommand {
public:
    std::string_view name() const override { return "PING"; }
    std::string_view usage() const override { return "PING <token>"; }
    CommandResult execute(const CommandContext& ctx,
                          const std::vector<std::string>& args) override;
};

/* ==============================
 * Utility: default tokenizer
 * ==============================
 * Split a raw IRC line (sans CRLF) into command + args, respecting the
 * trailing parameter introduced by ':' (RFC 1459). Provide declaration here;
 * implement in a .cpp for reuse in your input pipeline.
 */
struct ParsedLine {
    std::string verb;                   // uppercase command verb
    std::vector<std::string> args;      // parameters (trailing combined)
};

ParsedLine parseIrcLine(std::string_view raw);

/* ==============================
 * Convenience: bulk registration
 * ==============================
 * Call this once at startup to register all built-in commands.
 */
void registerBuiltinCommands(CommandRegistry& reg);

} // namespace irc
