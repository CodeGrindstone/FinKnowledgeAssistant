#pragma once


/*
    定义系统配置项的数据结构（当前用内存mock维护）
*/

struct SystemConfig {
    bool enable_multimodal = true;
    int top_k = 5;
    double temperature = 0.2;
    bool enable_web_search = false;
};