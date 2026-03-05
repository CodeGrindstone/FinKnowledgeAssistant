#include "controllers/QaController.h"
#include <drogon/drogon.h>

#include "auth/AuthMiddleware.h"
#include "common/ApiResponse.h"
#include "common/Logger.h"
#include "services/QaService.h"

using namespace drogon;

namespace controllers {

void registerQaRoutes(AppContext &ctx) {
    /*
    功能：注册并实现问答接口 /api/qa/ask，是该项目的核心接口
    */
    // POST /api/qa/ask
    // body:
    // {
    //   "question": "什么是市盈率？"
    // }
    app().registerHandler(
        "/api/qa/ask",
        [&ctx](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            /*
                1.requireAuth(...)
                2.解析 JSON body
                3.校验 question 是否存在
                4.校验长度上限（如 >4000）
                5.获取当前用户身份（用于传给 Python）
                6.读取 X-Request-Id（trace_id）
                7.调 QaService
                8.返回统一响应
            */
            if (!requireAuth(req, callback, ctx)) return;

            auto json = req->getJsonObject();
            if (!json) {
                auto resp = api_resp::makeJson(
                    40001,
                    "Invalid JSON body",
                    Json::Value(Json::objectValue),
                    k400BadRequest
                );
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            const std::string question = json->get("question", "").asString();
            if (question.empty()) {
                auto resp = api_resp::makeJson(
                    40010,
                    "question is required",
                    Json::Value(Json::objectValue),
                    k400BadRequest
                );
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            // 可选：简单长度限制（防止太长）
            if (question.size() > 4000) {
                auto resp = api_resp::makeJson(
                    40011,
                    "question is too long",
                    Json::Value(Json::objectValue),
                    k400BadRequest
                );
                sendJsonAndLog(req, std::move(callback), resp);
                return;
            }

            // auto data = services::QaService::askMock(question, ctx);

            auto identityOpt = getIdentity(req, ctx);
            std::string userId = identityOpt.has_value() ? identityOpt->username : "unknown";
            
            std::string traceId = req->getHeader("X-Request-Id");
            if (traceId.empty()) traceId = "trace-local";

            auto data = services::QaService::askByPythonRag(question, userId, traceId, ctx);

            auto resp = api_resp::makeJson(0, "success", data);
            sendJsonAndLog(req, std::move(callback), resp);
        },
        {Post}
    );
}

}  // namespace controllers