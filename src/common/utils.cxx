#include "utils.hxx"

namespace clubmoss {

/**
 * @brief 相对路径 -> 绝对路径.
 * @param sub_path 相对于项目根目录的路径 (无前导斜杠, 例如 subdir/file.txt).
 * @return 相应的绝对路径 (本例中可能为 d:/somewhere/clubmoss/subdir/file.txt).
 * @note 本函数不检查路径是否存在, 请确保传入的路径合法.
 **/
auto Utils::absPath(std::string_view sub_path) -> std::string {
    static const std::string ROOT = searchForProjectRoot();
    return std::format("{}/{}", ROOT, sub_path);
}

/**
 * @brief 搜索项目根目录.
 * @return 项目根目录 (例如 d:/somewhere/clubmoss).
 * @throws FatalError 如果未在指定范围内找到满足条件的目录.
 * @note 本函数会递归向上搜索, 并将第一个包含所有必要文件的目录视为项目根目录.
 **/
auto Utils::searchForProjectRoot() -> std::string {
    auto path = std::filesystem::current_path();

    uz depth = 0;
    while (depth++ < 5) { // 限制搜索深度
        if (findAllRequiredFiles(path)) {
            return path.generic_string();
        }
        path = path.parent_path();
        if (path == path.root_path()) {
            break; // 到达根目录, 退出搜索
        }
    }

    // 未找到满足条件的目录, 抛出异常
    throw FatalError("Missing necessary files");
}

auto Utils::findAllRequiredFiles(const std::filesystem::path& path) -> bool {
    for (const auto& [sub_dir_name, file_names] : REQUIRED_FILES) {
        const std::filesystem::path curr_dir = path / sub_dir_name;
        if (not exists(curr_dir) or not is_directory(curr_dir)) {
            return false;
        }
        for (const std::string& file_name : file_names) {
            if (not exists(curr_dir / file_name)) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief 行列号 -> 键位.
 * @param row 行号.
 * @param col 列号.
 **/
auto Utils::posFromRowCol(const Row row, const Col col) noexcept -> Pos {
    assert(Utils::isLegalRow(row));
    assert(Utils::isLegalCol(col));
    return row * COL_COUNT + col;
}

/**
 * @brief 键位 -> 列号.
 * @param pos 键位.
 **/
auto Utils::colOfPos(const Pos pos) noexcept -> Col {
    assert(Utils::isLegalPos(pos));
    return pos % COL_COUNT;
}

/**
 * @brief 键位 -> 行号.
 * @param pos 键位.
 **/
auto Utils::rowOfPos(const Pos pos) noexcept -> Row {
    assert(Utils::isLegalPos(pos));
    return pos / COL_COUNT;
}

}
