//
// Created by jinhai on 23-10-31.
//

#include "command_statement.h"

namespace infinity {

std::string UseCmd::ToString() const { return "Use Command"; }
std::string CheckTable::ToString() const { return "Check Table Command"; }

std::string CommandStatement::ToString() const { return command_info_->ToString(); }

} // namespace infinity