#pragma once
#include <json/json.h>
#include <string>
#include "core/AppContext.h"

namespace services {

class QaService {
    /*
    功能：封装问答业务逻辑，不让Controller直接依赖具体实现细节
    ***未来会在这里完成：***
        1.调Python RAG
        2.记录问答日志
        3.结果后处理（敏感词过滤/格式优化）
        4.置信度阈值判断
        5.降级策略
    */
public:
    // mock 版本：后续替换为真实 RAG 服务调用
    static Json::Value askMock(const std::string &question, AppContext &ctx);

    // 优先调用 Python RAG，失败时可回退 mock
    static Json::Value askByPythonRag(const std::string &question,
                                      const std::string &userId,
                                      const std::string &traceId,
                                      AppContext &ctx);
};

}  // namespace services