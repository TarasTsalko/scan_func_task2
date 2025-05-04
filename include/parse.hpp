#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "types.hpp"


namespace stdx::details {

#define SCAN_UNUSED(x) (void)(x)

template<typename T>
concept IsInt_TType = (std::same_as<T, int8_t> || std::same_as<T, int16_t> || 
                       std::same_as<T, int32_t> || std::same_as<T, int64_t>);

template<typename T>
concept IsUInt_TType =  (std::same_as<T, uint8_t>  || std::same_as<T, uint16_t> || 
                         std::same_as<T, uint32_t> || std::same_as<T, uint64_t>);

template <typename T>
concept IsStringType =  std::same_as<T, std::string> || 
                        std::same_as<T, std::string_view>;


// здесь ваш код
template <typename T>
requires IsStringType<T>
constexpr std::expected<T, scan_error> parse_value( std::string_view input, std::string_view fmt ) {
    if ( !fmt.empty() && fmt != "%s" )
          return std::unexpected( scan_error{ "Unexpected format" } );  
    return std::string( input );
}

template <typename T>
requires std::floating_point<T>
constexpr std::expected<T, scan_error> parse_value( std::string_view input, std::string_view fmt ){ 
    if ( !fmt.empty() && fmt != "%f" )
         return std::unexpected( scan_error{ "Unexpected format" } );

    const char* begin = input.data();
    char* end;
    T retval = {};
    if ( std::same_as<T, double> )
        retval = std::strtod(begin, &end);
    else if ( std::same_as<T, float> )
        retval  = std::strtof(begin, &end);

    if (begin == end)
        return std::unexpected( scan_error{ "invalid input data to conertion" } );
    else if (std::isinf(retval)) 
        return std::unexpected( scan_error{ "overflow" } );
    return retval;
}

template <typename T>
requires IsInt_TType<T> || IsUInt_TType<T>
constexpr std::expected<T, scan_error> parse_value( std::string_view input, std::string_view fmt ){ 
    if ( !fmt.empty() && (fmt != "%d" && fmt !="%u" ) )
         return std::unexpected( scan_error{ "Unexpected format" } );

    const char* begin = input.data();
    char* end;
    T retval = {};
    if ( IsInt_TType<T> )
        retval = static_cast<T>( std::strtol(begin, &end, 10) );
    else if ( IsUInt_TType<T> )
        retval = static_cast<T>( std::strtoul(begin, &end, 10) );
    
    if (begin == end)
        return std::unexpected( scan_error{ "invalid input data to conertion" } );
    const bool range_error = errno == ERANGE;
    if ( range_error )
        return std::unexpected( scan_error{ "overflow" } );
    return retval;
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {

    using NonCV_T = std::remove_cv_t<T>;
    return parse_value<NonCV_T>( input, fmt ); 
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1 ));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

template <std::size_t I, typename... Ts>
struct type_at; 

template <std::size_t I, typename T, typename... Ts>
struct type_at<I, T, Ts...> : type_at<I - 1, Ts...> {};

template <typename T, typename... Ts>
struct type_at<0, T, Ts...> {
    using type = T;
};

template <size_t I, typename... Ts>
struct ParseHelper
{
    std::expected<bool, scan_error> operator()( const std::vector<std::string_view>& input_parts, const std::vector<std::string_view>& format_parts,
                                                scan_result<Ts...>& results )
    {
        const size_t index = sizeof...(Ts) - I;
        if ( input_parts.size() <= index )
            return std::unexpected( scan_error{ "Different placeholders count and input types" });

        using T = details::type_at<index, Ts...>::type;
        const auto res = stdx::details::parse_value_with_format<T>( input_parts[ index ], format_parts[ index ] );
        if ( res.has_value() ){
            results.template value<T, index>( res.value() );
            return ParseHelper<I - 1, Ts...>()( input_parts, format_parts, results );
        }
        else {
            return std::unexpected( std::move( res.error() ) );
        }
    }
};
 
template <typename... Ts>
struct ParseHelper<0, Ts...>
{
    std::expected<bool, scan_error> operator()( const std::vector<std::string_view>& input_parts, const std::vector<std::string_view>& format_parts,
                                                scan_result<Ts...>& results )
    {
        SCAN_UNUSED( input_parts );
        SCAN_UNUSED( format_parts );
        SCAN_UNUSED( results );
        return true;
    }
};

} // namespace stdx::details