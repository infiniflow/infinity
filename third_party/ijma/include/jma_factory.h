/** \file jma_factory.h
 * Definition of class JMA_Factory.
 *
 * \author Jun Jiang
 * \version 0.1
 */

#ifndef JMA_FACTORY_H
#define JMA_FACTORY_H

namespace jma {

class Analyzer;
class Knowledge;

/**
 * JMA_Factory creates instances for Japanese morphological analysis.
 */
class JMA_Factory {
public:
    /**
     * Create an instance of \e JMA_Factory.
     * \return the pointer to instance
     */
    static JMA_Factory *instance();

    /**
     * Create an instance of \e Analyzer.
     * \return the pointer to instance
     */
    virtual Analyzer *createAnalyzer();

    /**
     * Create an instance of \e Knowledge.
     * \return the pointer to instance
     */
    virtual Knowledge *createKnowledge();

protected:
    /**
     * Constructor.
     */
    JMA_Factory();

    /**
     * Destructor.
     */
    virtual ~JMA_Factory();

private:
    /** the instance of factory */
    static JMA_Factory *instance_;
};

} // namespace jma

#endif // JMA_FACTORY_H
