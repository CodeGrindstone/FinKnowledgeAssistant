#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <exception>

#include "core/AppContext.h"
#include "core/RouteRegistrar.h"
#include "auth/MockTokenProvider.h"

using namespace drogon;

int main() {
    // 1) 初始化应用上下文（后续可接 MySQL / Redis / JWT）
    AppContext ctx;
    ctx.tokenProvider = std::make_shared<MockTokenProvider>();

    ctx.users["admin"] = MockUser{"admin", "admin123", "admin"};
    ctx.users["user"]  = MockUser{"user",  "user123",  "user"};

    // 2) 日志
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    spdlog::set_level(spdlog::level::info);
    app().setLogLevel(trantor::Logger::kWarn);

    // 3) 全局异常处理
    app().setExceptionHandler(
        [](const std::exception &e,
           const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback) {
            spdlog::error("Unhandled exception: path={} what={}", req->path(), e.what());
            Json::Value data(Json::objectValue);
            
            Json::Value root(Json::objectValue);
            root["code"] = 50000;
            root["message"] = "Internal Server Error";
            root["data"] = data;

            auto resp = HttpResponse::newHttpJsonResponse(root);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        });

    // 4) 注册所有路由
    core::registerAllRoutes(ctx);

    // 5) 启动服务
    app().addListener("0.0.0.0", 8080);
    spdlog::info("Server started at http://0.0.0.0:8080");
    app().run();

    return 0;
}