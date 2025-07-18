module;

#include <stdexcept>
#include <string>

export module infinity_core:character_util;

import :stl;

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
        // std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
        // return converter.from_bytes(utf8_str);
        std::wstring utf16_str;
        std::string_view utf8_view(utf8_str);
        while (!utf8_view.empty()) {
            if ((utf8_view[0] & 0x80) == 0) { // 1-byte character
                utf16_str.push_back(static_cast<wchar_t>(utf8_view[0]));
                utf8_view.remove_prefix(1);
            } else if ((utf8_view[0] & 0xE0) == 0xC0) { // 2-byte character
                if (utf8_view.size() < 2)
                    throw std::invalid_argument("Invalid UTF-8 sequence");
                utf16_str.push_back(static_cast<wchar_t>(((utf8_view[0] & 0x1F) << 6) | (utf8_view[1] & 0x3F)));
                utf8_view.remove_prefix(2);
            } else if ((utf8_view[0] & 0xF0) == 0xE0) { // 3-byte character
                if (utf8_view.size() < 3)
                    throw std::invalid_argument("Invalid UTF-8 sequence");
                utf16_str.push_back(static_cast<wchar_t>(((utf8_view[0] & 0x0F) << 12) | ((utf8_view[1] & 0x3F) << 6) | (utf8_view[2] & 0x3F)));
                utf8_view.remove_prefix(3);
            } else if ((utf8_view[0] & 0xF8) == 0xF0) { // 4-byte character
                if (utf8_view.size() < 4)
                    throw std::invalid_argument("Invalid UTF-8 sequence");
                u32 code_point =
                    ((utf8_view[0] & 0x07) << 18) | ((utf8_view[1] & 0x3F) << 12) | ((utf8_view[2] & 0x3F) << 6) | (utf8_view[3] & 0x3F);
                if (code_point <= 0xFFFF) {
                    utf16_str.push_back(static_cast<wchar_t>(code_point));
                } else {
                    code_point -= 0x10000;
                    utf16_str.push_back(static_cast<wchar_t>((code_point >> 10) + 0xD800));
                    utf16_str.push_back(static_cast<wchar_t>((code_point & 0x3FF) + 0xDC00));
                }
                utf8_view.remove_prefix(4);
            } else {
                throw std::invalid_argument("Invalid UTF-8 sequence");
            }
        }

        return utf16_str;
    }

    static std::string UTF16ToUTF8(const std::wstring &utf16_str) {
        // std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
        // return converter.to_bytes(utf16_str);
        std::string utf8_str;
        std::wstring_view utf16_view(utf16_str);
        while (!utf16_view.empty()) {
            if (utf16_view[0] < 0xD800 || utf16_view[0] > 0xDFFF) { // Basic Multilingual Plane
                u32 code_point = utf16_view[0];
                if (code_point <= 0x7F) {
                    utf8_str.push_back(static_cast<char>(code_point));
                } else if (code_point <= 0x7FF) {
                    utf8_str.push_back(static_cast<char>(0xC0 | (code_point >> 6)));
                    utf8_str.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                } else {
                    utf8_str.push_back(static_cast<char>(0xE0 | (code_point >> 12)));
                    utf8_str.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
                    utf8_str.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                }
                utf16_view.remove_prefix(1);
            } else {
                if (utf16_view.size() < 2 || utf16_view[0] < 0xD800 || utf16_view[0] > 0xDBFF || utf16_view[1] < 0xDC00 || utf16_view[1] > 0xDFFF) {
                    throw std::invalid_argument("Invalid UTF-16 sequence");
                }
                u32 code_point = 0x10000 + ((utf16_view[0] - 0xD800) << 10) + (utf16_view[1] - 0xDC00);
                utf8_str.push_back(static_cast<char>(0xF0 | (code_point >> 18)));
                utf8_str.push_back(static_cast<char>(0x80 | ((code_point >> 12) & 0x3F)));
                utf8_str.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
                utf8_str.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
                utf16_view.remove_prefix(2);
            }
        }

        return utf8_str;
    }
};
} // namespace infinity