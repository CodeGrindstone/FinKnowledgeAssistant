#include "services/PythonRagClient.h"
#include <drogon/drogon.h>

using namespace drogon;

namespace services {

std::optional<Json::Value> PythonRagClient::ask(
    const std::string &baseUrl,
    const std::string &question,
    const std::string &userId,
    const std::string &traceId,
    int topK,
    double temperature,
    std::string &errMsg) {

    errMsg.clear();

    auto client = HttpClient::newHttpClient(baseUrl);
    if (!client) {
        errMsg = "failed to create HttpClient";
        return std::nullopt;
    }

    Json::Value body(Json::objectValue);
    body["question"] = question;
    body["user_id"] = userId;
    body["trace_id"] = traceId;

    Json::Value options(Json::objectValue);
    options["top_k"] = topK;
    options["temperature"] = temperature;
    body["options"] = options;

    auto req = HttpRequest::newHttpJsonRequest(body);
    req->setMethod(Post);
    req->setPath("/rag/ask");

    // 同步请求（开发阶段方便调试；后续可改异步）
    auto [result, resp] = client->sendRequest(req);

    if (result != ReqResult::Ok || !resp) {
        errMsg = "python rag request failed";
        return std::nullopt;
    }

    if (resp->statusCode() != k200OK) {
        errMsg = "python rag http status not 200";
        return std::nullopt;
    }

    auto json = resp->getJsonObject();
    if (!json) {
        errMsg = "python rag returned invalid json";
        return std::nullopt;
    }

    int code = (*json).get("code", -1).asInt();
    if (code != 0) {
        errMsg = (*json).get("message", "python rag business error").asString();
        return std::nullopt;
    }

    if (!json->isMember("data")) {
        errMsg = "python rag missing data field";
        return std::nullopt;
    }

    return (*json)["data"];
}

}  // namespace services