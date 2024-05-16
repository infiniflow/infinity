/** \file jma_dictionary.h
 * Definition of class JMA_Dictionary.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Aug 02, 2010
 */

#ifndef JMA_DICTIONARY_H
#define JMA_DICTIONARY_H

#include "mutex.h" // MeCab::Mutex

#include <map>
#include <string>
#include <vector>

namespace jma {

/**
 * DictUnit is a dictionary file in archive.
 */
struct DictUnit {
    /** start address */
    char *text_;

    /** total length */
    unsigned int length_;

    /** file name (without path) */
    std::string fileName_;

    /**
     * Constructor.
     */
    DictUnit() : text_(0), length_(0) {}
};

/** mapping from dictionary file name to dictionary instance */
typedef std::map<std::string, DictUnit> DictMap;

/**
 * DictArchive is a system dictionary as an archive of dictionary files.
 */
struct DictArchive {
    /** reference count,
     * incremented by JMA_Dictionary::open(),
     * decremented by JMA_Dictionary::close(),
     * deleted when reached to 0 */
    int refCount_;

    /** the start address */
    char *startAddr_;

    /** mapping from dictionary file name (without path) to dictionary instance */
    DictMap dictMap_;

    /**
     * Constructor.
     * The reference count is initialized to 1.
     */
    DictArchive() : refCount_(1), startAddr_(0) {}
};

/**
 * JMA_Dictionary is a collection of \e DictArchive shared by multiple threads.
 */
class JMA_Dictionary {
public:
    /**
     * Get the instance of \e JMA_Dictionary.
     * \return the pointer to instance
     */
    static JMA_Dictionary *instance();

    /**
     * Load the archive file "sys.bin" under \e dirName.
     * \param dirName the directory name
     * \return true for success, false for failure
     */
    bool open(const char *dirName);

    /**
     * Close the opened archive file "sys.bin" under \e dirName.
     * \param dirName the directory name
     * \return true for success, false for failure
     */
    bool close(const char *dirName);

    /**
     * Get a dictionary file.
     * \param fileName the full file name including path
     * \return the pointer to dictionary file, 0 is returned if not loaded
     */
    const DictUnit *getDict(const char *fileName) const;

    /**
     * Complile dictionary files \e srcVec into archive \e destFile.
     * \param srcFiles the file names of dictionary files
     * \param destFile the archive file name
     * \return true for success, false for failure
     */
    static bool compile(const std::vector<std::string> &srcFiles, const char *destFile);

    /**
     * Print status for debug use.
     */
    void debugPrint() const;

protected:
    /**
     * Constructor.
     */
    JMA_Dictionary();

    /**
     * Destructor.
     */
    virtual ~JMA_Dictionary();

private:
    friend class JMA_KnowledgeTest;

    /** the instance of dictionary */
    static JMA_Dictionary *instance_;

    /** mapping from directory name to archive instance */
    typedef std::map<std::string, DictArchive> ArchiveMap;

    /** archive map instance */
    ArchiveMap archiveMap_;

    /** mutex for lock critical section */
    mutable MeCab::Mutex mutex_;
};

/**
 * JMA_UserDictionary is a collection of user dictionaries shared by multiple threads.
 */
class JMA_UserDictionary {
public:
    /**
     * Get the instance of \e JMA_UserDictionary.
     * \return the pointer to instance
     */
    static JMA_UserDictionary *instance();

    /**
     * Create an empty file in memory.
     * \param newName the new file name is set when return value is true
     * \return true for success, false for failure
     */
    bool create(std::string &newName);

    /**
     * Destroy the created file.
     * \param fileName the file name
     * \return true for success, false for failure
     */
    bool release(const char *fileName);

    /**
     * Get a dictionary file.
     * \param fileName the created file name
     * \return the pointer to dictionary file, 0 is returned if not created
     */
    const DictUnit *getDict(const char *fileName) const;

    /**
     * Copy the string content to dictionary.
     * \param str the string content
     * \param fileName the created file name
     * \return true for success, false for failure
     */
    bool copyStrToDict(const std::string &str, const char *fileName);

    /**
     * Print status for debug use.
     */
    void debugPrint() const;

protected:
    /**
     * Constructor.
     */
    JMA_UserDictionary();

    /**
     * Destructor.
     */
    virtual ~JMA_UserDictionary();

private:
    /** the instance of dictionary */
    static JMA_UserDictionary *instance_;

    /** mapping from created file name to user dictionary instance */
    DictMap userDictMap_;

    /** mutex for lock critical section */
    mutable MeCab::Mutex mutex_;

    /** index number for each user file created */
    unsigned int index_;
};

} // namespace jma

#endif // JMA_DICTIONARY
