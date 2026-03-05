#include "controllers/HealthController.h"
#include <drogon/drogon.h>
#include "common/ApiResponse.h"
#include "common/Logger.h"

using namespace drogon;

namespace controllers {

void registerHealthRoutes(AppContext &ctx) {
    /*
    功能：注册并实现 /api/health
    */
    (void)ctx;

    app().registerHandler(
        "/api/health",
        [](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            Json::Value data(Json::objectValue);
            data["status"] = "ok";
            data["service"] = "financial_qa_backend_mock";
            data["version"] = "0.1.0";

            std::optional<std::string> requestId = std::nullopt;
            auto rid = req->getHeader("X-Request-Id");
            if (!rid.empty()) requestId = rid;

            auto resp = api_resp::makeJson(0, "success", data, k200OK, requestId);
            sendJsonAndLog(req, std::move(callback), resp);
        },
        {Get}
    );
}

}  // namespace controllers