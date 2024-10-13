#include <string>
#include <unordered_map>
#include <iostream>

#include <result.hpp>

enum NewErrorCode : ErrorCode {
    NewOne = 10002
};

static const Error error_example = Error(10001, "An Error Message!");
static const Error error_example_with_optional_message = Error(NewErrorCode::NewOne, "An Error Message!", "Opt!");

int main() {
    auto result_ok_void1 = Result<void>::make_ok();
    std::cout << "result_ok_void1: " << result_ok_void1.ok().value() << std::endl;
    std::cout << "result_ok_void1: " << result_ok_void1.unwrap() << std::endl;
    std::cout << "result_ok_void1: " << (
        result_ok_void1.error() ? result_ok_void1.error().value().format() : "No Error") << std::endl;

    auto result_ok_void2 = Result<void>::make_ok((void *) 0x66666666);
    std::cout << "result_ok_void2: " << result_ok_void2.ok().value() << std::endl;
    std::cout << "result_ok_void2: " << result_ok_void2.shared_ok() << std::endl;
    std::cout << "result_ok_void2: " << *result_ok_void2.shared_ok() << std::endl;
    std::cout << "result_ok_void2: " << result_ok_void2.unwrap() << std::endl;
    std::cout << "result_ok_void2: " << (
        result_ok_void2.error() ? result_ok_void2.error().value().format() : "No Error") << std::endl;
    // std::cout << result_void2.unwrap_err().error_message() << std::endl;

    auto result_error_void1 = Result<void>::make_err(error_example);
    std::cout << "result_error_void1: " << result_error_void1.error().value().format() << std::endl;
    std::cout << "result_error_void1: " << result_error_void1.unwrap_err().add_optional_message("Optional Message!").
            format() << std::endl;
    //std::cout << result_error_void1.unwrap() << std::endl;

    auto result_ok_string1 = Result<std::string>::make_ok("123456");
    std::cout << "result_ok_string1: " << result_ok_string1.unwrap() << std::endl;
    std::cout << "result_ok_string1: " << result_ok_string1.shared_ok() << std::endl;
    std::cout << "result_ok_string1: " << *result_ok_string1.shared_ok() << std::endl;

    auto result_ok_string2 = Result<std::string>::make_ok(6, 'a');
    std::cout << "result_ok_string2: " << result_ok_string2.ok().value() << std::endl;

    auto result_ok_string3 = Result<std::string>::make_ok();
    std::cout << "result_ok_string3: " << result_ok_string3.unwrap() << std::endl;

    auto result_error_string1 = Result<std::string>::make_err(error_example_with_optional_message);
    std::cout << "result_error_string1: " << result_error_string1.unwrap_err().format() << std::endl;

    auto result_error_string2 = Result<std::string>::make_err();
    std::cout << "result_error_string2: " << result_error_string2.unwrap_err().format() << std::endl;

    auto result_ok_int1 = Result<int>::make_ok(12138);
    std::cout << "result_ok_int1: " << result_ok_int1.unwrap() << std::endl;
    std::cout << "result_ok_int1: " << result_ok_int1.shared_ok() << std::endl;
    std::cout << "result_ok_int1: " << *result_ok_int1.shared_ok() << std::endl;

    auto result_ok_int2 = Result<int>::make_ok();
    std::cout << "result_ok_int2: " << result_ok_int2.unwrap() << std::endl;
    std::cout << "result_ok_int2: " << result_ok_int2.shared_ok() << std::endl;
    std::cout << "result_ok_int2: " << *result_ok_int2.shared_ok() << std::endl;

    auto result_ok_unordered_map1 = Result<std::unordered_map<std::string, int> >::make_ok(
        std::unordered_map<std::string, int>({{"k1", 1,}, {"k2", 2}}));
    std::cout << "result_ok_unordered_map1: " << result_ok_unordered_map1.shared_ok() << std::endl; {
        auto &mp1 = *result_ok_unordered_map1.shared_ok();
        for (auto &iter: mp1) {
            std::cout << "result_ok_unordered_map1: item: " << iter.first << ": " << iter.second << std::endl;
        }
    }

    auto result_string_view_error_string1 = Result<std::string, std::string_view>::make_err(
        "string_view error message!");
    std::cout << "result_string_view_error_string1: " << result_string_view_error_string1.unwrap_err() << std::endl;

    return 0;
}
