#pragma once
#include <cstdint>

namespace warmup {
// One scoped owner of console formatting; does not own/close the standard handle.
class Console {
public:
    Console();
    ~Console() noexcept;
    Console(const Console&) = delete;

    Console& operator=(const Console&) = delete;

    void setHighlight(bool highlighted);
    void clear();

private:
    void* handle_ = nullptr;

    std::uint16_t originalAttributes_ = 0;
    unsigned int originalInputPage_ = 0;
    unsigned int originalOutputPage_ = 0;
    bool available_ = false;
    bool highlighted_ = false;
};
}
