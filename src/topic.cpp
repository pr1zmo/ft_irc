#include "Commands.hpp"
#include "Channel.hpp"



Topic::Topic(/* args */)
: Command()
{
}  
Topic::~Topic()
{
}

void Topic::execute(Client &cli, const std::string& param, const std::string& cmd, std::map<int, Client>& clients, Server& server) {
    (void)clients;
    std::istringstream iss(param);
    std::string channel_name;
    std::string new_topic;

    if (!(iss >> channel_name)) {
        cli.response("Usage: " + cmd + " <#channel> [:<new-topic>]\r\n");
        return;
    }

    std::getline(iss, new_topic);
    if (!new_topic.empty() && new_topic[0] == ' ') new_topic = new_topic.substr(1);
    if (!new_topic.empty() && new_topic[0] == ':') new_topic = new_topic.substr(1);

    Channel* channel = server.getChannel(channel_name);
    if (!channel) {
        cli.response(":server 403 " + cli.getNickname() + " " + channel_name + " :No such channel\r\n");
        return;
    }

    if (new_topic.empty()) {
        const std::string topic = channel->getTopic();
        if (topic.empty()) {
            cli.response(":server 331 " + cli.getNickname() + " " + channel_name + " :No topic is set\r\n");
        } else {
            cli.response(":server 332 " + cli.getNickname() + " " + channel_name + " :" + topic + "\r\n");
        }
        return;
    }

    if (!channel->contains(cli.getNickname())) {
        cli.response(":server 442 " + cli.getNickname() + " " + channel_name + " :You're not on that channel\r\n");
        return;
    }

    if (channel->isTopicRestricted() && !channel->isOp(cli.getNickname())) {
        cli.response(":server 482 " + cli.getNickname() + " " + channel_name + " :You're not channel operator\r\n");
        return;
    }

    channel->setTopic(new_topic);

    std::string topicMsg = ":" + cli.getNickname() + "!" + cli.getUsername() + "@" + cli.getHostname()
                         + " TOPIC " + channel_name + " :" + new_topic + "\r\n";
    channel->broadcast(cli.getNickname(), topicMsg, server);
}