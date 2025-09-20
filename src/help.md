*** ft_irc — HELP ***

Server start (shell):
  ./ircserv <port> <password>

Registration (do these first):
  PASS <password>                  — provide server password
  NICK <nickname>                  — set your nickname
  USER <username> 0 * :<realname>  — set username & real name

Messaging:
  PRIVMSG <nick>|<#channel> :<text>   — send a private or channel message

Channels:
  JOIN <#chan>[,<#chan>...] [<key>[,<key>...]]   — join/create channels

Channel operators:
  KICK <#chan> <nick> [<reason>]                 — remove a user
  INVITE <nick> <#chan>                          — invite a user
  TOPIC <#chan> [:<new-topic>]                   — set/view the topic
  MODE <#chan> {[+|-]i|t|k|o|l} [<args>...]      — channel modes:
     +i/-i         invite-only on/off
     +t/-t         restrict TOPIC to ops / allow all
     +k <key>/-k   set/remove channel key (password)
     +o <nick>/-o  give/take operator privilege
     +l <n>/-l     set/remove user limit

Conventions:
  • Commands are case-insensitive and end with CRLF (\r\n).
  • Channels start with '#'. The last parameter may contain spaces and must be prefixed with ':'.
  • Targets accept either a nickname or a #channel, depending on the command.

Quick start:
  PASS s3cr3t
  NICK alice
  USER alice 0 * :Alice Liddell
  JOIN #tea
  PRIVMSG #tea :hello world
