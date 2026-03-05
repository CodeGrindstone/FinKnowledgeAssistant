#include "auth/MockTokenProvider.h"

MockTokenProvider::MockTokenProvider() {
    std::lock_guard<std::mutex> lock(mu_);
    tokenMap_["mock-admin-token"] = Identity{"admin", "admin"};
    tokenMap_["mock-user-token"]  = Identity{"user", "user"};
}

std::string MockTokenProvider::issueToken(const std::string &username, const std::string &role) {
    /*
        根据用户名和角色返回固定token
    */
    if (username == "admin" && role == "admin") return "mock-admin-token";
    if (username == "user"  && role == "user")  return "mock-user-token";
    return "mock-token";
}

std::optional<Identity> MockTokenProvider::verifyToken(const std::string &token) {
    /*
        从tokenMap_查找身份
    */
    std::lock_guard<std::mutex> lock(mu_);
    auto it = tokenMap_.find(token);
    if (it == tokenMap_.end()) return std::nullopt;
    return it->second;
}

void MockTokenProvider::revokeToken(const std::string &token) {
    // mock 阶段默认不失效，方便测试
    (void)token;
}