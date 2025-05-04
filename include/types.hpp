#pragma once

#include <cstddef>
#include <string>
#include <format>
#include <tuple>



namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
    const std::string& get_string() const {
        return message;
    }
};


inline std::ostream& operator<<(std::ostream& os, const scan_error &rhv )
{
    os << rhv.message;
    return os;
}

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {

    template<size_t index>
    auto value() const{
        using TupleT = std::tuple_element_t<index, decltype(values_)>;
        return static_cast<TupleT>(std::get<index>( values_ ));
    }

    template< typename T, size_t index>
    void value( T val_ ){
        using TupleElementType = std::tuple_element_t<index, decltype(values_)>;
        static_assert( std::same_as<TupleElementType, T>, "Incorrect type!" );
        TupleElementType &val = const_cast<TupleElementType&>( std::get<index>( values_ ) );
        val = std::move( val_ );
    }

    std::tuple<Ts...> values_;
};

} // namespace stdx::details

namespace std {
    template <>
    struct formatter<stdx::details::scan_error> : formatter<std::string> {
        auto format(const stdx::details::scan_error& value, format_context& ctx) {
            return formatter<string>::format(value.get_string(), ctx);
        }
    };
}

