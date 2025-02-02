#ifndef CLUBMOSS_LAYOUT_AREA_HXX
#define CLUBMOSS_LAYOUT_AREA_HXX

#include "layout.hxx"

namespace clubmoss::layout {

// 可变区域 //
class Area final {
public:
    explicit Area(const Toml& config);

    Area(const Area&);
    Area& operator=(const Area&);

    Area() = delete;

    auto assign(Layout& layout, Prng& prng) noexcept -> void;
    auto mutate(Layout& layout, Prng& prng) noexcept -> void;

    [[nodiscard]] auto isSafeFor(const Layout& layout) const noexcept -> bool;

protected:
    std::vector<Cap> cap_list_{}; // 键值列表, 升序排列
    std::vector<Pos> pos_list_{}; // 键位列表, 随机打乱

    uz size_; // 可变区域的大小
    uz ths_; // 状态更新的频率阈值
    uz idx_; // 当前选取的键位的索引

private:
    explicit Area(uz size);

    friend class Config;
};

}

#endif //CLUBMOSS_LAYOUT_AREA_HXX
