module;

module infinity_core:ik_analyzer.impl;

import :ik_analyzer;
import :stl;
import :segmenter;
import :cjk_segmenter;
import :cn_quantifier_segmenter;
import :letter_segmenter;
import :analyze_context;
import :lexeme;
import :arbitrator;
import :term;
import :status;
import :character_util;

import std;

namespace infinity {

IKAnalyzer::IKAnalyzer(const std::string &path) : dict_path_(path) {}

IKAnalyzer::IKAnalyzer(const IKAnalyzer &other) : own_dict_(false), ik_smart_(other.ik_smart_), dict_(other.dict_) { Init(); }

IKAnalyzer::~IKAnalyzer() {
    if (own_dict_) {
        delete dict_;
    }
}

void IKAnalyzer::Init() {
    context_ = std::make_unique<AnalyzeContext>(dict_, ik_smart_);
    LoadSegmenters();
    arbitrator_ = std::make_unique<IKArbitrator>();
}

void IKAnalyzer::SetFineGrained(bool fine_grained) {
    ik_smart_ = !fine_grained;
    if (context_.get()) {
        context_->ik_smart_ = ik_smart_;
    }
}

Status IKAnalyzer::Load() {
    dict_ = new Dictionary(dict_path_);
    if (auto load_status = dict_->Load(); !load_status.ok()) {
        return load_status;
    }
    own_dict_ = true;
    Init();
    return Status::OK();
}

void IKAnalyzer::LoadSegmenters() {
    segmenters_.reserve(4);
    segmenters_.push_back(std::make_unique<LetterSegmenter>());
    segmenters_.push_back(std::make_unique<CNQuantifierSegmenter>(dict_));
    segmenters_.push_back(std::make_unique<CJKSegmenter>(dict_));
}

void IKAnalyzer::Reset() {
    context_->Reset();
    for (auto &segmenter : segmenters_) {
        segmenter->Reset();
    }
}

int IKAnalyzer::GetLastUselessCharNum() { return context_->GetLastUselessCharNum(); }

int IKAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
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
    arbitrator_->Process(context_.get(), ik_smart_);
    context_->OutputToResult();
    context_->MarkBufferOffset();
    Lexeme *lexeme = nullptr;
    while ((lexeme = context_->GetNextLexeme()) != nullptr) {
        std::wstring text = lexeme->GetLexemeText();
        std::string token = CharacterUtil::UTF16ToUTF8(text);
        func(data, token.c_str(), token.size(), offset++, 0, false, 0);
        delete lexeme;
    };
    return 0;
}

} // namespace infinity
