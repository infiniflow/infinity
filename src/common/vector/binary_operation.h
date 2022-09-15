//
// Created by JinHai on 2022/9/15.
//

#include "storage/transient_block.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class BinaryOperation {
public:
    template <typename LeftType, typename RightType, typename OutputType, typename Operation>
    static void Execute(Chunk& left, Chunk& right, Chunk& output) {

        // TODO: too many if else check, use table to refactor the code.
        if(left.chunk_type() == ChunkType::kNormal) {
            if(right.chunk_type() == ChunkType::kNormal) {
                // Left & right are both normal type.
                // TODO: This is a simplified implementation, need to be designed carefully in the future.
                int64_t row_count = left.row_count();
                for(int64_t i = 0; i < row_count; ++ i) {
                    std::any_cast<OutputType>(output.data()[i]) =
                            Operation::template Execute<LeftType, RightType, OutputType>(
                                    std::any_cast<LeftType>(left.data()[i]),
                                    std::any_cast<RightType>(left.data()[i])
                                    );
                }
                return ;
            } else if (right.chunk_type() == ChunkType::kConstant) {
                // Left is normal type and right is constant type.
                // TODO: This is a simplified implementation, need to be designed carefully in the future.
                int64_t row_count = left.row_count();
                for(int64_t i = 0; i < row_count; ++ i) {
                    std::any_cast<OutputType>(output.data()[i]) =
                            Operation::template Execute<LeftType, RightType, OutputType>(
                                    std::any_cast<LeftType>(left.data()[i]),
                                    std::any_cast<RightType>(left.data()[i])
                            );
                }
                return ;
            }
        } else {
            if(right.chunk_type() == ChunkType::kNormal) {
                // Left & right are both normal type.
                // TODO: This is a simplified implementation, need to be designed carefully in the future.
                int64_t row_count = left.row_count();
                for(int64_t i = 0; i < row_count; ++ i) {
                    std::any_cast<OutputType>(output.data()[i]) =
                            Operation::template Execute<LeftType, RightType, OutputType>(
                                    std::any_cast<LeftType>(left.data()[i]),
                                    std::any_cast<RightType>(left.data()[i])
                            );
                }
                return ;
            } else if (right.chunk_type() == ChunkType::kConstant) {
                // Left is normal type and right is constant type.
                // TODO: This is a simplified implementation, need to be designed carefully in the future.
                int64_t row_count = left.row_count();
                for(int64_t i = 0; i < row_count; ++ i) {
                    std::any_cast<OutputType>(output.data()[i]) =
                            Operation::template Execute<LeftType, RightType, OutputType>(
                                    std::any_cast<LeftType>(left.data()[i]),
                                    std::any_cast<RightType>(left.data()[i])
                            );
                }
                return ;
            }
        }
    }
};

}
