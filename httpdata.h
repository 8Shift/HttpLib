#pragma once

#include <vector>
#include <map>

#pragma comment(lib, "Ws2_32.lib")

enum class HttpMethod
{
    GET,
    HEAD,
    POST,
    PUT,
    PATCH,
    DEL,
    TRACE,
    CONNECT
};

std::ostream& operator<<(std::ostream &os, const HttpMethod method);

enum class HttpConnectionType
{
    KEEPALIVE,
    CLOSE
};

std::ostream& operator<<(std::ostream &os, const HttpConnectionType method);

enum class HttpDataType
{
    TEXT_HTML,
    TEXT_PLAIN
};

std::ostream& operator<<(std::ostream &os, const HttpDataType method);

const extern std::map <HttpConnectionType, std::string> connectionTypeTable;
const extern std::map <HttpDataType, std::string> dataTypeTable;
const extern std::map <HttpMethod, std::string> httpMethodTable;


struct HttpHeader
{
    HttpMethod method; // http packet startline
    std::string path;
    std::string hostname;
    std::string userAgent;
    HttpDataType dataType;
    HttpConnectionType connectionType;
    const std::string bodySlice = "\n"; // empty line to slice body
};

struct HttpBody
{
    HttpHeader header;
    std::vector <char> data;

    std::vector <char> serialize();
    void deserialize(std::vector <char>& buffer);
};