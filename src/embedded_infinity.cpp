#include "nanobind/nanobind.h"
#include "parser/statement/command_statement.h"

import infinity;
import stl;

int add(int a, int b) {
    return a + b;
}

int mul(int a, int b) {
    return a * b;
}
using infinity::Infinity;

NB_MODULE(embedded_infinity, m) {
    m.def("add", &add);
    m.def("mul", &mul);
    m.def("hello", &infinity::Infinity::Hello);

    nanobind::class_<Infinity>(m, "Infinity")
        .def(nanobind::init<>())  // bind constructor

        .def("GetSessionId", &Infinity::GetSessionId)

        .def_static("LocalInit", &Infinity::LocalInit)
        .def_static("LocalUnInit", &Infinity::LocalUnInit)
        .def_static("Hello", &Infinity::Hello)

        .def_static("RemoteConnect", &Infinity::RemoteConnect)
        .def("RemoteDisconnect", &Infinity::RemoteDisconnect)
        .def_static("LocalConnect", &Infinity::LocalConnect)
        .def("LocalDisconnect", &Infinity::LocalDisconnect)

        .def("CreateDatabase", &Infinity::CreateDatabase)
        .def("DropDatabase", &Infinity::DropDatabase)
        .def("ListDatabases", &Infinity::ListDatabases)
        .def("GetDatabase", &Infinity::GetDatabase)
        .def("ShowDatabase", &Infinity::ShowDatabase)
        .def("Flush", &Infinity::Flush)

        .def("SetVariableOrConfig", nanobind::overload_cast<const infinity::String&, bool, infinity::SetScope>(&Infinity::SetVariableOrConfig))
        .def("SetVariableOrConfig", nanobind::overload_cast<const infinity::String&, infinity::i64, infinity::SetScope>(&Infinity::SetVariableOrConfig))
        .def("SetVariableOrConfig", nanobind::overload_cast<const infinity::String&, double, infinity::SetScope>(&Infinity::SetVariableOrConfig))
        .def("SetVariableOrConfig", nanobind::overload_cast<const infinity::String&, infinity::String, infinity::SetScope>(&Infinity::SetVariableOrConfig))

        .def("ShowVariable", &Infinity::ShowVariable)
        .def("ShowVariables", &Infinity::ShowVariables)
        .def("ShowConfig", &Infinity::ShowConfig)
        .def("ShowConfigs", &Infinity::ShowConfigs)

        .def("Query", &Infinity::Query)

        .def("CreateTable", &Infinity::CreateTable)
        .def("DropTable", &Infinity::DropTable)
        .def("ListTables", &Infinity::ListTables)
        .def("ShowTable", &Infinity::ShowTable)
        .def("ShowColumns", &Infinity::ShowColumns)
        .def("ListTableIndexes", &Infinity::ListTableIndexes)
        .def("ShowTables", &Infinity::ShowTables)
        .def("GetTable", &Infinity::GetTable)

        .def("CreateIndex", &Infinity::CreateIndex)
        .def("DropIndex", &Infinity::DropIndex)
        .def("ShowIndex", &Infinity::ShowIndex)
        .def("ShowSegment", &Infinity::ShowSegment)
        .def("ShowSegments", &Infinity::ShowSegments)
        .def("ShowBlock", &Infinity::ShowBlock)
        .def("ShowBlocks", &Infinity::ShowBlocks)
        .def("ShowBlockColumn", &Infinity::ShowBlockColumn)

        .def("Insert", &Infinity::Insert)
        .def("Import", &Infinity::Import)
        .def("Delete", &Infinity::Delete)
        .def("Update", &Infinity::Update)
        .def("Explain", &Infinity::Explain)
        .def("Search", &Infinity::Search)
        .def("Optimize", &Infinity::Optimize);

}
