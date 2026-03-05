#pragma once
#include <json/json.h>
#include <optional>
#include <string>

namespace services {

class PythonRagClient {
public:

    /*
    功能：专门负责 C++ 调Python服务器的HTTP通信细节
    */
    static std::optional<Json::Value> ask(
        const std::string &baseUrl,
        const std::string &question,
        const std::string &userId,
        const std::string &traceId,
        int topK,
        double temperature,
        std::string &errMsg
    );
};

}  // namespace services