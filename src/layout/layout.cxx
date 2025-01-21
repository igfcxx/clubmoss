#include "layout.hxx"

namespace clubmoss {

using Marks = std::bitset<MAX_KEY_CODE>;

/**
 * @brief 仅供内部使用，用于构造一个未初始化的 Layout 对象.
 * @note 直接操作未初始化的 Layout 可能导致未定义行为.
 **/
Layout::Layout() = default;

/**
 * @brief 根据按键序列构造 Layout 对象.
 * @param seq 按键序列 (字符串, 其内容为所有合法键值的一个排列. 按键顺序为从左到右,
 *            从上到下, 行优先. 例如 "QWERTYUIOPASDFGHJKL;ZXCVBNM,./".
 **/
Layout::Layout(const std::string_view seq) {
    validateSeq(seq);
    loadFromSeq(seq);
}

auto Layout::validateSeq(const std::string_view seq) -> void {
    // 检验序列长度是否为 KEY_COUNT
    if (const uz len = seq.length(); len != KEY_COUNT) {
        constexpr auto what{"expect seq length = {:d}, got {:d}"};
        throw IllegalSeq(std::format(what, KEY_COUNT, len), seq);
    }
    // 检验[键值]合法性
    for (const char ch : seq) {
        if (not Utils::isLegalCap(ch)) {
            constexpr auto what{"illegal key code '{:c}'"};
            throw IllegalSeq(std::format(what, ch), seq);
        }
    }
    // 检验[键值]唯一性
    Marks observed_caps(0);
    for (const char ch : seq) {
        if (observed_caps[ch]) {
            constexpr auto what{"duplicate keys '{:c}'"};
            throw IllegalSeq(std::format(what, ch), seq);
        }
        observed_caps.set(ch);
    }
}

void Layout::loadFromSeq(const std::string_view str) {
    for (const auto& [i, cap] : str | std::views::enumerate) {
        setKey(static_cast<Cap>(cap), static_cast<Pos>(i));
    }
}

auto Layout::getCap(const Pos pos) const noexcept -> Cap {
    assert(Utils::isLegalPos(pos));
    return key_map_[pos];
}

auto Layout::getPos(const Cap cap) const noexcept -> Pos {
    assert(Utils::isLegalCap(cap));
    return key_map_[cap];
}

auto Layout::toString() const noexcept -> std::string {
    auto caps = key_map_ | std::views::take(KEY_COUNT);
    return {caps.begin(), caps.end()};
}

/**
 * @brief 设置按键.
 * @param cap 键值.
 * @param pos 键位.
 * @note 可能导致未定义行为.
 **/
auto Layout::setKey(const Cap cap, const Pos pos) noexcept -> void {
    assert(Utils::isLegalCap(cap));
    assert(Utils::isLegalPos(pos));
    key_map_[cap] = pos;
    key_map_[pos] = cap;
}

/**
 * @brief 交换两个按键.
 * @param pos1 第一个键位.
 * @param pos2 第二个键位.
 **/
auto Layout::swap2Keys(const Pos pos1, const Pos pos2) noexcept -> void {
    assert(Utils::isLegalPos(pos1));
    assert(Utils::isLegalPos(pos2));
    std::swap(key_map_[pos1], key_map_[pos2]);
    std::swap(key_map_[getCap(pos1)], key_map_[getCap(pos2)]);
}

auto Layout::operator<=>(const Layout& other) const noexcept -> std::strong_ordering {
    auto keys = std::views::zip(
        this->key_map_ | std::views::take(KEY_COUNT),
        other.key_map_ | std::views::take(KEY_COUNT)
    );
    for (const auto [this_cap, other_cap] : keys) {
        if (this_cap < other_cap) { return std::strong_ordering::less; }
        if (this_cap > other_cap) { return std::strong_ordering::greater; }
    }
    return std::strong_ordering::equivalent;
}

auto Layout::operator==(const Layout& other) const noexcept -> bool {
    return this->key_map_ == other.key_map_;
}

auto Layout::isValid() const noexcept -> bool {
    return areElemsLegal() and areKeysUnique();
}

auto Layout::areElemsLegal() const noexcept -> bool {
    for (const Pos pos : POS_SET) { // 检验[键值]合法性
        if (not Utils::isLegalCap(getCap(pos))) {
            return false;
        }
    }
    for (const Cap cap : CAP_SET) { // 检验[键位]合法性
        if (not Utils::isLegalPos(getPos(cap))) {
            return false;
        }
    }
    return true;
}

auto Layout::areKeysUnique() const noexcept -> bool {
    Marks observed(0);
    // 检验[键位集]对应[键值]的唯一性
    for (const Pos pos : POS_SET) {
        const Cap cap = getCap(pos);
        if (observed[cap]) return false;
        observed.set(cap);
    }
    // 检验[键值集]对应[键位]的唯一性
    for (const Cap cap : CAP_SET) {
        const Pos pos = getPos(cap);
        if (observed[pos]) return false;
        observed.set(pos);
    }
    return true;
}

}
