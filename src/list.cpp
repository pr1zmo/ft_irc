#include "Commands.hpp"
#include "Channel.hpp"


List::List(/* args */)
: Command()
{
}
List::~List()
{
}


void List::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    (void)param;
    (void)cmd;
    (void)clients;

    const std::map<std::string, Channel*>& channels = server.getChannels();
    cli.response(":server 321 " + cli.getNickname() + " Channel  + Number of users\r\n"); // RPL_LISTSTART

    for (std::map<std::string, Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel* channel = it->second;
        std::string channelName = channel->getName();
        size_t userCount = channel->userCount();
        cli.response(":server 322 " + cli.getNickname() + " " + channelName + " " + to_string98(static_cast<int>(userCount)) + " :\r\n"); // RPL_LIST
    }

    cli.response(":server 323 " + cli.getNickname() + " :End of /LIST\r\n"); // RPL_LISTEND
}