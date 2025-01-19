#include "layout_area.hxx"

namespace clubmoss::layout {

auto size_of_area = [](const Toml& config) -> uz {
    return config.at("cap_list").size();
};

auto even_floor = [](const uz n) -> uz {
    return n % 2 == 0 ? n : n - 1;
};

/**
 * @note 假设布局配置合法.
 **/
Area::Area(const Toml& config)
    : Area(size_of_area(config)) {
    for (const Toml& item : config.at("cap_list").as_array()) {
        const auto cap = static_cast<Cap>(item.as_string()[0]);
        cap_list_.emplace_back(cap);
    }
    for (const Toml& item : config.at("pos_list").as_array()) {
        const auto pos = static_cast<Pos>(item.as_integer());
        pos_list_.emplace_back(pos);
    }
    // 排序[键值列表], 以便进行比较
    std::ranges::sort(cap_list_);
}

Area::Area(const uz size)
    : size_(size), ths_(even_floor(size)), idx_(ths_ + 1) {
    assert(size_ <= KEY_COUNT); // 大小不应超过按键数量
    cap_list_.reserve(size);
    pos_list_.reserve(size);
}

Area::Area(const Area& other)
    : cap_list_(other.cap_list_),
      pos_list_(other.pos_list_),
      size_(other.size_),
      ths_(other.ths_),
      idx_(ths_ + 1) {}

Area& Area::operator=(const Area& rhs) {
    if (this == &rhs) { return *this; }
    cap_list_ = rhs.cap_list_;
    pos_list_ = rhs.pos_list_;
    size_ = rhs.size_;
    ths_ = rhs.ths_;
    idx_ = ths_ + 1;
    return *this;
}

/**
 * @brief 随机分配区域内的所有按键.
 * @param layout: 待修改的[键盘布局]对象.
 * @param prng: 符合 C++11 标准的随机数引擎.
 * @note 假定 layout 合法且与当前区域兼容.
 **/
auto Area::assign(Layout& layout, Prng& prng) noexcept -> void {
    std::ranges::shuffle(pos_list_, prng); // 随机构建新的<键值, 键位>映射
    for (const auto [cap, pos] : std::views::zip(cap_list_, pos_list_)) {
        layout.setKey(cap, pos);
    }
}

/**
 * @brief 随机交换区域内的两个按键.
 * @param layout: 待修改的[键盘布局]对象.
 * @param prng: 符合 C++11 标准的随机数引擎.
 * @note 假定 layout 合法且与当前区域兼容.
 **/
auto Area::mutate(Layout& layout, Prng& prng) noexcept -> void {
    // 为了提高效率, 随机抽取两个[键位]的实现其实是: 从打乱的[键位列表]中
    // 依次取出两个[键位], 并在经过一定次数后重新打乱[键位列表]. 因此,
    // idx_ 被初始化为 ths_ + 1, 以便在第一次调用时触发更新.
    if (idx_ >= ths_) {
        std::ranges::shuffle(pos_list_, prng);
        idx_ = 0;
    }
    // 从经过随机化的[键位列表]中依次取出两个[键位],
    // 在[键盘布局]对象中交换这两个位置上的[键值].
    const Pos pos1 = pos_list_[idx_++];
    const Pos pos2 = pos_list_[idx_++];
    layout.swap2Keys(pos1, pos2);
}

/**
 * @brief 检测布局是否与当前区域兼容.
 * @param layout 待测[键盘布局]对象.
 * @return 若适配, 则返回真, 否则返回假.
 * @note 假定 layout 对象已经被正确初始化.
 **/
auto Area::isSafeFor(const Layout& layout) const noexcept -> bool {
    // 若区域内的所有[键值]都能在布局的相应位置中找到对应的对象, 不论顺序,
    // 则assign()和mutate()操作不会损坏对象, 返回真; 否则, 返回假.
    std::vector<Cap> observed_caps;
    for (const Pos pos : pos_list_) {
        observed_caps.emplace_back(
            layout.getCap(pos)
        );
    }
    // 排序在 layout 中被统计到的所有按键, 以消除顺序影响.
    std::ranges::stable_sort(observed_caps);
    return observed_caps == cap_list_;
}

}
