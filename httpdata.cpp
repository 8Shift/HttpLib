#include <string>
#include <sstream>
#include <map>
#include <stdexcept>

#include "httpdata.h"

const std::map <HttpConnectionType, std::string> connectionTypeTable =
{
    {HttpConnectionType::KEEPALIVE, "keep-alive"},
    {HttpConnectionType::CLOSE, "close"}
};

const std::map <HttpDataType, std::string> dataTypeTable =
{
    {HttpDataType::TEXT_HTML, "text/html"},
    {HttpDataType::TEXT_PLAIN, "text/plain"}
};

const std::map <HttpMethod, std::string> httpMethodTable =
{
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::HEAD, "HEAD"},
    {HttpMethod::PUT, "PUT"},
    {HttpMethod::PATCH, "PATCH"},
    {HttpMethod::DEL, "DEL"},
    {HttpMethod::TRACE, "TRACE"},
    {HttpMethod::CONNECT, "CONNECT"},
};

std::ostream & operator<<(std::ostream &os, const HttpMethod method)
{
    os << httpMethodTable.at(method);
    return os;
}

std::ostream & operator<<(std::ostream &os, const HttpConnectionType method)
{
    os << connectionTypeTable.at(method);
    return os;
}

std::ostream & operator<<(std::ostream &os, const HttpDataType method)
{
    os << dataTypeTable.at(method);
    return os;
}

std::vector<char> HttpBody::serialize()
{
    std::vector<char> buffer;
    std::string startLine = httpMethodTable.at(header.method) + " " + header.path + " HTTP/1.1\r\n";
    buffer.insert(buffer.end(), startLine.begin(), startLine.end());

    std::string hostLine = "Host: " + header.hostname + "\r\n";
    buffer.insert(buffer.end(), hostLine.begin(), hostLine.end());

    std::string userAgentLine = "User-Agent: " + header.userAgent + "\r\n";
    buffer.insert(buffer.end(), userAgentLine.begin(), userAgentLine.end());

    std::string connectionLine = "Connection: " + connectionTypeTable.at(header.connectionType) + "\r\n";
    buffer.insert(buffer.end(), connectionLine.begin(), connectionLine.end());

    if (!data.empty())
    {
        std::string contentTypeLine = "Content-Type: " + dataTypeTable.at(header.dataType) + "\r\n";
        buffer.insert(buffer.end(), contentTypeLine.begin(), contentTypeLine.end());

        std::string contentLengthLine = "Content-Length: " + std::to_string(data.size()) + "\r\n";
        buffer.insert(buffer.end(), contentLengthLine.begin(), contentLengthLine.end());
    }

    buffer.insert(buffer.end(), "\r\n", "\r\n" + 2);

    if (!data.empty())
    {
        buffer.insert(buffer.end(), data.begin(), data.end());
    }

    return buffer;
}

void HttpBody::deserialize(std::vector <char>& buffer)
{
    std::string data(buffer.begin(), buffer.end());

    size_t headerEnd = data.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        throw std::runtime_error("invalid packet");
    }

    std::string headersPart = data.substr(0, headerEnd);
    std::string bodyPart = data.substr(headerEnd + 4);

    std::istringstream headersStream(headersPart);
    std::string line;

    if (std::getline(headersStream, line))
    {
        std::istringstream startLine(line);
        std::string method, path, version;
        startLine >> method >> path >> version;

        for (const auto& [k, v] : httpMethodTable)
        {
            if (v == method)
            {
                header.method = k;
                break;
            }
        }

        header.path = path;
    }

    while (std::getline(headersStream, line))
    {
        if (size_t colonPos = line.find(':'); colonPos != std::string::npos)
        {
            std::string headerName = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 1);

            headerValue.erase(0, headerValue.find_first_not_of(" \t"));
            headerValue.erase(headerValue.find_last_not_of(" \t") + 1);

            if (headerName == "Host") {
                header.hostname = headerValue;
            }
            else if (headerName == "User-Agent")
            {
                header.userAgent = headerValue;
            }
            else if (headerName == "Connection")
            {
                for (const auto& [k, v] : connectionTypeTable)
                {
                    if (v == headerValue) {
                        header.connectionType = k;
                        break;
                    }
                }
            }
            else if (headerName == "Content-Type")
            {
                for (const auto& [k, v] : dataTypeTable)
                {
                    if (v == headerValue)
                    {
                        header.dataType = k;
                        break;
                    }
                }
            }
        }
    }
    data.assign(bodyPart.begin(), bodyPart.end());
}
