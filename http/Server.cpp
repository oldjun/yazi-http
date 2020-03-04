#include "Server.h"
using namespace yazi::http;

#include <sys/resource.h>

#include <fstream>
#include <sstream>
using std::ostringstream;

#include "Logger.h"
#include "Singleton.h"
using namespace yazi::utility;

#include "TaskDispatcher.h"
using namespace yazi::thread;

#include "SocketHandler.h"
using namespace yazi::socket;

#include "HttpTask.h"
using namespace yazi::task;

Task * http_task_creator(Socket * socket)
{
    return new HttpTask(socket);
}

Server::Server()
{
    core_dump();

    m_root_path = get_root_path();

    // initialize the global logger
    Logger::instance()->open(m_root_path + "/main.log");
}

Server::~Server()
{
}

void Server::on(const string & path, server_handler handler)
{
    m_handlers[path] = handler;
}

string Server::handle(const Request & req)
{
    const string & path = req.path();
    std::map<string, server_handler>::iterator it = m_handlers.find(path);
    if (it != m_handlers.end())
    {
        Response resp;
        it->second(req, resp);
        return resp.data();
    }

    string filename;

    if (path == "/")
    {
        filename = "./html/index.html";
    }
    else
    {
        filename = "./html" + path;
    }
    std::ifstream in(filename.c_str());
    if (!in.good())
    {
        in.open("./html/404.html");
        ostringstream buf;
        char ch;
        while(in.get(ch))
        {
            buf.put(ch);
        }
        string body = buf.str();

        ostringstream os;
        os << "HTTP/1.1 404 Not Found\r\n";
        os << "Content-Type: text/html; charset: UTF-8\r\n";
        os << "Content-Length: " << body.size() << "\r\n\r\n";
        os << body << "\r\n";
        return os.str();
    }
    ostringstream buf;
    char ch;
    while(in.get(ch))
    {
        buf.put(ch);
    }
    string body = buf.str();

    ostringstream os;
    os << "HTTP/1.1 200 OK\r\n";
    os << "Content-Type: text/html; charset: UTF-8\r\n";
    os << "Content-Length: " << body.size() << "\r\n\r\n";
    os << body << "\r\n";
    return os.str();
}

void Server::listen(const string & ip, int port)
{
    // initialize the socket handler
    SocketHandler * socket_handler = Singleton<SocketHandler>::instance();
    socket_handler->listen(ip, port);
}

void Server::start(int threads, int connects)
{
    // initialize the thread pool and task queue
    Singleton<TaskDispatcher>::instance()->init(threads);

    SocketHandler * socket_handler = Singleton<SocketHandler>::instance();

    // register the echo task creator
    socket_handler->creator(http_task_creator);
    socket_handler->handle(connects);
}

void Server::core_dump()
{
    // core dump
    struct rlimit x;
    int ret = getrlimit(RLIMIT_CORE, &x);
    x.rlim_cur = x.rlim_max;
    ret = setrlimit(RLIMIT_CORE, &x);
    ret = getrlimit(RLIMIT_DATA, &x);
    x.rlim_cur = 768000000;
    ret = setrlimit(RLIMIT_DATA, &x);
}

string Server::get_root_path()
{
    char path[1024];
    memset(path, 0, 1024);
    int cnt = readlink("/proc/self/exe", path, 1024);
    if (cnt < 0 || cnt >= 1024)
    {
        return "";
    }
    for (int i = cnt; i >= 0; --i)
    {
        if (path[i] == '/')
        {
            path[i] = '\0';
            break;
        }
    }
    return string(path);
}