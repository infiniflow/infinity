module;

#include <vector>

import stl;
import segmenter;
import cjk_segmenter;
import cn_quantifier_segmenter;
import letter_segmenter;
import analyze_context;
import lexeme;
import arbitrator;
import term;
import status;
import character_util;
import third_party;

module ik_analyzer;

namespace infinity {

IKAnalyzer::IKAnalyzer(const String &path) : dict_path_(path) {}

IKAnalyzer::IKAnalyzer(const IKAnalyzer &other) : own_dict_(false), dict_(other.dict_) { Init(); }

IKAnalyzer::~IKAnalyzer() {
    if (own_dict_) {
        delete dict_;
    }
}

void IKAnalyzer::Init() {
    context_ = MakeUnique<AnalyzeContext>(dict_);
    LoadSegmenters();
    arbitrator_ = MakeUnique<IKArbitrator>();
}

Status IKAnalyzer::Load() {
    dict_ = new Dictionary(dict_path_);
    Status load_status = dict_->Load();
    if (!load_status.ok()) {
        return load_status;
    }
    own_dict_ = true;
    Init();
    return Status::OK();
}

void IKAnalyzer::LoadSegmenters() {
    segmenters_.reserve(4);
    segmenters_.push_back(MakeUnique<LetterSegmenter>());
    segmenters_.push_back(MakeUnique<CNQuantifierSegmenter>(dict_));
    segmenters_.push_back(MakeUnique<CJKSegmenter>(dict_));
}

void IKAnalyzer::Reset() {
    context_->Reset();
    for (auto &segmenter : segmenters_) {
        segmenter->Reset();
    }
}

int IKAnalyzer::GetLastUselessCharNum() { return context_->GetLastUselessCharNum(); }

int IKAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    unsigned level = 0;
    unsigned offset = 0;
    std::wstring line = CharacterUtil::UTF8ToUTF16(input.text_);
    context_->Reset();
    context_->FillBuffer(line);
    context_->InitCursor();
    do {
        for (auto &segmenter : segmenters_) {
            segmenter->Analyze(context_.get());
        }
    } while (context_->MoveCursor());
    for (auto &segmenter : segmenters_) {
        segmenter->Reset();
    }
    arbitrator_->Process(context_.get(), true);
    context_->OutputToResult();
    context_->MarkBufferOffset();
    Lexeme *lexeme = nullptr;
    while ((lexeme = context_->GetNextLexeme()) != nullptr) {
        std::wstring text = lexeme->GetLexemeText();
        String token = CharacterUtil::UTF16ToUTF8(text);
        func(data, token.c_str(), token.size(), offset++, 0, Term::AND, level, false);
        delete lexeme;
    };
    return 0;
}

} // namespace infinity
