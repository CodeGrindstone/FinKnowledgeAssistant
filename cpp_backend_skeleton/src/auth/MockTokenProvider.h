#pragma once
#include "auth/TokenProvider.h"
#include <mutex>
#include <unordered_map>

/*
    该文件仅用作测试，后续会被JwtTokenProvider.*替代
*/
class MockTokenProvider final : public ITokenProvider {
public:
    MockTokenProvider();

    std::string issueToken(const std::string &username, const std::string &role) override;
    std::optional<Identity> verifyToken(const std::string &token) override;
    void revokeToken(const std::string &token) override;

private:
    std::mutex mu_;
    std::unordered_map<std::string, Identity> tokenMap_;
};