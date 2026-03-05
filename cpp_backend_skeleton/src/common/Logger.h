#pragma once
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <functional>

/*
    统一记录请求日志，并封装“记录日志 + 返回响应”的重复逻辑
*/
inline void logResponse(const drogon::HttpRequestPtr &req,
                        const drogon::HttpResponsePtr &resp) {
    /*
    打印日志（路径、方法、状态码）
    */
    spdlog::info("path={} method={} status={}",
                 req->path(),
                 req->methodString(),
                 static_cast<int>(resp->statusCode()));
}

inline void sendJsonAndLog(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> callback,
                           const drogon::HttpResponsePtr &resp) {
    logResponse(req, resp);
    callback(resp);
}