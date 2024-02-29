module;

import stl;
import local_file_system;
import third_party;
import default_values;

export module random;

namespace infinity {

export String RandomString(SizeT len, u32 seed = 0);

export SharedPtr<String> DetermineRandomString(const String &parent_dir, const String &name) {
    LocalFileSystem fs;
    SharedPtr<String> result;
    do {
        u32 seed = std::time(nullptr);
        result = MakeShared<String>(fmt::format("{}/{}_{}", parent_dir, RandomString(DEFAULT_RANDOM_NAME_LEN, seed), name));
    } while (!fs.CreateDirectoryNoExp(*result));
    return result;
}

} // namespace infinity