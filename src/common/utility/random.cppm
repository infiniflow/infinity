module;

import stl;

export module random;

namespace infinity {

export String RandomString(SizeT len, u32 seed = 0);

export SharedPtr<String> DetermineRandomString(const String &parent_dir, const String &name);

} // namespace infinity