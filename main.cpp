#include <iostream>
using namespace std;

#include "Singleton.h"
using namespace yazi::utility;

#include "Server.h"
#include "Request.h"
#include "Response.h"
using namespace yazi::http;

#include "Json.h"
using namespace yazi::json;

void handle_html(const Request & req, Response & resp)
{
    string html = "<!DOCTYPE html> \
                    <html> \
                        <head> \
                        <meta charset=\"utf-8\"> \
                        <title>Welcome to yazi-http!</title> \
                    </head> \
                    <body> \
                        <h1>this is html page</h1> \
                        <p>yazi-http是一个轻量级的c++ http服务器</p> \
                    </body> \
                </html>";
    resp.html(html);
}

void handle_json(const Request & req, Response & resp)
{
    Json json;
    json["code"] = 0;
    json["type"] = "json";
    json["data"]["id"] = req.get("id");
    json["data"]["name"] = req.get("name");
    
    resp.json(json.toString());
}

int main()
{
    Server * server = Singleton<Server>::instance();

    server->on("/", handle_html);
    server->on("/json", handle_json);

    server->listen("", 15599);
    server->start();

    return 0;
}
