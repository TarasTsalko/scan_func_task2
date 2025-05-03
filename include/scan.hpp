#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <iostream>

namespace stdx {

// замените болванку функции scan на рабочую версию
template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    
    details::scan_result<Ts...> results;
    const auto parse_sources_res =  stdx::details::parse_sources<Ts...>( input, format );
    const size_t I = sizeof...( Ts );
    if ( parse_sources_res.has_value() )
    {
        const auto parse_res = details::ParseHelper<I, Ts...>()( parse_sources_res->second, parse_sources_res->first, results );
        if ( parse_res.has_value() )
           return results;
        return std::unexpected( std::move( parse_res.error() ) );
    }
    else {
        return std::unexpected( std::move( parse_sources_res.error() ) );
    }
}

} // namespace stdx
