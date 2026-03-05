#pragma once
#include <drogon/drogon.h>
#include <optional>
#include <string>
#include "core/AppContext.h"

/*
    集中管理：
        1.从请求头提取 Bearer Token
        2.获取当前用户身份
        3.登录校验
        4.管理员权限校验
*/

std::optional<std::string> extractBearerToken(const drogon::HttpRequestPtr &req);
std::optional<Identity> getIdentity(const drogon::HttpRequestPtr &req, AppContext &ctx);

bool requireAuth(const drogon::HttpRequestPtr &req,
                 std::function<void(const drogon::HttpResponsePtr &)> &callback,
                 AppContext &ctx);

bool requireAdmin(const drogon::HttpRequestPtr &req,
                  std::function<void(const drogon::HttpResponsePtr &)> &callback,
                  AppContext &ctx);