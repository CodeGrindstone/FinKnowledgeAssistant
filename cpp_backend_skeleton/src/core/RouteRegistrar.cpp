#include "core/RouteRegistrar.h"
#include "controllers/HealthController.h"
#include "controllers/AuthController.h"
#include "controllers/SystemController.h"
#include "controllers/QaController.h"

namespace core {

void registerAllRoutes(AppContext &ctx) {
    /*
    统一注册所有API路由，并绑定回调函数
    未来如果要添加新的API路由，只需要在src/controllers下新建XxxController并在这里注册对应函数即可
    */
    controllers::registerHealthRoutes(ctx);
    controllers::registerAuthRoutes(ctx);
    controllers::registerSystemRoutes(ctx);
    controllers::registerQaRoutes(ctx);
}

}  // namespace core