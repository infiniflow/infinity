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

    static bool IsChinese(const wchar_t &ch) {
        if ((ch >= 0x4E00 && ch <= 0x9FFF) ||   //
            (ch >= 0x3400 && ch <= 0x4DBF) ||   //
            (ch >= 0x20000 && ch <= 0x2A6DF) || //
            (ch >= 0x2A700 && ch <= 0x2B73F) || //
            (ch >= 0x2B740 && ch <= 0x2B81F) || //
            (ch >= 0x2B820 && ch <= 0x2CEAF) || //
            (ch >= 0xF900 && ch <= 0xFAFF) ||   //
            (ch >= 0x2F800 && ch <= 0x2FA1F))
            return true;
        return false;
    }

    static bool IsJK(int unicode) {
        return ((unicode >= 0x3190 && unicode <= 0x319F) || // 漢文用の記号
                (unicode >= 0x3040 && unicode <= 0x309F) || // ひらがな
                (unicode >= 0x30a0 && unicode <= 0x30FF) || // カタカナ
                (unicode >= 0xFF61 && unicode <= 0xFF9F) || // 半角カタカナ
                (unicode >= 0x1100 && unicode <= 0x11F9) || // Korean
                (unicode >= 0x3131 && unicode <= 0x318E) || // Korean
                (unicode >= 0xAC00 && unicode <= 0xD7A3) || // Korean
                (unicode >= 0xFF01 && unicode <= 0xFFEE)    // Korean
        );
    }

    static bool IsCJK(const wchar_t unicode) {
        return ((unicode >= 0x4E00 && unicode <= 0x9FCF) ||   // CJK統合漢字
                (unicode >= 0x3400 && unicode <= 0x4DBF) ||   // CJK統合漢字拡張A
                (unicode >= 0x20000 && unicode <= 0x2A6DF) || // CJK統合漢字拡張B
                (unicode >= 0xF900 && unicode <= 0xFADF) ||   // CJK互換漢字
                (unicode >= 0x2F800 && unicode <= 0x2FA1F) || // CJK互換漢字補助
                (unicode >= 0x3190 && unicode <= 0x319F) ||   // 漢文用の記号
                (unicode >= 0x3040 && unicode <= 0x309F) ||   // ひらがな
                (unicode >= 0x30a0 && unicode <= 0x30FF) ||   // カタカナ
                (unicode >= 0xFF61 && unicode <= 0xFF9F)      // 半角カタカナ
        );
    }
    static int IdentifyCharType(const wchar_t input) {
        if (input >= L'0' && input <= L'9') {
            return CHAR_ARABIC;
        } else if ((input >= L'a' && input <= L'z') || (input >= L'A' && input <= L'Z')) {
            return CHAR_ENGLISH;
        } else {
            if (IsChinese(input))
                return CHAR_CHINESE;
            if (IsJK(input))
                return CHAR_OTHER_CJK;
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

    static std::wstring UTF8ToUTF16(const std::string &utf8_str) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
        return converter.from_bytes(utf8_str);
    }

    static std::string UTF16ToUTF8(const std::wstring &utf16_str) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
        return converter.to_bytes(utf16_str);
    }
};
} // namespace infinity