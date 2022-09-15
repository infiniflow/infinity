//
// Created by JinHai on 2022/9/15.
//

#include "storage/transient_block.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class UnaryOperation {
public:
    template <typename InputType, typename OutputType, typename Operation>
    static void Execute(const Chunk& input, Chunk& output) {
        switch(input.chunk_type()) {
            case ChunkType::kNormal: {
                // TODO: This is a simplified implementation, need to be designed carefully in the future.
                int64_t row_count = input.row_count();
                for(int64_t i = 0; i < row_count; ++ i) {
//                    std::any_cast<OutputType>(output.data()[i]) =
//                            Operation::template Execute<InputType, OutputType>(std::any_cast<InputType>(input.data()[i]));
                    // FIXME: output.data() should be the raw pointer not std::any
                    output.data()[i] = Operation::template Execute<InputType, OutputType>(std::any_cast<InputType>(input.data()[i]));
                }
            }
            case ChunkType::kConstant: {
                // TODO: This is a simplified implementation, need to be designed carefully in the future.
                int64_t row_count = input.row_count();
                for(int64_t i = 0; i < row_count; ++ i) {
//                    std::any_cast<OutputType>(output.data()[i]) =
//                            Operation::template Execute<InputType, OutputType>(std::any_cast<InputType>(input.data()[i]));
                    // FIXME: output.data() should be the raw pointer not std::any
                    output.data()[i] = Operation::template Execute<InputType, OutputType>(std::any_cast<InputType>(input.data()[i]));
                }
            }
            default: {
                ExecutorError("Wrong chunk type");
            }
        }
    }
};

}
