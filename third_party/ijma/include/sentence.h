/** \file sentence.h
 * Definition of class Sentence.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#ifndef JMA_SENTENCE_H
#define JMA_SENTENCE_H

#include <map>
#include <string>
#include <vector>

namespace jma {

/**
 * Morpheme is a pair of lexicon string and its part-of-speech tag.
 */
struct Morpheme {
    /** the lexicon string value */
    std::string lexicon_;

    /** the index code of part-of-speech tag */
    int posCode_;

    /** the POS string value */
    std::string posStr_;

    /**
     * the base form string value.
     * As an example in Japanese, "来る" is the base form of lexicon "来".
     */
    std::string baseForm_;

    /**
     * the reading form string value.
     * As an example in Japanese, "くる" is the reading form of lexicon "来る".
     */
    std::string readForm_;

    /**
     * the normalized form string value.
     * As an example in Japanese, "渡辺" is the normalized form of lexicon "渡邊", "エキスポ" is the normalized form of lexicon "エクスポ".
     */
    std::string normForm_;

    /**
     * Constructor.
     * The string value of lexicon, POS, base form, and reading form are initialized with empty string,
     * and the index code of part-of-speech tag is initialized with -1, meaning that no part-of-speech tag is available.
     */
    Morpheme();

    /**
     * Constructor.
     * \param lexicon the lexicon string value
     * \param posCode the index code of part-of-speech tag
     * \param posStr the POS string value
     * \param baseForm the base form string value
     * \param readForm the reading form string value
     * \param normForm the normalized form string value
     */
    Morpheme(const std::string &lexicon,
             int posCode,
             const std::string &posStr,
             const std::string &baseForm,
             const std::string &readForm,
             const std::string &normForm);
};

/** A list of morphemes. */
typedef std::vector<Morpheme> MorphemeList;

/**
 * Sentence saves the results of Japanese morphological analysis.
 * Typically, the usage is like below:
 *
 * \code
 * Sentence s;
 * s.setString("...");
 *
 * Analyzer* analyzer = ...;
 * analyzer->runWithSentence(s);
 *
 * // get n-best results
 * for(int i=0; i<s.getListSize(); ++i)
 * {
 *      for(int j=0; j<s.getCount(i); ++j)
 *      {
 *          const char* pLexicon = s.getLexicon(i, j);
 *          const char* strPOS = s.getStrPOS(i, j);
 *          const char* baseForm = s.getBaseForm(i, j);
 *          const char* readForm = s.getReadForm(i, j);
 *          const char* normForm = s.getNormForm(i, j);
 *          ...
 *      }
 *      double score = s.getScore(i);
 *      ...
 * }
 *
 * // get one-best result
 * int i = s.getOneBestIndex();
 * for(int j=0; j<s.getCount(i); ++j)
 * {
 *      const char* pLexicon = s.getLexicon(i, j);
 *      const char* strPOS = s.getStrPOS(i, j);
 *      const char* baseForm = s.getBaseForm(i, j);
 *      const char* readForm = s.getReadForm(i, j);
 *      const char* normForm = s.getNormForm(i, j);
 *      ...
 * }
 * ...
 * \endcode
 */
class Sentence {
public:
    /**
     * Default Constructor
     */
    Sentence();

    /**
     * Constructor with initial sentence string
     */
    Sentence(const char *pString);

    /**
     * Set the raw sentence string.
     * \param pString value of the raw string
     * \attention the previous analysis results will be removed.
     */
    void setString(const char *pString);

    /**
     * Get the raw sentence string.
     * \return value of the raw string
     */
    const char *getString(void) const;

    /**
     * Get the number of candidates of morphological analysis result.
     * \return the number of candidates
     */
    int getListSize(void) const;

    /**
     * Get the number of morphemes in candidate result \e nPos.
     * \param nPos candidate result index
     * \return the number of morphemes
     */
    int getCount(int nPos) const;

    /**
     * Get the string of morpheme \e nIdx in candidate result \e nPos.
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return morpheme string
     */
    const char *getLexicon(int nPos, int nIdx) const;

    /**
     * Get the POS index code of morpheme \e nIdx in candidate result \e nPos.
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return POS index code, -1 for non POS available
     */
    int getPOS(int nPos, int nIdx) const;

    /**
     * Get the POS string of morpheme \e nIdx in candidate result \e nPos.
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return POS string
     */
    const char *getStrPOS(int nPos, int nIdx) const;

    /**
     * Get the base form string of morpheme \e nIdx in candidate result \e nPos.
     * For example in Japanese, "来る" is returned as the base form of morpheme "来".
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return base form string
     */
    const char *getBaseForm(int nPos, int nIdx) const;

    /**
     * Get the reading form string of morpheme \e nIdx in candidate result \e nPos.
     * For example in Japanese, "くる" is returned as the reading form of morpheme "来る".
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return reading form string, if no reading form exists, empty string "" is returned
     */
    const char *getReadForm(int nPos, int nIdx) const;

    /**
     * Get the normalized form string of morpheme \e nIdx in candidate result \e nPos.
     * For example in Japanese, "渡辺" is returned as the normalized form of morpheme "渡邊".
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return normalized form string, if no normalized form exists, empty string "" is returned
     */
    const char *getNormForm(int nPos, int nIdx) const;

    /**
     * Get the MorphemeList of candidate result \e nPos.
     * \param nPos candidate result index
     * \return the MorphemeList
     */
    const MorphemeList *getMorphemeList(int nPos) const;

    /**
     * Get the score of candidate result \e nPos.
     * \param nPos candidate result index
     * \return the score value
     */
    double getScore(int nPos) const;

    /**
     * Set the score of candidate result \e nPos.
     * \param nPos candidate result index
     * \param nScore the score value
     */
    void setScore(int nPos, double nScore);

    /**
     * Get the index of the candidate result, which has the highest score.
     * \return candidate result index, -1 is returned if there is no candidate result.
     */
    int getOneBestIndex(void) const;

    /**
     * Add a candidate result of morphological analysis.
     * \param morphemeList the candidate result. After this function call, this parameter content would be cleared as its original content is swapped
     * into \e candidates_. \param score the score value of the candidate
     */
    void addList(MorphemeList &morphemeList, double score = 0.0);

private:
    /** the raw sentence string */
    std::string raw_;

    /** the candidates list of morphological analysis result */
    std::vector<MorphemeList> candidates_;

    /** the scores list of candidates */
    std::vector<double> scores_;
};

} // namespace jma

#endif // JMA_SENTENCE_H
