#pragma once
#include <memory>
#include <cstdint>
#include <string>
#include <sstream>
#include <string_view>
#include <optional>
#include <type_traits>

typedef uint32_t ErrorCode;

class Error {
public:
    Error() = default;

    template<class EC>
    Error(EC error_code, std::string_view error_message,
          std::optional<std::string> optional_message = std::nullopt) noexcept
        : m_error_code(static_cast<ErrorCode>(error_code)),
          m_error_message(error_message),
          m_optional_message(std::move(optional_message)) {
    }

    Error(const Error &e) noexcept
        : m_error_code(e.m_error_code),
          m_error_message(e.m_error_message),
          m_optional_message(e.m_optional_message) {
    }

    Error(Error &&e) noexcept
        : m_error_code(e.m_error_code),
          m_error_message(e.m_error_message),
          m_optional_message(std::move(e.m_optional_message)) {
    }

    Error &operator=(const Error &e) noexcept {
        m_error_code = e.m_error_code;
        m_error_message = e.m_error_message;
        m_optional_message = e.m_optional_message;
        return *this;
    }

    Error &operator=(Error &&e) noexcept {
        m_error_code = e.m_error_code;
        m_error_message = e.m_error_message;
        m_optional_message = std::move(e.m_optional_message);
        return *this;
    }

public:
    [[nodiscard]] ErrorCode error_code() const noexcept {
        return m_error_code;
    }

    Error &error_code(ErrorCode error_code) noexcept {
        m_error_code = error_code;
        return *this;
    }

    [[nodiscard]] std::string_view error_message() const noexcept {
        return m_error_message;
    }

    Error &error_message(std::string_view error_message) noexcept {
        m_error_message = error_message;
        return *this;
    }

    [[nodiscard]] const std::optional<std::string> &optional_message() const noexcept {
        return m_optional_message;
    }

    Error &optional_message(std::optional<std::string> optional_message) noexcept {
        m_optional_message = optional_message;
        return *this;
    }

    [[nodiscard]] Error add_optional_message(std::optional<std::string> optional_message) const noexcept {
        Error temp(*this);
        temp.m_optional_message = std::move(optional_message);
        return temp;
    }

public:
    [[nodiscard]] std::string format() const {
        std::stringstream ss;
        std::string opmess = m_optional_message ? m_optional_message.value() : "";
        ss << "Error Code: " << m_error_code
                << ", Error Message: " << m_error_message
                << (m_optional_message ? ", Optional Message:" + m_optional_message.value() : "");
        return ss.str();
    }

private:
    ErrorCode m_error_code;
    std::string_view m_error_message;
    std::optional<std::string> m_optional_message;
};

// 要求是void类型或者是非错误类型的对象
template<class T>
constexpr bool is_valid_body_type = std::is_void_v<T> || std::is_object_v<T>;

template<class E>
constexpr bool is_valid_error_type = !std::is_void_v<E> && std::is_object_v<E>;

template<class T, class E = Error>
class Result {
    static_assert(is_valid_body_type<T>, "Invalid body type!");
    static_assert(is_valid_error_type<E>, "Invalid error type!");
    // 如果T和E是同一类型，那么构造函数就无法进行重载
    static_assert(!std::is_same_v<T, E>, "Body and error should be different type!");

    using acceptable_type = std::conditional_t<std::is_void_v<T>, void *, T>;
    using pointer = T *;
    using left_reference = std::conditional_t<std::is_void_v<T>, void *, std::add_lvalue_reference_t<T> >;

public:
    // 默认都是OK
    Result() noexcept
        : m_body(std::make_shared<acceptable_type>()),
          m_error(std::nullopt),
          m_is_ok(true) {
    }

    // 对于空类型，这里就不能传入void&&，所以要定义成void*传入，
    // 当然，在实际情况中，这个void构造直接传入`nullptr`即可.
    // 注意：由于make_shared无法构造void类型的shared_ptr，
    // 因此，这里仍然会先make_shared<void*>(nullptr)，构造出一个shared_ptr<void*>
    // 再移动构造，转换成m_body的shared_ptr<void>
    explicit Result(acceptable_type &&t) noexcept
        : m_body(std::make_shared<acceptable_type>(std::forward<acceptable_type>(t))),
          m_error(std::nullopt),
          m_is_ok(true) {
    }

    explicit Result(E &&e) noexcept
        : m_body(nullptr),
          m_error(std::forward<E>(e)),
          m_is_ok(false) {
    }

    Result(const Result &result) noexcept
        : m_body(result.m_body),
          m_error(result.m_error),
          m_is_ok(result.m_is_ok) {
    }

    Result(Result &&result) noexcept
        : m_body(std::move(result.m_body)),
          m_error(std::move(result.m_error)),
          m_is_ok(result.m_is_ok) {
    }

public:
    Result &operator=(const Result &result) noexcept {
        m_body = result.m_body;
        m_error = result.m_error;
        m_is_ok = result.m_is_ok;
        return *this;
    }

    Result &operator=(Result &&result) noexcept {
        m_body = std::move(result.m_body);
        m_error = std::move(result.m_error);
        m_is_ok = result.m_is_ok;
        return *this;
    }

public:
    static Result make_ok() {
        return Result();
    }

    static Result make_ok(acceptable_type &&actp) {
        return Result(std::forward<acceptable_type>(actp));
    }

    template<class... Args>
    static Result make_ok(Args &&... args) {
        return Result(acceptable_type(std::forward<Args>(args)...));
    }

    static Result make_err() {
        return Result(E());
    }

    static Result make_err(E &&e) {
        return Result(std::forward<E>(e));
    }

    template<class... Args>
    static Result make_err(Args &&... args) {
        return Result(E(std::forward<Args>(args)...));
    }

public:
    [[nodiscard]] const std::optional<bool> &result_status() const noexcept {
        return m_is_ok;
    }

    [[nodiscard]] bool is_ok() const noexcept {
        return m_is_ok.has_value() && m_is_ok.value();
    }

    [[nodiscard]] bool is_err() const noexcept {
        return m_is_ok.has_value() && !m_is_ok.value();
    }

    // 一般情况下不会出现undefined，这里也是提供一个防御性编程的接口
    [[nodiscard]] bool is_undefined() const noexcept {
        return !m_is_ok.has_value();
    }

public:
    std::shared_ptr<acceptable_type> shared_ok() const noexcept {
        if (!m_is_ok) {
            return nullptr;
        }
        return m_body;
    }

    std::optional<acceptable_type> ok() const noexcept {
        if (!m_is_ok) {
            return std::nullopt;
        }
        return *m_body;
    }

    const std::optional<E> &error() const noexcept {
        return m_error;
    }

    acceptable_type unwrap() const {
        return *m_body;
    }

    E unwrap_err() const {
        return m_error.value();
    }

private:
    std::shared_ptr<acceptable_type> m_body = nullptr;
    std::optional<E> m_error = std::nullopt;
    std::optional<bool> m_is_ok = std::nullopt;
};
