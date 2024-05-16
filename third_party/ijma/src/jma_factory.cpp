/** \file jma_factory.cpp
 * Implementation of class JMA_Factory.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#include "jma_factory.h"
#include "jma_analyzer.h"
#include "jma_knowledge.h"

namespace jma {

JMA_Factory *JMA_Factory::instance_;

JMA_Factory *JMA_Factory::instance() {
    if (instance_ == 0) {
        instance_ = new JMA_Factory;
    }

    return instance_;
}

Analyzer *JMA_Factory::createAnalyzer() { return new JMA_Analyzer; }

Knowledge *JMA_Factory::createKnowledge() { return new JMA_Knowledge; }

JMA_Factory::JMA_Factory() {}

JMA_Factory::~JMA_Factory() {}

} // namespace jma
