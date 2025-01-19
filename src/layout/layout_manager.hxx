#ifndef CLUBMOSS_LAYOUT_MANAGER_HXX
#define CLUBMOSS_LAYOUT_MANAGER_HXX

#include "layout_config.hxx"

namespace clubmoss::layout {

// 布局管理器 //
class Manager final {
public:
    Manager();
    Manager(const Manager&);
    Manager& operator=(const Manager&);

    static auto loadCfg(const Toml& cfg) -> void;

    auto create() noexcept -> Layout;
    auto reinit(Layout& layout) noexcept -> void;
    auto mutate(Layout& child, const Layout& parent) noexcept -> void;

    [[nodiscard]] auto canManage(const Layout& layout) const noexcept -> bool;

protected:
    std::vector<Area> mutable_areas_; // 可变区域列表
    std::vector<Key> pinned_keys_; // 固定按键列表
    std::vector<uz> area_ids_; // 区域编号列表

    Prng prng_{}; // 随机数生成器

    bool need_to_select_area_; // 是否存在多个[可变区域]需要进行抽取
    bool have_pinned_key_; // 是否存在[固定按键]

    uz ths_; // 状态更新的频率阈值
    uz idx_; // 当前选取的区域ID的索引

    auto randomlySelectAnArea() noexcept -> Area&;

private:
    inline static Config& cfg_ = Config::getInstance();

    auto assignFixedKeys(Layout& layout) noexcept -> void;
    auto assignMutableKeys(Layout& layout) noexcept -> void;
};

}

#endif //CLUBMOSS_LAYOUT_MANAGER_HXX
