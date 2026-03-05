#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

#include "auth/TokenProvider.h"
#include "models/SystemConfig.h"
#include "models/UserModels.h"

struct AppContext {
    /*
    全局共享上下文，多个模块需要用到同一份用户数据、配置、token校验器
    AppContext就是把这些共享资源集中起来，避免到处写全局变量。
    简单理解就是：存储多个模块都会使用到的全局变量
    */
    std::shared_ptr<ITokenProvider> tokenProvider;

    std::unordered_map<std::string, MockUser> users;

    SystemConfig systemConfig;
    std::mutex systemConfigMu;
};