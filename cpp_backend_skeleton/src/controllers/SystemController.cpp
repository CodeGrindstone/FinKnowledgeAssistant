#include "controllers/SystemController.h"
#include <drogon/drogon.h>

#include "auth/AuthMiddleware.h"
#include "common/ApiResponse.h"
#include "common/Logger.h"

using namespace drogon;

namespace controllers {

void registerSystemRoutes(AppContext &ctx) {
    /*
    功能：注册并实现系统配置接口：
            * GET /api/system/config
            * PUT /api/system/config
    */
    app().registerHandler(
        "/api/system/config",
        [&ctx](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            /*
                1.requireAuth(...)
                2.加锁读取 ctx.systemConfig
                3.返回配置字段
            */
            if (!requireAuth(req, callback, ctx)) return;

            Json::Value data(Json::objectValue);
            {
                std::lock_guard<std::mutex> lock(ctx.systemConfigMu);
                data["enable_multimodal"] = ctx.systemConfig.enable_multimodal;
                data["top_k"] = ctx.systemConfig.top_k;
                data["temperature"] = ctx.systemConfig.temperature;
                data["enable_web_search"] = ctx.systemConfig.enable_web_search;
            }

            auto resp = api_resp::makeJson(0, "success", data);
            sendJsonAndLog(req, std::move(callback), resp);
        },
        {Get}
    );

    // PUT /api/system/config
    app().registerHandler(
        "/api/system/config",
        [&ctx](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            /*
                1.requireAdmin(...)
                2.解析 Json body
                3.加锁更新 ctx.systemConfig（支持部分字段更新）
                4.返回 {update:true}
            */
            if (!requireAdmin(req, callback, ctx)) return;

            auto json = req->getJsonObject();
            if (!json) {
                auto resp = api_resp::makeJson(
                    40001, "Invalid JSON body", Json::Value(Json::objectValue), k400BadRequest);
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            {
                std::lock_guard<std::mutex> lock(ctx.systemConfigMu);

                if (json->isMember("enable_multimodal")) {
                    ctx.systemConfig.enable_multimodal = (*json)["enable_multimodal"].asBool();
                }
                if (json->isMember("top_k")) {
                    ctx.systemConfig.top_k = (*json)["top_k"].asInt();
                }
                if (json->isMember("temperature")) {
                    ctx.systemConfig.temperature = (*json)["temperature"].asDouble();
                }
                if (json->isMember("enable_web_search")) {
                    ctx.systemConfig.enable_web_search = (*json)["enable_web_search"].asBool();
                }
            }

            Json::Value data(Json::objectValue);
            data["updated"] = true;

            auto resp = api_resp::makeJson(0, "config updated (mock)", data);
            sendJsonAndLog(req, std::move(callback), resp);
        },
        {Put}
    );
}

}  // namespace controllers