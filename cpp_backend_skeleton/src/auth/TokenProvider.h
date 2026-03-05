#pragma once
#include <optional>
#include <string>
#include "models/UserModels.h"

class ITokenProvider {
    /*
        定义Token能力的抽象接口（解耦mock与真实JWT）
    */
public:
    virtual ~ITokenProvider() noexcept = default;

    //登陆成功后签发token
    virtual std::string issueToken(const std::string &username, const std::string &role) = 0;
    // 校验token，返回Identity 
    virtual std::optional<Identity> verifyToken(const std::string &token) = 0;
    //注销token
    virtual void revokeToken(const std::string &token) = 0;
};