#pragma once

import file_writer;

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