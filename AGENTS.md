# AGENTS.md

行为指南。

---

## 1. 构建系统

- **环境**：GCC 16（MinGW-w64）、xmake、Qt6、OpenMP。
- **唯一构建工具**：`xmake`，构建逻辑以 `xmake.lua` 为准。
- **根目录 `CMakeLists.txt` 与 `compile_commands`** 仅供 IDE 静态校验，禁止用作构建依据。
- **平台特征**：MinGW 使用 `xmake f -p mingw` 显式指定；不指定 `-p` 时为 MSVC（Windows 默认）。
  在 `xmake.lua` 中通过 `is_plat("mingw")` / `is_plat("windows")` 区分：MSVC 不需要 `--allow-multiple-definition` 链接标志。

```powershell
xmake f -p mingw -m release # release / debug 切换
xmake build unit_tests_core # 构建 core 单元测试目标
xmake run unit_tests_core --test_case="xxx" # 运行 core 单元测试的特定套件
xmake clean -a # 清理构建缓存
```

---

## 2. C++ 标准与新特性

- **语言标准**：C++26 (`cxx26`)，编译器为 GCC 16。
- **优先使用**`std::ranges`、`std::views`、`std::expected`、`std::optional`、`std::span`、`consteval`/`constexpr`/`constinit`、结构化绑定、概念约束、`using` 别名模板等新特性，以提升可读性。
- **避免**过度元编程，能直写则不必抽象。

---

## 3. 编码规范

### 3.1 命名

| 类别 | 风格 | 示例 |
|---|---|---|
| 全局常量 | `UPPER_CASE` | `MAX_KEY_COUNT` |
| 变量 / 函数 | `snake_case` | `calculate_cost()` |
| 类 / 结构体 / 枚举 | `PascalCase` | `Engine`, `Layout` |
| 私有成员 | `trailing_underscore_` | `cost_matrix_` |
| Helper 文件名 | 前导下划线 | `_test_*_helpers.hpp`, `_toml_helpers.hpp` |

### 3.2 风格约束

#### 3.2.1 命名空间与作用域注释

- 具名命名空间与 `TEST_SUITE` 末尾**不加**注释。
- 匿名命名空间末尾添加注释 `// anonymous namespace`。

#### 3.2.2 分隔注释约定

三级分隔：L1 三行等号线（文件级分界）、L2 三行虚线（函数分区）、L3 单行虚线（函数内子段）。L1/L2 横线总宽 80 列（`// ` 后 77 字符），L3 不限长。

```cpp
// =============================================================================
//  Major Section Name
// =============================================================================

// -----------------------------------------------------------------------------
//  Helper Section Name
// -----------------------------------------------------------------------------

// --- sub-section name ---
```

#### 3.2.3 其他约束

- 优先组合而非继承。
- 凡可写 `const` 处都应写。
- 优先使用 `and` / `or` / `not`，替代 `&&` / `||` / `!`。
- 非 void 函数统一使用尾随返回类型 `auto fn(...) -> ReturnType`。
- **禁用**`friend` 声明、裸指针管理生命周期、含非平凡构造的静态作用域对象。

### 3.3 架构边界

- `core/` 层**禁止**引用 Qt 头文件、直接使用 spdlog、直接访问文件系统。
- `core/` 层异常仅抛 `FatalError` 及其子类；OpenMP 并行区须含 `try-catch` 边界，区外重新抛出。

---

## 4. 测试规范（DOCTEST）

### 4.1 框架

- [doctest](https://github.com/doctest/doctest) v2.5.2。
- [nanobench](https://github.com/martinus/nanobench) v4.3.2。

### 4.2 测试文件组织

```
tests/core/unit_tests/
├── main.cpp                     # 定义 DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN   
├── <module>/                    # 对应模块
│   └── <submodule>/             # 对应子模块
│       ├── _test_*_helpers.hpp  # 共享测试辅助工具
│       └── test_*.cpp           # 单元测试
└── ...
```

### 4.3 编写 DOCTEST 用例

```cpp
#include <doctest.h>

TEST_SUITE("模块名称") {

TEST_CASE("用例描述") {
    CHECK(condition);
    CHECK_EQ(actual, expected);

    SUBCASE("场景A") { /* ... */ }
    SUBCASE("场景B") { /* ... */ }
}

}
```

- **`TEST_SUITE`**：按模块组织。
- **`TEST_CASE`**：描述一条有意义的测试场景。
- **`SUBCASE`**：相同初始化下测试不同分支，各自独立执行。
- **测试辅助**：重复逻辑提取为模板函数或共享 `*_helpers.hpp`。
- **断言宏**：优先 `CHECK_*` 系列，仅必须终止时用 `REQUIRE_*`。
- **比较宏**：禁止在 `CHECK()` 内使用比较运算符，改用对应 DOCTEST 比较宏。

| 禁止写法 | 正确写法 |
|---|---|
| `CHECK(a == b)` | `CHECK_EQ(a, b)` |
| `CHECK(a != b)` | `CHECK_NE(a, b)` |
| `CHECK(a < b)`  | `CHECK_LT(a, b)` |
| `CHECK(a > b)`  | `CHECK_GT(a, b)` |
| `CHECK(a <= b)` | `CHECK_LE(a, b)` |
| `CHECK(a >= b)` | `CHECK_GE(a, b)` |

`CHECK(boolean)` 仅用于无比较运算符的布尔判断（如 `CHECK(flag)`、`CHECK(ptr)`）。
**查找断言用法**：访问 [DOCTEST 官方文档](https://github.com/doctest/doctest/blob/master/doc/markdown/assertions.md)。

### 4.4 测试命名规范

**TEST_SUITE**：kebab-case，按 `"模块名-子模块"` 组织，如 `"config-layout"`。
**TEST_CASE**：小写英文句子，描述被测行为。

| 规则 | 正确 | 错误 |
|---|---|---|
| 动词或条件开头，描述行为 | `"empty TOML builds implicit zone"` | `"euclidean_distance"` |
| 首字母小写，类型保留大写 | `"RngPool throws when exhausted"` | `"rngpool throws when exhausted"` |
| 无 C++ scope 语法 | `"config manager build succeeds"` | `"ConfigManager::build succeeds"` |
| 句末无句号 | `"shuffle uniformity"` | `"shuffle uniformity."` |

特殊约定：TOML fixture 固定模板 `<fixture_name> should_pass` / `<fixture_name> should_fail`。

### 4.5 浮点数比较

```cpp
CHECK_EQ(actual, doctest::Approx(expected).epsilon(1e-6));
```

无需自定义 epsilon 时可直接使用 `doctest::Approx(expected)`。

### 4.6 测试命名空间约定

- **Helper 命名空间**：测试辅助头文件 (`_test_*_helpers.hpp`) 中的符号定义在 `namespace clubmoss` 中，无需额外子空间区分。
- **测试文件引入方式**：文件开头用一行 `using namespace clubmoss;` 引入，**不**使用 `namespace clubmoss { }` 包裹。
  ```cpp
  using namespace clubmoss;

  TEST_SUITE("config-layout") { ... }
  ```
- **匿名命名空间**：用于测试文件内部的局部辅助函数/类型，注释 `// anonymous namespace`。

---

## 5. 项目结构速览

```
src/core/       # 纯 C++ 核心（禁止 Qt）
├── common/     # 基础工具
├── models/     # 领域模型
├── metric/     # 评价指标
├── engine/     # 搜索引擎
└── report/     # 搜索报告

src/app/        # 桌面层
├── service/    # 配置加载、文件 IO
├── bridge/     # QObject 适配层
└── view/       # QML 界面

tests/core/
├── unit_tests/ # 单元测试
└── perf_tests/ # 性能测试
```

层级依赖：`app/view → app/bridge → app/service → core`

---

## 6. 参考文档

- 项目规范：`docs/project.md`

---

## 7. Git Commit 规范

### 7.1 提交信息格式

```
<type>(<scope>): <subject>

<body>
```

单行不超过 **72 字符**。body 可选，与 subject 之间空一行。

### 7.2 Type

| Type | 含义 | 示例场景 |
|---|---|---|
| `feat` | 新功能 | 新增变异算子、添加布局渲染 |
| `fix` | 修复 Bug | 修复种子随机性不足、修正越界读取 |
| `refactor` | 重构（不改变行为） | 提取公共基类、重命名符号 |
| `perf` | 性能优化 | 并行化适应度计算、缓存预热 |
| `test` | 增加/修改测试 | 新增 Layout 单元测试、补充边界用例 |
| `docs` | 文档变更 | 更新 AGENTS.md、补充 TOML 配置说明 |
| `chore` | 杂项（构建、配置、依赖） | 升级 doctest 版本、调整 xmake.lua |
| `style` | 仅格式化（无语义变化） | 重排 include、调整空格 |

### 7.3 Scope

小写 kebab-case：

| 层 | Scope |
|---|---|
| `core/` | `common`, `models`, `models-config` |
| `app/` | `service`, `service-toml`, `bridge` |
| `tests/` | `tests-unit`, `tests-unit-core` |
| 全局 | `build`, `deps`, `docs` |

### 7.4 Subject

祈使句、小写字母开头、句末无句号；说明**"做了什么"**而非"为什么"。

```
feat(engine): add roulette wheel selection ✓  fixed a bug ✗
fix(models): correct layout row offset overflow ✓  feat(engine): Added simulated annealing ✗
```

### 7.5 Body

说明 **为什么** 这么做，而非 **怎么做的**。每行 ≤ 72 字符。

```
feat(engine): add elitism strategy

Elitism preserves the top-N individuals across generations,
preventing regression in best-fitness. Default rate is 5%.
```

### 7.6 破坏性变更

type 后加 `!`，body 必须以 `BREAKING CHANGE:` 开头：

```
refactor!(metric): redesign cost interface

BREAKING CHANGE: KeyCost::evaluate() now takes a LayoutSpan
instead of Layout&. All subclasses must be updated.
```

### 7.7 分支命名规范

`<type>/<kebab-case-description>`，如 `feat/roulette-wheel-selection`。

### 7.8 提交粒度原则

- **一个提交只做一件事**：`fix` 不与 `refactor` 混合。
- **测试先行**：`test` 提交可先于 `feat` 提交。
- **TDD节奏**：`test` → `feat`（红绿循环）。
