#include "controllers/AuthController.h"
#include <drogon/drogon.h>

#include "auth/AuthMiddleware.h"
#include "common/ApiResponse.h"
#include "common/Logger.h"

using namespace drogon;

namespace controllers {

void registerAuthRoutes(AppContext &ctx) {
    /*
    功能：注册并实现认证相关接口：
            * /api/auth/login
            * /api/auth/logout
            * /api/auth/me
    */
    app().registerHandler(
        "/api/auth/login",
        [&ctx](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            /*
            1.解析 JSON body
            2.校验 username/password 是否存在
            3.在 ctx.users 中查mock用户
            4.调 ctx.tokenProvider->issueToken(...)
            5.返回token / role / username
            */
            auto json = req->getJsonObject();
            if (!json) {
                auto resp = api_resp::makeJson(
                    40001, "Invalid JSON body", Json::Value(Json::objectValue), k400BadRequest);
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            const std::string username = json->get("username", "").asString();
            const std::string password = json->get("password", "").asString();

            if (username.empty() || password.empty()) {
                auto resp = api_resp::makeJson(
                    40002, "username and password are required",
                    Json::Value(Json::objectValue), k400BadRequest);
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            auto it = ctx.users.find(username);
            if (it == ctx.users.end() || it->second.password != password) {
                auto resp = api_resp::makeJson(
                    40103, "Invalid username or password",
                    Json::Value(Json::objectValue), k401Unauthorized);
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            const auto &user = it->second;
            const auto token = ctx.tokenProvider->issueToken(user.username, user.role);

            Json::Value data(Json::objectValue);
            data["token"] = token;
            data["role"] = user.role;
            data["username"] = user.username;
            data["token_type"] = "Bearer";

            auto resp = api_resp::makeJson(0, "login success", data);
            sendJsonAndLog(req, std::move(callback), resp);
        },
        {Post}
    );

    // POST /api/auth/logout
    app().registerHandler(
        "/api/auth/logout",
        [&ctx](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            /*
                1.requireAuth(...)
                2.提取 token
                3.调 ctx.tokenProvider->revokeToken(...)
                4.返回 { logged_out: true }
            */
            if (!requireAuth(req, callback, ctx)) return;

            auto tokenOpt = extractBearerToken(req);
            if (tokenOpt.has_value()) {
                ctx.tokenProvider->revokeToken(*tokenOpt);
            }

            Json::Value data(Json::objectValue);
            data["logged_out"] = true;

            auto resp = api_resp::makeJson(0, "logout success", data);
            sendJsonAndLog(req, std::move(callback), resp);
        },
        {Post}
    );

    // GET /api/auth/me
    app().registerHandler(
        "/api/auth/me",
        [&ctx](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            /*
                1.requireAuth(...)
                2.getIdentity(req, ctx)
                3.返回当前用户身份信息
            */
            if (!requireAuth(req, callback, ctx)) return;

            auto identityOpt = getIdentity(req, ctx);
            if (!identityOpt.has_value()) {
                auto resp = api_resp::makeJson(
                    40101, "Unauthorized", Json::Value(Json::objectValue), k401Unauthorized);
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            Json::Value data(Json::objectValue);
            data["username"] = identityOpt->username;
            data["role"] = identityOpt->role;

            auto resp = api_resp::makeJson(0, "success", data);
            sendJsonAndLog(req, std::move(callback), resp);
        },
        {Get}
    );
}

}  // namespace controllers