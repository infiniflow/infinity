/** \file analyzer.cpp
 * Implementation of class Analyzer.
 *
 * \author Jun Jiang
 * \version 0.1
 */

#include "analyzer.h"

#include <cassert>

namespace {
/** the default delimiter between word and POS tag in the output result of \e runWithString() and \e runWithStream() */
const char *DEFAULT_POS_DELIMITER = "/";

/** the default delimiter between the pairs (word and POS tag) in the output result of \e runWithString() and \e runWithStream() */
const char *DEFAULT_WORD_DELIMITER = "  ";
} // namespace

namespace jma {

Analyzer::Analyzer() : options_(OPTION_TYPE_NUM), posDelimiter_(DEFAULT_POS_DELIMITER), wordDelimiter_(DEFAULT_WORD_DELIMITER) {
    options_[OPTION_TYPE_POS_TAGGING] = 1;         // tag part-of-speech tags defaultly
    options_[OPTION_TYPE_NBEST] = 1;               // set the default number of candidate results of runWithSentence()
    options_[OPTION_TYPE_POS_FORMAT_ALPHABET] = 0; // disable output part-of-speech tags in alphabet format defaultly, use Japanese format instead.
    options_[OPTION_TYPE_POS_FULL_CATEGORY] = 0;   // disable output part-of-speech tags in Japanese full category format
    options_[OPTION_TYPE_COMPOUND_MORPHOLOGY] = 1; // enable combining into compound words defaultly
    options_[OPTION_TYPE_CONVERT_TO_HIRAGANA] = 0; // disable conversion to Hiragana characters defaultly
    options_[OPTION_TYPE_CONVERT_TO_KATAKANA] = 0; // disable conversion to Katakana characters defaultly
}

Analyzer::~Analyzer() {}

void Analyzer::setOption(OptionType nOption, double nValue) {
    // check nbest value range
    if (nOption == OPTION_TYPE_NBEST && nValue < 1)
        return;

    options_[nOption] = nValue;
}

double Analyzer::getOption(OptionType nOption) const { return options_[nOption]; }

void Analyzer::setPOSDelimiter(const char *delimiter) { posDelimiter_ = delimiter; }

const char *Analyzer::getPOSDelimiter() const { return posDelimiter_; }

void Analyzer::setWordDelimiter(const char *delimiter) { wordDelimiter_ = delimiter; }

const char *Analyzer::getWordDelimiter() const { return wordDelimiter_; }

} // namespace jma
