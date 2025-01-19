#ifndef CLUBMOSS_LAYOUT_HXX
#define CLUBMOSS_LAYOUT_HXX

#include "../common/utils.hxx"

namespace clubmoss {

namespace layout {
    class Manager;
    class Area;
}

// 键盘布局 //
class Layout {
public:
    explicit Layout(std::string_view seq);

    [[nodiscard]] auto getCap(Pos) const noexcept -> Cap;
    [[nodiscard]] auto getPos(Cap) const noexcept -> Pos;

    [[nodiscard]] auto toCapSeq() const noexcept -> std::string;
    [[nodiscard]] auto isValid() const noexcept -> bool;

    auto operator<=>(const Layout& other) const noexcept -> std::strong_ordering;
    auto operator==(const Layout& other) const noexcept -> bool;

protected:
    // 按键列表, 存储了[键值]与[键位]的双向映射
    std::array<u8, MAX_KEY_CODE> key_map_{};

    Layout();

    auto setKey(Cap, Pos) noexcept -> void;
    auto swap2Keys(Pos, Pos) noexcept -> void;

    auto loadFromSeq(std::string_view str) -> void;

private:
    static auto validateSeq(std::string_view seq) -> void;

    [[nodiscard]] auto areElemsLegal() const noexcept -> bool;
    [[nodiscard]] auto areKeysUnique() const noexcept -> bool;

    class IllegalSeq final : public FatalError {
    public:
        IllegalSeq() = delete;

        IllegalSeq(const std::string_view msg, const std::string_view seq) noexcept
            : FatalError(std::format(WHAT, seq, msg)) {}

    private:
        static constexpr auto WHAT{"Illegal layout \"{:s}\"\n{:s}"};
    };

    friend class layout::Manager;
    friend class layout::Area;
};

namespace layout::baselines {

    class Baseline : public Layout {
    public:
        Baseline(const std::string_view seq, const std::string_view name)
            : Layout(seq), name(name) {}

        const std::string name;
    };

    // @formatter:off //
    // 部分布局使用'/'替代了不在键值集中的字符
    static const auto QWERTY   = Baseline("QWERTYUIOPASDFGHJKL;ZXCVBNM,./", "QWERTY");
    static const auto DVORAK   = Baseline(";,.PYFGCRLAOEUIDHTNS/QJKXBMWVZ", "Dvorak");
    static const auto COLEMAK  = Baseline("QWFPGJLUY;ARSTDHNEIOZXCVBKM,./", "Colemak");
    static const auto MINIMAK  = Baseline("QWDFKYUIL;ASTRGHNEOPZXCVBJM,./", "Miniak");   // 12-keys 版本
    static const auto KLAUSLER = Baseline("K,UYPWLMFCOAEIDRNTHSQ./;ZXVGBJ", "Klausler");
    static const auto CAPEWELL = Baseline(".YWDFJPLUQAERSGBTNIOXZCV;KMH,/", "Capewell"); // close-keys 版本
    static const auto QGMLWY   = Baseline("QGMLWYFUB;DSTNRIAEOHZXCVJKP,./", "QGMLWY");   // 源于 Carpalx 项目
    static const auto ASSET    = Baseline("QWFGJYPUL;ASETDHNIORZXCVBKM,./", "Asset");
    static const auto ARENSITO = Baseline("QL.P/;FUDKARENBGSITOZW,HJVCYMX", "Arensito");
    static const auto WORKMAN  = Baseline("QDRWBJFUP;ASHTGYNEOIZXMCVKL,./", "Workman");
    static const auto NORMAN   = Baseline("QWDFKJURL;ASETGYNIOHZXCVBPM,./", "Norman");
    static const auto CHIN     = Baseline(",TSZBPFRD.OEAGYLNIHUXQJCVKMW;/", "Chin");
    static const auto GOAT     = Baseline(",SEZBPFRD.GOATYLNIHUXQJCVKMW;/", "Goat");
    // @formatter:on //

    static const std::vector ALL{
        QWERTY, DVORAK, COLEMAK, MINIMAK, KLAUSLER, CAPEWELL,
        QGMLWY, ASSET, ARENSITO, WORKMAN, NORMAN, CHIN, GOAT,
    };
}

}

#endif //CLUBMOSS_LAYOUT_HXX
