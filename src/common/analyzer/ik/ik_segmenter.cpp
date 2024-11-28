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

module ik_segmenter;

namespace infinity {

IKSegmenter::IKSegmenter(const String &path) : dict_path_(path) {}

IKSegmenter::IKSegmenter(const IKSegmenter &other) : own_dict_(false), dict_(other.dict_) {}

IKSegmenter::~IKSegmenter() {
    if (own_dict_) {
        delete dict_;
    }
}

Status IKSegmenter::Load() {
    context_ = MakeShared<AnalyzeContext>(dict_);
    segmenters_ = LoadSegmenters();
    arbitrator_ = MakeShared<IKArbitrator>();
    dict_ = new Dictionary(dict_path_);
    Status load_status = dict_->Load();
    if (!load_status.ok()) {
        return load_status;
    }
    own_dict_ = true;
    return Status::OK();
}

Vector<SharedPtr<Segmenter>> IKSegmenter::LoadSegmenters() {
    Vector<SharedPtr<Segmenter>> segmenters_;
    segmenters_.reserve(4);
    segmenters_.push_back(MakeShared<LetterSegmenter>());
    segmenters_.push_back(MakeShared<CNQuantifierSegmenter>(dict_));
    segmenters_.push_back(MakeShared<CJKSegmenter>(dict_));
    return segmenters_;
}

Lexeme *IKSegmenter::Next() {
    Lexeme *l = nullptr;
    while ((l = context_->GetNextLexeme()) == nullptr) {
        context_->InitCursor();
        do {
            for (auto &segmenter : segmenters_) {
                segmenter->Analyze(context_.get());
            }
            if (context_->NeedRefillBuffer()) {
                break;
            }
        } while (context_->MoveCursor());
        for (auto &segmenter : segmenters_) {
            segmenter->Reset();
        }
        arbitrator_->Process(context_.get(), true);
        context_->OutputToResult();
        context_->MarkBufferOffset();
    }
    return l;
}

void IKSegmenter::Reset() {
    context_->Reset();
    for (auto &segmenter : segmenters_) {
        segmenter->Reset();
    }
}

int IKSegmenter::GetLastUselessCharNum() { return context_->GetLastUselessCharNum(); }

int IKSegmenter::AnalyzeImpl(const Term &input, void *data, HookType func) {
    unsigned level = 0;
    unsigned offset = 0;
    std::wstring line = CharacterUtil::UTF8ToUTF16(input.text_);
    context_->FillBuffer(line);
    Lexeme *lexeme = nullptr;
    while ((lexeme = Next()) != nullptr) {
        std::wstring text = lexeme->GetLexemeText();
        String token = CharacterUtil::UTF16ToUTF8(text);
        func(data, token.c_str(), token.size(), offset++, 0, Term::AND, level, false);
        delete lexeme;
    };
    return 0;
}

} // namespace infinity
