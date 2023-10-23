#pragma once

#include "storage/io/file_writer.h"

namespace infinity {

class DictionaryWriterImpl;
class DictionaryWriter {
public:
    DictionaryWriter(FileWriter &writer);
    ~DictionaryWriter();

private:
    DictionaryWriterImpl *impl_ = nullptr;
};

} // namespace infinity