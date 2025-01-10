#include <doctest/doctest.h>
#include <nanobench.h>

#include "../../src/layout/layout_manager.hxx"

namespace clubmoss::layout::bench {

// 若要测试 std::array<Layout, N>; 的性能,
// 需要暂时将无参构造函数 Layout() 设为 public.

static constexpr uz NUM_LAYOUTS    = 1000;
static constexpr uz CONTAINER_SIZE = NUM_LAYOUTS * 2;

TEST_SUITE("Bench Layout container operations") {

    using ObjVec = std::vector<Layout>;
    using ObjArr = std::array<Layout, CONTAINER_SIZE>;
    using PtrVec = std::vector<std::unique_ptr<Layout>>;
    using PtrArr = std::array<std::unique_ptr<Layout>, CONTAINER_SIZE>;

    Manager manager;

    // ObjArr obj_arr;
    ObjVec obj_vec;
    PtrArr ptr_arr;
    PtrVec ptr_vec;

    auto initializeContainers() -> void {
        static bool initialized = false;
        if (initialized) { return; }

        // for (auto& obj : obj_arr) {
        //     obj = manager.create();
        // }
        for (uz i = 0; i < CONTAINER_SIZE; ++i) {
            obj_vec.emplace_back(manager.create());
        }
        for (auto& ptr : ptr_arr) {
            ptr = std::make_unique<Layout>(manager.create());
        }
        for (uz i = 0; i < CONTAINER_SIZE; ++i) {
            ptr_vec.emplace_back(std::make_unique<Layout>(manager.create()));
        }

        initialized = true;
    }

    TEST_CASE("Bench layout::Manager::reinit()") {
        ankerl::nanobench::Bench b;
        initializeContainers();

        b.title("Layout reinit()")
         .relative(true)
         .warmup(10)
         .minEpochIterations(500);
        b.performanceCounters(true);

        // b.run(
        //     "Arr of obj",
        //     [&]()-> void {
        //         for (auto& obj : obj_arr) {
        //             manager.reinit(obj);
        //         }
        //     }
        // );

        b.run(
            "Vec of obj",
            [&]()-> void {
                for (auto& obj : obj_vec) {
                    manager.reinit(obj);
                }
            }
        );

        b.run(
            "Arr of ptr",
            [&]()-> void {
                for (auto& obj : ptr_arr) {
                    manager.reinit(*obj);
                }
            }
        );

        b.run(
            "Vec of ptr",
            [&]()-> void {
                for (auto& obj : ptr_vec) {
                    manager.reinit(*obj);
                }
            }
        );
    }

    TEST_CASE("Bench layout::Manager::mutate()") {
        ankerl::nanobench::Bench b;
        initializeContainers();

        b.title("Layout mutate()")
         .relative(true)
         .warmup(10)
         .minEpochIterations(500);
        b.performanceCounters(true);

        // b.run(
        //     "Arr of obj",
        //     [&]()-> void {
        //         std::ranges::sort(obj_arr);
        //         for (uz i = 0; i < NUM_LAYOUTS; ++i) {
        //             manager.mutate(obj_arr[i + NUM_LAYOUTS], obj_arr[i]);
        //         }
        //     }
        // );

        b.run(
            "Vec of obj",
            [&]()-> void {
                std::ranges::sort(obj_vec);
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.mutate(obj_vec[i + NUM_LAYOUTS], obj_vec[i]);
                }
            }
        );

        b.run(
            "Arr of ptr",
            [&]()-> void {
                std::ranges::sort(ptr_arr);
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.mutate(*ptr_arr[i + NUM_LAYOUTS], *ptr_arr[i]);
                }
            }
        );

        b.run(
            "Vec of ptr",
            [&]()-> void {
                std::ranges::sort(ptr_vec);
                for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                    manager.mutate(*ptr_vec[i + NUM_LAYOUTS], *ptr_vec[i]);
                }
            }
        );
    }
}

}
