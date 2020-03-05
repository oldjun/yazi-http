# yazi-http
a c++ http server with epoll and thread pool

|Author|junping|
|---|---|
|E-mail|oldjun@sina.com|
|Wechat|chenjunping1024|

## Start the HTTP server
```c++
#include <iostream>
using namespace std;

#include "Singleton.h"
using namespace yazi::utility;

#include "Server.h"
#include "Request.h"
#include "Response.h"
using namespace yazi::server;

int main()
{
    Singleton<System>::instance()->init();

    Server * server = Singleton<Server>::instance();

    server->on("/", handle_html);
    server->on("/json", handle_json);

    server->listen("", 15599);
    server->start();

    return 0;
}
```

## Handle dynamic web request
```c++
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
```
## Handle static web request
if server didn't register a `handler` to the specific request, server will look for the html page in the directory `html`.

## Compile & Run the project
download the source code, cd into the yazi-http project working directory, run command make && ./main 
```bash
cd ./yazi-http
make
./main
```
