#include "auth/AuthMiddleware.h"
#include "common/ApiResponse.h"
#include "common/Logger.h"

std::optional<std::string> extractBearerToken(const drogon::HttpRequestPtr &req) {
    /*
        功能：从 Authorization 请求头里提取token 
    */
    const auto auth = req->getHeader("Authorization");
    if (auth.empty()) return std::nullopt;

    const std::string prefix = "Bearer ";
    if (auth.rfind(prefix, 0) != 0) return std::nullopt;

    auto token = auth.substr(prefix.size());
    if (token.empty()) return std::nullopt;
    return token;
}

std::optional<Identity> getIdentity(const drogon::HttpRequestPtr &req, AppContext &ctx) {
    /*
        功能：先提取token，在调用ctx.tokenProvider->verifyToken(...)验证身份
    */
    auto tokenOpt = extractBearerToken(req);
    if (!tokenOpt.has_value()) return std::nullopt;
    return ctx.tokenProvider->verifyToken(*tokenOpt);
}

bool requireAuth(const drogon::HttpRequestPtr &req,
                 std::function<void(const drogon::HttpResponsePtr &)> &callback,
                 AppContext &ctx) {
    /*
        功能：校验是否已登录（是否有有效token）
        * 成功：返回 true
        * 失败：直接构造401响应并callback(resp)，然后返回false
    */
    auto identityOpt = getIdentity(req, ctx);
    if (!identityOpt.has_value()) {
        auto resp = api_resp::makeJson(
            40101,
            "Unauthorized: missing or invalid Authorization Bearer token",
            Json::Value(Json::objectValue),
            drogon::k401Unauthorized
        );
        logResponse(req, resp);
        callback(resp);
        return false;
    }
    return true;
}

bool requireAdmin(const drogon::HttpRequestPtr &req,
                  std::function<void(const drogon::HttpResponsePtr &)> &callback,
                  AppContext &ctx) {
    /*
        功能：校验管理员权限（内部也包含登陆校验）
    */
    auto identityOpt = getIdentity(req, ctx);
    if (!identityOpt.has_value()) {
        auto resp = api_resp::makeJson(
            40101,
            "Unauthorized",
            Json::Value(Json::objectValue),
            drogon::k401Unauthorized
        );
        logResponse(req, resp);
        callback(resp);
        return false;
    }

    if (identityOpt->role != "admin") {
        auto resp = api_resp::makeJson(
            40301,
            "Forbidden: admin role required",
            Json::Value(Json::objectValue),
            drogon::k403Forbidden
        );
        logResponse(req, resp);
        callback(resp);
        return false;
    }

    return true;
}