#ifndef CLUBMOSS_PRNGS_HXX
#define CLUBMOSS_PRNGS_HXX

#include <random>

namespace clubmoss::prng {
class PRNG {
public:
    using result_type = uint64_t;

    [[maybe_unused]] static constexpr auto min() -> result_type {
        return std::numeric_limits<result_type>::min();
    }

    [[maybe_unused]] static constexpr auto max() -> result_type {
        return std::numeric_limits<result_type>::max();
    }
};

/// Written in 2015 by Sebastiano Vigna.
/// To the extent possible under law, the author has dedicated all copyright
/// and related and neighboring rights to this software to the public domain
/// worldwide. This software is distributed without any warranty.
/// See <http://creativecommons.org/publicdomain/zero/1.0/>.
class SplitMix64 final : public PRNG {
public:
    SplitMix64() : SplitMix64(42) {}

    explicit SplitMix64(const uint64_t seed) {
        this->seed(seed), warmup();
    }

    auto operator()() -> result_type {
        uint64_t z = (state_ += 0x9E3779B97F4A7C15ull);
        z = (z ^ (z >> 30)) * 0XBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0X94D049BB133111EBull;
        return z ^ (z >> 31);
    }

    auto seed(const uint64_t seed) -> void {
        this->state_ = seed;
    }

    auto warmup() -> void {
        for (size_t i = 0; i < 5; ++i) {
            operator()();
        }
    }

private:
    uint64_t state_{};
};

/// Copyright 2020 Mark A. Overton.
/// Licensed under Apache-2.0.
class RomuTrio64 final : public PRNG {
public:
    RomuTrio64() : RomuTrio64(42) {}

    explicit RomuTrio64(const uint64_t seed) {
        this->seed(seed), warmup();
    }

    auto operator()() -> result_type {
        const uint64_t xp = x_, yp = y_, zp = z_;
        x_ = 15241094284759029579ull * z_; // zp
        y_ = yp - xp, y_ = std::rotl(y_, 12);
        z_ = zp - yp, z_ = std::rotl(z_, 44);
        return xp;
    }

    auto seed(const uint64_t seed) -> void {
        static SplitMix64 initializer_;
        initializer_.seed(seed);
        initializer_.warmup();
        x_ = initializer_();
        y_ = initializer_();
        z_ = initializer_();
    }

    auto warmup() -> void {
        for (size_t i = 0; i < 5; ++i) {
            operator()();
        }
    }

private:
    uint64_t x_{};
    uint64_t y_{};
    uint64_t z_{};
};

/// Copyright 2019 Wang Yi.
/// Licensed under "The Unlicense".
class WyRand64 final : public PRNG {
public:
    WyRand64() : WyRand64(42) {}

    explicit WyRand64(const uint64_t seed) {
        this->seed(seed), warmup();
    }

    auto operator()() -> result_type {
        const __uint128_t sp = state_;
        state_ += 0xA0761D6478BD642Full;
        const __uint128_t c = sp ^ 0xE7037ED1A0B428DBull * sp;
        return static_cast<uint64_t>((c >> 64) ^ c);
    }

    auto seed(const uint64_t seed) -> void {
        this->state_ = seed;
    }

    auto warmup() -> void {
        for (size_t i = 0; i < 5; ++i) {
            operator()();
        }
    }

private:
    uint64_t state_{};
};

}

#endif // CLUBMOSS_PRNGS_HXX
