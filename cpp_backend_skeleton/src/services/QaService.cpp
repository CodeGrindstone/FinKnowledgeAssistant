#include "services/QaService.h"
#include "services/PythonRagClient.h"

namespace services {

Json::Value QaService::askMock(const std::string &question, AppContext &ctx) {
    (void)ctx;  // 目前 mock 阶段未使用，后续可用来读系统配置、用户上下文等

    Json::Value data(Json::objectValue);

    // mock answer
    std::string answer = "【Mock回答】你问的是：";
    answer += question;
    answer += "。当前为演示阶段，后续将由 Python LangChain RAG 服务返回真实答案。";

    data["answer"] = answer;
    data["model"] = "mock-rag-engine";
    data["latency_ms"] = 12;
    data["confidence"] = 0.86;

    // 预留引用列表（后续接知识库检索结果）
    Json::Value refs(Json::arrayValue);
    {
        Json::Value r1(Json::objectValue);
        r1["title"] = "金融知识库示例文档";
        r1["source"] = "mock_kb";
        r1["chunk_id"] = "doc-001#chunk-01";
        r1["score"] = 0.92;
        refs.append(r1);
    }
    {
        Json::Value r2(Json::objectValue);
        r2["title"] = "市场术语说明示例";
        r2["source"] = "mock_kb";
        r2["chunk_id"] = "doc-002#chunk-03";
        r2["score"] = 0.88;
        refs.append(r2);
    }
    data["references"] = refs;

    return data;
}

Json::Value QaService::askByPythonRag(const std::string &question,
                                      const std::string &userId,
                                      const std::string &traceId,
                                      AppContext &ctx) {
    /*
        1.从 ctx.systemConfig 读取 top_k / temperature
        2.调 PythonRagClient::ask(...)
        3.成功则返回 Python RAG 的data
        4.失败则fallback到 askMock(...)，并附加 fallback_reason
    */


    // 这里先写死 Python 服务地址；后续可放到 systemConfig 或 config 文件中
    const std::string pythonRagBaseUrl = "http://192.168.10.90:8000";

    int topK = 5;
    double temperature = 0.2;
    {
        std::lock_guard<std::mutex> lock(ctx.systemConfigMu);
        topK = ctx.systemConfig.top_k;
        temperature = ctx.systemConfig.temperature;
    }

    std::string errMsg;
    auto dataOpt = PythonRagClient::ask(
        pythonRagBaseUrl,
        question,
        userId,
        traceId,
        topK,
        temperature,
        errMsg
    );

    if (dataOpt.has_value()) {
        return *dataOpt;
    }

    // Python 服务失败时回退 mock，并附加提示字段，方便调试与演示
    auto fallback = askMock(question, ctx);
    fallback["fallback"] = true;
    fallback["fallback_reason"] = errMsg.empty() ? "python rag unavailable" : errMsg;
    return fallback;
}


}  // namespace services