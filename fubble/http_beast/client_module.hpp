#ifndef UUID_26340D53_7656_4F7F_8B94_036831E343ED
#define UUID_26340D53_7656_4F7F_8B94_036831E343ED

#include <fubble/http2/module.hpp>
#include <fubble/utils/executor_module.hpp>

namespace fubble::http_beast {
std::unique_ptr<http2::factory>
create_factory(std::shared_ptr<utils::executor_module> executor);
} // namespace fubble::http_beast

#endif
