#include "Server.h"
using namespace yazi::server;

#include <fstream>
#include <sstream>
using std::ostringstream;

#include "System.h"
#include "Singleton.h"
using namespace yazi::utility;

#include "TaskDispatcher.h"
using namespace yazi::thread;

#include "SocketHandler.h"
using namespace yazi::socket;

Server::Server() : m_ip(""), m_port(0), m_threads(1024), m_connects(1024)
{
    System * sys = Singleton<System>::instance();
    m_root_path = sys->get_root_path();
}

Server::~Server()
{
}

void Server::listen(const string & ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void Server::start()
{
    // initialize the thread pool and task queue
    TaskDispatcher * dispatcher = Singleton<TaskDispatcher>::instance();
    dispatcher->init(m_threads);

    // initialize the socket handler
    SocketHandler * socket_handler = Singleton<SocketHandler>::instance();
    socket_handler->listen(m_ip, m_port);
    socket_handler->handle(m_connects);
}

void Server::set_threads(int threads)
{
    m_threads = threads;
}

void Server::set_connects(int connects)
{
    m_connects = connects;
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
        filename = m_root_path + "/html/index.html";
    }
    else
    {
        filename = m_root_path + "/html" + path;
    }
    std::ifstream in(filename.c_str());
    if (!in.good())
    {
        string page404 = m_root_path + "/html/404.html";
        in.open(page404.c_str());
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
