#pragma once
#include <string>
/*
    定义用户和身份相关的数据结构
*/
struct MockUser {
    /*
        用于mock登录校验
    */
    std::string username;
    std::string password;
    std::string role;  // admin / user
};

struct Identity {
    /*
        用于token校验成功后返回“当前身份信息”
    */
    std::string username;
    std::string role;
};