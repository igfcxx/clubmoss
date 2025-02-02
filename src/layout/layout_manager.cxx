#include "layout_manager.hxx"

namespace clubmoss::layout {

Manager::Manager()
    : mutable_areas_(cfg_.mutable_areas_),
      pinned_keys_(cfg_.pinned_keys_),
      area_ids_(cfg_.area_ids_),
      need_to_select_area_(cfg_.num_areas_ > 1),
      have_pinned_key_(cfg_.num_pinned_keys_ > 0),
      ths_(cfg_.num_mutable_keys_), idx_(ths_ + 1) {
    prng_.seed(std::random_device()());
}

Manager::Manager(const Manager& other)
    : mutable_areas_(cfg_.mutable_areas_),
      pinned_keys_(cfg_.pinned_keys_),
      area_ids_(cfg_.area_ids_),
      need_to_select_area_(other.need_to_select_area_),
      have_pinned_key_(other.have_pinned_key_),
      ths_(other.ths_), idx_(ths_ + 1) {
    prng_.seed(std::random_device()());
}

Manager& Manager::operator=(const Manager& rhs) {
    if (this != &rhs) {
        mutable_areas_ = cfg_.mutable_areas_;
        pinned_keys_ = cfg_.pinned_keys_;
        area_ids_ = cfg_.area_ids_;

        prng_.seed(std::random_device()());

        need_to_select_area_ = rhs.need_to_select_area_;
        have_pinned_key_ = rhs.have_pinned_key_;
        ths_ = rhs.ths_;
        idx_ = ths_ + 1;
    }
    return *this;
}

auto Manager::loadCfg(const Toml& cfg) -> void {
    cfg_.loadCfg(cfg);
}

auto Manager::create() noexcept -> Layout {
    Layout layout;
    assignFixedKeys(layout);
    assignMutableKeys(layout);
    assert(layout.isValid());
    return layout;
}

auto Manager::assignFixedKeys(Layout& layout) noexcept -> void {
    if (not have_pinned_key_) { return; }
    for (const auto& [cap, pos] : pinned_keys_) {
        layout.setKey(cap, pos);
    }
}

auto Manager::assignMutableKeys(Layout& layout) noexcept -> void {
    for (Area& area : mutable_areas_) {
        area.assign(layout, prng_);
    }
}

/**
 * @brief 随机打乱所有[可变按键].
 * @param layout: 待修改的[键盘布局]对象.
 * @note 假定 layout 合法且与当前设置兼容.
 **/
auto Manager::reinit(Layout& layout) noexcept -> void {
    assert(canManage(layout));
    assignMutableKeys(layout);
    assert(layout.isValid());
}

/**
 * @brief 使布局产生一个微小的突变.
 * @param child: 待修改的[键盘布局]对象.
 * @param parent: 作为参照的[键盘布局]对象.
 * @note 假定 parent 合法且与当前设置兼容; 对 child 无要求.
 **/
auto Manager::mutate(Layout& child, const Layout& parent) noexcept -> void {
    assert(parent.isValid());
    assert(canManage(parent));
    // 先复制 parent 布局, 再随机突变
    child.key_map_ = parent.key_map_;
    // 随机选择一个[可变区域], 交换其中的一对按键
    randomlySelectAnArea().mutate(child, prng_);
    assert(child.isValid());
}

auto Manager::randomlySelectAnArea() noexcept -> Area& {
    // 若仅有一个[可变区域], 则无需选择, 直接返回.
    if (not need_to_select_area_) {
        return mutable_areas_[0];
    }
    // 为了提高效率, 并使得每一个按键被选中的概率尽可能地接近于均匀分布,
    // 随机抽取一个[可变区域]的具体实现其实是: 从打乱的[区域编号列表]中
    // 依次取出[区域编号], 并在经过一定次数后重新打乱[区域编号列表].
    // 因此 idx_ 被初始化为 ths_ + 1, 以便在第一次调用时触发更新.
    if (idx_ >= ths_) {
        std::ranges::shuffle(area_ids_, prng_);
        idx_ = 0;
    }
    // 根据抽取的[区域编号], 返回对应的[可变区域].
    const uz random_id = area_ids_[idx_++];
    return mutable_areas_[random_id];
}

/**
 * @brief 检测布局是否与当前设置兼容.
 * @param layout 待测[键盘布局]对象.
 * @return 若适配, 则返回真, 否则返回假.
 * @note 假定 layout 对象已经被正确初始化.
 **/
auto Manager::canManage(const Layout& layout) const noexcept -> bool {
    auto is_compatible = [&layout](const Area& area) -> bool {
        return area.isSafeFor(layout);
    };
    // 检验是否所有[可变区域]都能兼容 layout.
    return std::ranges::all_of(mutable_areas_, is_compatible);
}

}
