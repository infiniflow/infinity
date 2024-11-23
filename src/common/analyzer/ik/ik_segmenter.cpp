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

module ik_segmenter;

namespace infinity {

IKSegmenter::IKSegmenter() { Init(); }

void IKSegmenter::Init() {
    context_ = MakeShared<AnalyzeContext>();
    segmenters_ = LoadSegmenters();
    arbitrator_ = MakeShared<IKArbitrator>();
}

Vector<SharedPtr<Segmenter>> IKSegmenter::LoadSegmenters() {
    Vector<SharedPtr<Segmenter>> segmenters_;
    segmenters_.reserve(4);
    segmenters_.push_back(MakeShared<LetterSegmenter>());
    segmenters_.push_back(MakeShared<CNQuantifierSegmenter>());
    segmenters_.push_back(MakeShared<CJKSegmenter>());
    return segmenters_;
}

Lexeme *IKSegmenter::Next() {
    Lexeme *l = nullptr;
    while ((l = context_->GetNextLexeme()) == nullptr) {
        int available = context_->FillBuffer(input_);
        if (available <= 0) {
            context_->Reset();
            return nullptr;
        } else {
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

} // namespace infinity
