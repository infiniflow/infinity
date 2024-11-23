module;

#include <codecvt>
#include <locale>
#include <string>

export module character_util;

import stl;

namespace infinity {

export class CharacterUtil {
public:
    static const int CHAR_USELESS = 0;
    static const int CHAR_ARABIC = 0x00000001;
    static const int CHAR_ENGLISH = 0x00000002;
    static const int CHAR_CHINESE = 0x00000004;
    static const int CHAR_OTHER_CJK = 0x00000008;

    static int IdentifyCharType(wchar_t input) {
        if (input >= L'0' && input <= L'9') {
            return CHAR_ARABIC;
        } else if ((input >= L'a' && input <= L'z') || (input >= L'A' && input <= L'Z')) {
            return CHAR_ENGLISH;
        } else {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string utf8 = converter.to_bytes(input);
            if (utf8.length() == 3) { // UTF-8中文字符长度为3
                return CHAR_CHINESE;
            } else if (utf8.length() == 3) { // UTF-8日韩字符长度为3
                return CHAR_OTHER_CJK;
            }
        }
        return CHAR_USELESS;
    }

    static wchar_t Regularize(wchar_t input, bool lowercase) {
        if (input == 12288) {
            input = (wchar_t)32;
        } else if (input > 65280 && input < 65375) {
            input = (wchar_t)(input - 65248);
        } else if (input >= L'A' && input <= L'Z' && lowercase) {
            input += 32;
        }
        return input;
    }
};
} // namespace infinity