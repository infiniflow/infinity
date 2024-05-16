/** \file jma_knowledge.h
 * Definition of class JMA_Knowledge.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 17, 2009
 */

#ifndef JMA_KNOWLEDGE_IMPL_H
#define JMA_KNOWLEDGE_IMPL_H

#include "char_table.h"
#include "jma_ctype.h"
#include "knowledge.h"
#include "pos_table.h"
#include "sentence.h"

#include <map>
#include <ostream>
#include <set>
#include <string>

namespace MeCab {
class Tagger;
class Iconv;
} // namespace MeCab

namespace jma {

class JMA_Dictionary;
class JMA_UserDictionary;

/**
 * JMA_Knowledge manages the linguistic information for Japanese morphological analysis.
 */
class JMA_Knowledge : public Knowledge {
public:
    /**
     * The map to decompose user defined noun.
     * The key of the map is the user defined noun, such as "本田総一郎",
     * the value of the map is a list of decomposed morphemes, such as "本田", "総一郎",
     * the decomposed morpheme includes Morpheme::lexicon_ and Morpheme::readForm_,
     * if Morpheme::readForm_ is an empty string, no read form is defined.
     */
    typedef std::map<std::string, MorphemeList> DecompMap;

    /**
     * Constructor.
     */
    JMA_Knowledge();

    /**
     * Destructor.
     */
    virtual ~JMA_Knowledge();

    /**
     * Load the dictionaries, which are set by \e setSystemDict() and \e addUserDict().
     * \return 0 for fail, 1 for success
     * \pre the directory path of system dictionary should be set by \e setSystemDict().
     * \pre if any user dictionary file need to be loaded, the file name should be added by \e addUserDict().
     */
    virtual int loadDict();

    /**
     * Load the stop-word dictionary file, which is in text format.
     * The words in this file are ignored in the morphological analysis result.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStopWordDict(const char *fileName);

    /**
     * Set the part-of-speech tags as keywords.
     * If this method is not called or \e posVec is empty,
     * the results of \e Analyzer::runWith*() would contain all words.
     * Otherwise, if \e posVec is not empty,
     * the results of \e Analyzer::runWith*() would only contain the words with part-of-speech tags specified by this method.
     * \param posVec the vector of part-of-speech tags in alphabet format, such as "NC-G"
     * \return the number of part-of-speech tags successfully set
     * \attention This method should be called after \e loadDict(), otherwise no keyword part-of-speech tag would be set
     */
    virtual int setKeywordPOS(const std::vector<std::string> &posVec);

    /**
     * Encode the system dictionary files from text to binary file.
     * \param txtDirPath the directory path of text files
     * \param binDirPath the directory path of binary files
     * \param binEncodeType the encoding type of binary system dictionary
     * \return 0 for fail, 1 for success
     * \pre both \e txtDirPath and \e binDirPath are assumed as the directory paths that already exists. If any of the directory paths does not exist,
     * 0 would be returned for fail.
     */
    virtual int encodeSystemDict(const char *txtDirPath, const char *binDirPath, EncodeType binEncodeType);

    /**
     * Create tagger by loading dictionary files.
     * \return pointer to tagger. 0 for fail, otherwise the life cycle of the tagger should be maintained by the caller.
     */
    MeCab::Tagger *createTagger() const;

    /**
     * Get the part-of-speech tags table.
     * \return reference to the table instance.
     */
    const POSTable &getPOSTable() const;

    /**
     * Get the mapping table to convert between Hiragana and Katakana characters.
     * \return reference to the table instance.
     */
    const CharTable &getKanaTable() const;

    /**
     * Get the mapping table to convert between half and full width characters.
     * \return reference to the table instance.
     */
    const CharTable &getWidthTable() const;

    /**
     * Get the mapping table to convert between lower and upper case characters.
     * \return reference to the table instance.
     */
    const CharTable &getCaseTable() const;

    /**
     * Get the map to decompose user defined noun.
     * \return reference to the map instance.
     */
    const DecompMap &getDecompMap() const;

    /**
     * Whether the specific word is stop word.
     * \param word the word to be checked
     * \return whether the word is in the stop word list
     */
    bool isStopWord(const std::string &word) const;

    /**
     * Get the number of stop words loaded.
     * \return the number of stop words loaded.
     */
    unsigned int stopWordCount() const;

    /**
     * Check whether is a seperator of sentence.
     * \param p pointer to the character string, it would check all the characters until null character
     * \return true for separator, false for non separator.
     */
    bool isSentenceSeparator(const char *p) const;

    /**
     * Whether the specific part-of-speech tag is keyword
     *
     * \param pos the index code of part-of-speech tag
     * \return whether is keyword
     */
    bool isKeywordPOS(int pos) const;

    /**
     * Get the number of keyword POS.
     * \return the number of keyword POS.
     */
    unsigned int keywordPOSCount() const;

    /**
     * Get the feature offset of base form indexed from zero.
     * For example, if an entry in dictionary file contains the features like "動詞,自立,*,*,一段,未然形,見る,ミ,ミ",
     * the feature offset of base form "見る" would be 6.
     * \return the feature offset
     */
    int getBaseFormOffset() const;

    /**
     * Get the feature offset of reading form indexed from zero.
     * For example, if an entry in dictionary file contains the features like "動詞,自立,*,*,一段,未然形,見る,ミ,ミ",
     * the feature offset of reading form "ミ" would be 7.
     * \return the feature offset
     */
    int getReadFormOffset() const;

    /**
     * Get the feature offset of normalized form indexed from zero.
     * For example, if an entry in dictionary file contains the features like "名詞,固有名詞,人名,姓,*,*,渡邊,わたなべ,ワタナベ,渡辺",
     * the feature offset of normalized form "渡辺" would be 9.
     * \return the feature offset
     */
    int getNormFormOffset() const;

    /**
     * Get the part-of-speech index code of user defined noun.
     * \return the POS index code, -1 for not defined.
     */
    int getUserNounPOSIndex() const;

    /**
     * Get the current JMA_CType
     * \return the current JMA_CType
     */
    JMA_CType *getCType();

private:
    /**
     * Whether any user dictionary is added by \e Knowledge::addUserDict().
     * \return true for user dictionary is added, false for no user dictionary is added.
     */
    bool hasUserDict() const;

    /**
     * Compile the user dictionary file from text to binary format.
     * \return true for success, false for fail.
     * \pre \e systemDictPath_ is assumed as the directory path of system dictionary.
     * \pre \e userDictNames_ is assumed as file names of user dictionaries in text format.
     * \post as the compilation result, \e binUserDic_ is the file name of the temporary user dictionary in binary format.
     */
    bool compileUserDict();

    /**
     * Load dictionary config file "dicrc" to get the values of entry defined by iJMA, such as "base-form-feature-offset" entry.
     * \attention if "dicrc" not exists, default configuration value would be used.
     */
    void loadDictConfig();

    /**
     * Convert the User's txt file to CSV format, which includes word, POS, read form.
     * \param userDicFile user dictionary file
     * \param ost csv output stream
     * \return how many entries are written into \e ost
     */
    unsigned int convertTxtToCSV(const UserDictFileType &userDicFile, std::ostream &ost);

    /**
     * Fill the binary encoding type of "binary-charset" from source "dicrc" to destination file.
     * \param src the source "dicrc" file
     * \param dest the destination "dicrc" file
     * \param binEncodeType the binary encoding type
     * \return true for success, false for failure
     */
    bool fillBinaryEncodeType(const char *src, const char *dest, EncodeType binEncodeType) const;

    /**
     * Load the sentence separator configuration file, which is in text format.
     * This file each separator character(only one character) per line.
     * \param fileName the file name
     * \param iconv to convert the input file's encoding type to run time encoding type
     * \return true for success, false for failure
     * \attention if this function is already called before, the separator previously loaded would be removed.
     */
    bool loadSentenceSeparatorConfig(const char *fileName, MeCab::Iconv &iconv);

private:
    /** the table of part-of-speech tags */
    POSTable posTable_;

    /** file name for binary user dictionary in memory */
    std::string binUserDic_;

    /** the part-of-speech index codes as keywords */
    std::set<int> keywordPOSSet_;

    /** stop words set */
    std::set<std::string> stopWords_;

    /** sentence separators */
    std::set<std::string> sentSeps_;

    /** whether POS result is in the format of full category */
    bool isOutputFullPOS_;

    /** POS category number, which value is got from "pos-id.def" in the directory of system dictionary in binary type */
    int posCatNum_;

    /** the feature offset (starting from zero) of base form, which value is got from entry "base-form-feature-offset" in "dicrc" in the directory of
     * system dictionary in binary type */
    int baseFormOffset_;

    /** the feature offset (starting from zero) of reading form, which value is got from entry "read-form-feature-offset" in "dicrc" in the directory
     * of system dictionary in binary type */
    int readFormOffset_;

    /** the feature offset (starting from zero) of normalized form, which value is got from entry "norm-form-feature-offset" in "dicrc" in the
     * directory of system dictionary in binary type */
    int normFormOffset_;

    /** the POS of user defined nouns */
    std::string userNounPOS_;

    /** The Character Type */
    JMA_CType *ctype_;

    /** character encode type of dictionary config files.
     * It is set by "config_charset" item in "dicrc" file.
     */
    EncodeType configEncodeType_;

    /** mapping table between Hiragana and Katakana characters */
    CharTable kanaTable_;

    /** mapping table between half and full width characters */
    CharTable widthTable_;

    /** mapping table between lower and upper case characters */
    CharTable caseTable_;

    /** the instance of decomposition map */
    DecompMap decompMap_;

    /** the system dictionary instance */
    JMA_Dictionary *dictionary_;

    /** the user dictionary instance */
    JMA_UserDictionary *userDictionary_;
};

} // namespace jma

#endif // JMA_KNOWLEDGE_IMPL_H
