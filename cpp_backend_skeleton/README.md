该md文档专门用来阐述后端开发流程和未来工作安排
# 后续修改时的“动刀指南”
## 新增一个API接口
若要新增一个API接口（比如/api/kb/list）
* 新建 controllers/kbController.h/.cpp
* (需要时)新建 services/kbService.h/.cpp
* 在 RouteRegistrar.cpp 中注册 registerKbRoutes(ctx)

## 把MockToken 换成JWT
* 新建 auth/JwtTokenProvider.h/.cpp
* 实现 ITokenProvider
* 在 main.cpp 中把 ctx.tokenProvider = std::make_shared<MockTokenProvider>() 改成 JWT 版本

## 把系统配置从内存改成 MySQL

* 新建 ConfigRepository / ConfigService
* SystemController 不变或少量改动
* AppContext 里加 repository/service
* QaService 继续从统一配置来源读取参数

# 把 /api/qa/ask 接真实 RAG
* 优先改 QaService / PythonRagClient
* QaController 尽量只做参数校验，不写复杂逻辑

# 调整统一响应格式
* common/ApiResponse.h
*全项目接口都会受影响（要谨慎）