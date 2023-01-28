//
// Created by JinHai on 2022/9/4.
//

#pragma once

#include "common/types/internal_types.h"

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
    kFunction,

    // Type exception
    kType,

    // Other exception reason
    kNotImplemented,
};

class Exception : public std::exception {
public:
    explicit Exception(String message) : message_(std::move(message)) {}
    [[nodiscard]] inline const char* what() const noexcept override {
        return message_.c_str();
    }

protected:
    template<typename... Args>
    static String
    BuildMessage(Args... params);

private:

    template<typename T, typename... Args>
    static String
    BuildMessageInternal(std::vector<String>& values, T param, Args... params);

    static String
    BuildMessageInternal(std::vector<String>& values);

    String message_;

};

template<typename... Args>
String
Exception::BuildMessage(Args... params) {
    std::vector<String> values;
    return BuildMessageInternal(values, params...);
}

template<typename T, typename... Args>
String
Exception::BuildMessageInternal(std::vector<String>& values, T param, Args... params) {
    values.push_back(std::move(param));
    return BuildMessageInternal(values, params...);
}

class ClientException: public Exception {
public:
    template<typename... Args>
    explicit ClientException(Args... params) : Exception(BuildMessage(String("Client Error:"), params...)) {}
};

class CatalogException: public Exception {
public:
    template<typename... Args>
    explicit CatalogException(Args... params) : Exception(BuildMessage(String("Catalog Error:"), params...)) {}
};

class NetworkException: public Exception {
public:
    template<typename... Args>
    explicit NetworkException(Args... params) : Exception(BuildMessage(String("Network Error:"), params...)) {}
};

class ParserException: public Exception {
public:
    template<typename... Args>
    explicit ParserException(Args... params) : Exception(BuildMessage(String("Parser Error:"), params...)) {}
};

class PlannerException: public Exception {
public:
    template<typename... Args>
    explicit PlannerException(Args... params) : Exception(BuildMessage(String("Planner Error:"), params...)) {}
};

class OptimizerException: public Exception {
public:
    template<typename... Args>
    explicit OptimizerException(Args... params) : Exception(BuildMessage(String("Optimizer Error:"), params...)) {}
};

class ExecutorException: public Exception {
public:
    template<typename... Args>
    explicit ExecutorException(Args... params) : Exception(BuildMessage(String("Executor Error:"), params...)) {}
};

class SchedulerException: public Exception {
public:
    template<typename... Args>
    explicit SchedulerException(Args... params) : Exception(BuildMessage(String("Scheduler Error:"), params...)) {}
};

class StorageException: public Exception {
public:
    template<typename... Args>
    explicit StorageException(Args... params) : Exception(BuildMessage(String("Storage Error:"), params...)) {}
};

class TypeException: public Exception {
public:
    template<typename... Args>
    explicit TypeException(Args... params) : Exception(BuildMessage(String("Type Error:"), params...)) {}
};

class FunctionException: public Exception {
public:
    template<typename... Args>
    explicit FunctionException(Args... params) : Exception(BuildMessage(String("Function Error:"), params...)) {}
};

class NotImplementException: public Exception {
public:
    template<typename... Args>
    explicit NotImplementException(Args... params) : Exception(BuildMessage(String("NotImplement Error:"), params...)) {}
};

}


