//
// Created by JinHai on 2022/9/4.
//

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace infinity {

enum class ExceptionType {
    kInvalid,

    // Exception from function module
    kNetwork,
    kParser,
    kPlanner,
    kOptimizer,
    kExecutor,
    kScheduler,
    kCatalog,

    // Type exception
    kType,

    // Other exception reason
    kNotImplemented,
};

class Exception : public std::exception {
public:
    explicit Exception(std::string message) : message_(std::move(message)) {}
    [[nodiscard]] inline const char* what() const noexcept override {
        return message_.c_str();
    }

protected:
    template<typename... Args>
    static std::string
    BuildMessage(Args... params);

private:

    template<typename T, typename... Args>
    static std::string
    BuildMessageInternal(std::vector<std::string>& values, T param, Args... params);

    static std::string
    BuildMessageInternal(std::vector<std::string>& values);

    std::string message_;

};

template<typename... Args>
std::string
Exception::BuildMessage(Args... params) {
    std::vector<std::string> values;
    return BuildMessageInternal(values, params...);
}

template<typename T, typename... Args>
std::string
Exception::BuildMessageInternal(std::vector<std::string>& values, T param, Args... params) {
    values.push_back(std::move(param));
    return BuildMessageInternal(values, params...);
}

class CatalogException: public Exception {
public:
    template<typename... Args>
    explicit CatalogException(Args... params) : Exception(BuildMessage(std::string("Catalog Error:"), params...)) {}
};

class NetworkException: public Exception {
public:
    template<typename... Args>
    explicit NetworkException(Args... params) : Exception(BuildMessage(std::string("Network Error:"), params...)) {}
};

class ParserException: public Exception {
public:
    template<typename... Args>
    explicit ParserException(Args... params) : Exception(BuildMessage(std::string("Parser Error:"), params...)) {}
};

class PlannerException: public Exception {
public:
    template<typename... Args>
    explicit PlannerException(Args... params) : Exception(BuildMessage(std::string("Planner Error:"), params...)) {}
};

class OptimizerException: public Exception {
public:
    template<typename... Args>
    explicit OptimizerException(Args... params) : Exception(BuildMessage(std::string("Optimizer Error:"), params...)) {}
};

class ExecutorException: public Exception {
public:
    template<typename... Args>
    explicit ExecutorException(Args... params) : Exception(BuildMessage(std::string("Executor Error:"), params...)) {}
};

class SchedulerException: public Exception {
public:
    template<typename... Args>
    explicit SchedulerException(Args... params) : Exception(BuildMessage(std::string("Scheduler Error:"), params...)) {}
};

class StorageException: public Exception {
public:
    template<typename... Args>
    explicit StorageException(Args... params) : Exception(BuildMessage(std::string("Storage Error:"), params...)) {}
};

class TypeException: public Exception {
public:
    template<typename... Args>
    explicit TypeException(Args... params) : Exception(BuildMessage(std::string("Type Error:"), params...)) {}
};

class NotImplementException: public Exception {
public:
    template<typename... Args>
    explicit NotImplementException(Args... params) : Exception(BuildMessage(std::string("NotImplement Error:"), params...)) {}
};

}


