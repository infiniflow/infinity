#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <unordered_map>

namespace nb = nanobind;
using namespace nb::literals;


NB_MODULE(test_issue_ext, m) {
    // ------------------------------------
    // issue #279: dynamic_attr broken
    // ------------------------------------

    struct Component {
        virtual ~Component() = default;
    };

    struct Param : Component { };

    struct Model : Component {
        void add_param(const std::string &name, std::shared_ptr<Param> p) {
            params_[name] = std::move(p);
        }

        std::shared_ptr<Param> get_param(const std::string &name) {
            return params_.find(name) != params_.end() ? params_[name] : nullptr;
        }

        std::unordered_map<std::string, std::shared_ptr<Param>> params_;
    };

    struct ModelA : Model {
        ModelA() {
            add_param("a", std::make_shared<Param>());
            add_param("b", std::make_shared<Param>());
        }
    };

    nb::class_<Component>(m, "Component");
    nb::class_<Param, Component>(m, "ParamBase");
    nb::class_<Model, Component>(m, "Model", nb::dynamic_attr()).def(nb::init<>{})
        .def("_get_param", &Model::get_param, "name"_a)
        .def("_add_param", &Model::add_param, "name"_a, "p"_a);
    nb::class_<ModelA, Model>(m, "ModelA").def(nb::init<>{});

    /// Issue #307: move constructor unexpectedly called
    struct Example { std::string text; };

    nb::class_<Example>(m, "Example")
        .def(nb::init<const std::string&>())
        .def("__repr__",
             [](const Example& e) {
                 return std::string("Example(\"") + e.text + "\")";
             });

    m.def("process",
          [](const std::vector<Example>& v) {
              return v.size();
          }, nb::arg("v"));
}
