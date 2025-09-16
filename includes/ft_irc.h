#ifndef ft_irc_h
# define ft_irc_h
# include <iostream>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <cstring>
# include <cstdlib>
# include <vector>
# include <map>
# include <sstream>
# include <sys/epoll.h>
# include <fcntl.h>
# include <ctime>
# include <algorithm>
# include <cctype>
# include <csignal>
# include <iomanip>
# include <fstream>
# include <cerrno>

# define MAX_CLIENTS 1024
# define MAX_EVENTS 128
# define BUFFER_SIZE 512
# define DEFAULT_PORT 6667

using namespace std;

#endif