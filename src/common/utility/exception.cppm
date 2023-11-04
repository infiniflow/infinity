//
// Created by jinhai on 23-10-15.
//
module;

export module infinity_exception;

import stl;

namespace infinity {

export class Exception : public StdException {
public:
    explicit Exception(String message) : message_(Move(message)) {}
    [[nodiscard]] inline const char *what() const noexcept override { return message_.c_str(); }

protected:
    template <typename... Args>
    static String BuildMessage(Args... params);

private:
    template <typename T, typename... Args>
    static String BuildMessageInternal(Vector<String> &values, T param, Args... params);

    static String BuildMessageInternal(Vector<String> &values);

    String message_;
};

String Exception::BuildMessageInternal(Vector<String> &values) {
    auto values_count = values.size();
    if (values_count > 0) {
        String msg(values[0]);
        for (SizeT idx = 1; idx < values_count; ++idx) {
            msg.append(" ").append(values[idx]);
        }
        return msg;
    }
    return String();
}

template <typename... Args>
String Exception::BuildMessage(Args... params) {
    Vector<String> values;
    return BuildMessageInternal(values, params...);
}

template <typename T, typename... Args>
String Exception::BuildMessageInternal(Vector<String> &values, T param, Args... params) {
    values.push_back(Move(param));
    return BuildMessageInternal(values, params...);
}

export class ClientException : public Exception {
public:
    template <typename... Args>
    explicit ClientException(Args... params) : Exception(BuildMessage(String("Client Error:"), params...)) {}
};

export class CatalogException : public Exception {
public:
    template <typename... Args>
    explicit CatalogException(Args... params) : Exception(BuildMessage(String("Catalog Error:"), params...)) {}
};

export class NetworkException : public Exception {
public:
    template <typename... Args>
    explicit NetworkException(Args... params) : Exception(BuildMessage(String("Network Error:"), params...)) {}
};

export class PlannerException : public Exception {
public:
    template <typename... Args>
    explicit PlannerException(Args... params) : Exception(BuildMessage(String("Planner Error:"), params...)) {}
};

export class OptimizerException : public Exception {
public:
    template <typename... Args>
    explicit OptimizerException(Args... params) : Exception(BuildMessage(String("Optimizer Error:"), params...)) {}
};

export class ExecutorException : public Exception {
public:
    template <typename... Args>
    explicit ExecutorException(Args... params) : Exception(BuildMessage(String("Executor Error:"), params...)) {}
};

export class SchedulerException : public Exception {
public:
    template <typename... Args>
    explicit SchedulerException(Args... params) : Exception(BuildMessage(String("Scheduler Error:"), params...)) {}
};

export class StorageException : public Exception {
public:
    template <typename... Args>
    explicit StorageException(Args... params) : Exception(BuildMessage(String("Storage Error:"), params...)) {}
};

export class TypeException : public Exception {
public:
    template <typename... Args>
    explicit TypeException(Args... params) : Exception(BuildMessage(String("Type Error:"), params...)) {}
};

export class FunctionException : public Exception {
public:
    template <typename... Args>
    explicit FunctionException(Args... params) : Exception(BuildMessage(String("Function Error:"), params...)) {}
};

export class NotImplementException : public Exception {
public:
    template <typename... Args>
    explicit NotImplementException(Args... params) : Exception(BuildMessage(String("NotImplement Error:"), params...)) {}
};

export class TransactionException : public Exception {
public:
    template <typename... Args>
    explicit TransactionException(Args... params) : Exception(BuildMessage(String("Transaction Error:"), params...)) {}
};

#ifdef INFINITY_DEBUG

export template <typename ExceptionType>
inline void Assert(bool is_true,
                   const String &message,
                   const char *file_name = std::source_location::current().file_name(),
                   u32 line = std::source_location::current().line()) {
    if (!(is_true)) {
        String err_msg = message;
        err_msg.append(" @").append(infinity::TrimPath(file_name)).append(":").append(ToStr(line));
        throw ExceptionType(err_msg);
    }
}

export template <typename ExceptionType>
inline void
Error(const String &message, const char *file_name = std::source_location::current().file_name(), u32 line = std::source_location::current().line()) {
    return Assert<ExceptionType>(false, message, file_name, line);
}

#elif

export template <typename ExceptionType>
inline void Assert(bool is_true, const String &message) {
    if (!(is_true)) {
        String err_msg = message;
        throw ExceptionType(err_msg);
    }
}

export template <typename ExceptionType>
inline void Error(const String &message) {
    return Assert<ExceptionType>(false, message);
}

#endif

} // namespace infinity
