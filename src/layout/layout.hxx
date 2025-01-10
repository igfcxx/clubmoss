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
    // 部分布局使用'/'替代了不在键值集中的字符
    static const auto QWERTY   = Layout("QWERTYUIOPASDFGHJKL;ZXCVBNM,./");
    static const auto DVORAK   = Layout(";,.PYFGCRLAOEUIDHTNS/QJKXBMWVZ");
    static const auto COLEMAK  = Layout("QWFPGJLUY;ARSTDHNEIOZXCVBKM,./");
    static const auto MINIMAK  = Layout("QWDFKYUIL;ASTRGHNEOPZXCVBJM,./"); // 12-keys 版本
    static const auto KLAUSLER = Layout("K,UYPWLMFCOAEIDRNTHSQ./;ZXVGBJ");
    static const auto CAPEWELL = Layout(".YWDFJPLUQAERSGBTNIOXZCV;KMH,/"); // close-keys 版本
    static const auto QGMLWY   = Layout("QGMLWYFUB;DSTNRIAEOHZXCVJKP,./"); // 源于 Carpalx 项目
    static const auto ASSET    = Layout("QWFGJYPUL;ASETDHNIORZXCVBKM,./");
    static const auto ARENSITO = Layout("QL.P/;FUDKARENBGSITOZW,HJVCYMX");
    static const auto WORKMAN  = Layout("QDRWBJFUP;ASHTGYNEOIZXMCVKL,./");
    static const auto NORMAN   = Layout("QWDFKJURL;ASETGYNIOHZXCVBPM,./");
    static const auto CHIN     = Layout(",TSZBPFRD.OEAGYLNIHUXQJCVKMW;/");
    static const auto GOAT     = Layout(",SEZBPFRD.GOATYLNIHUXQJCVKMW;/");

    static const std::vector ALL_BASELINES{
        QWERTY, DVORAK, COLEMAK, MINIMAK, KLAUSLER, CAPEWELL,
        QGMLWY, ASSET, ARENSITO, WORKMAN, NORMAN, CHIN, GOAT,
    };
}

}

#endif //CLUBMOSS_LAYOUT_HXX
