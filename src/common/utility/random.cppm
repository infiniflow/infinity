module;

import stl;

export module random;

namespace infinity {

export String RandomString(SizeT len);

export SharedPtr<String> DetermineRandomString(const String &parent_dir, const String &name);

export SharedPtr<String> DetermineRandomString(const String &base_dir, const String &parent_dir, const String &name);

} // namespace infinity