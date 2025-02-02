#ifndef CLUBMOSS_KEY_COST_HXX
#define CLUBMOSS_KEY_COST_HXX

#include "key_cost_data.hxx"

namespace clubmoss::metric {

// 击键代价 //
class KeyCost {
public:
    explicit KeyCost(const key_cost::Data& data);

    auto measure(const Layout&) -> fz;
    auto analyze(const Layout&) -> std::pair<fz, uz>;
    auto scan(const Layout&, Toml& stats) -> std::pair<fz, uz>;

    KeyCost() = delete;

protected:
    fz cost_{0.0}; // 代价
    uz flaw_count_{0}; // 缺陷

    std::array<fz, KEY_COUNT> heat_map_{0.0}; // 热力图
    std::array<fz, ROW_COUNT> row_usage_{0.0}; // 行使用率
    std::array<fz, COL_COUNT> col_usage_{0.0}; // 列使用率

    std::array<fz, Finger::_size()> finger_usage_{0.0}; // 每个手指的使用率

    fz similarity_{0.0}; // 与标准布局的相似度

    auto calcFingerUsage(const Layout&) noexcept -> void;

    key_cost::Data data_;

private:
    inline static Config& cfg_ = Config::getInstance();

    auto validateFingerHandUsage() noexcept -> void;

    friend class clubmoss::Evaluator;
};

}

#endif // CLUBMOSS_KEY_COST_HXX
