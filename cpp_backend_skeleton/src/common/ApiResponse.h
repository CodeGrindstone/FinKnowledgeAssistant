#pragma once
#include <drogon/drogon.h>
#include <json/json.h>
#include <optional>
#include <string>

namespace api_resp {

    /*
    统一接口返回格式
    */
inline Json::Value build(int code,
                         const std::string &message,
                         const Json::Value &data = Json::Value(Json::objectValue),
                         const std::optional<std::string> &requestId = std::nullopt) {
    Json::Value root(Json::objectValue);
    root["code"] = code;
    root["message"] = message;
    root["data"] = data;
    if (requestId.has_value()) {
        root["request_id"] = *requestId;
    }
    return root;
}

inline drogon::HttpResponsePtr makeJson(
    int bizCode,
    const std::string &message,
    const Json::Value &data = Json::Value(Json::objectValue),
    drogon::HttpStatusCode httpCode = drogon::k200OK,
    const std::optional<std::string> &requestId = std::nullopt) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse(build(bizCode, message, data, requestId));
    resp->setStatusCode(httpCode);
    return resp;
}

}  // namespace api_resp