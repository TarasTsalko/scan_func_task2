#pragma once

#include <cstddef>
#include <string>
#include <tuple>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

std::ostream& operator<<(std::ostream& os, const scan_error &rhv )
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
        using NonCV_T = std::remove_cv_t<T>;
        NonCV_T &val = const_cast<NonCV_T&>( std::get<index>( values_ ) );
        val = std::move( val_ );
    }

    std::tuple<Ts...> values_;
};

} // namespace stdx::details
