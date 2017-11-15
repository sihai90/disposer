//-----------------------------------------------------------------------------
// Copyright (c) 2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__parser_fn__hpp_INCLUDED_
#define _disposer__core__parser_fn__hpp_INCLUDED_

#include "../tool/print_if_supported.hpp"

#include <logsys/log.hpp>
#include <logsys/stdlogb.hpp>

#include <boost/hana/type.hpp>


namespace disposer{


	struct parser_fn_tag;

	template < typename Fn >
	class parser_fn{
	public:
		using hana_tag = parser_fn_tag;

		constexpr parser_fn()
			noexcept(std::is_nothrow_default_constructible_v< Fn >)
			: fn_() {}

		constexpr explicit parser_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		constexpr explicit parser_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		template < typename Accessory, typename T >
		static constexpr bool calc_noexcept()noexcept{
			static_assert(
				std::is_invocable_r_v< T, Fn const, Accessory const&,
					std::string_view, hana::basic_type< T > >,
				"Wrong function signature, expected: "
				"T f(auto const& iop, std::string_view value, "
				"hana::basic_type< T > type)"
			);

			return std::is_nothrow_invocable_v< Fn const, Accessory const&,
				std::string_view, hana::basic_type< T > >;
		}

		template < typename Accessory, typename T >
		T operator()(
			std::string_view parameter_name,
			Accessory const& accessory,
			std::string_view value,
			hana::basic_type< T > type
		)const noexcept(calc_noexcept< Accessory, T >()){
			return accessory.log(
				[parameter_name](logsys::stdlogb& os, T const* value){
					os << "parameter(" << parameter_name << ") parsed value";
					if(value){
						os << ": ";
						print_if_supported(os, *value);
					}
					os << " ["
						<< type_index::type_id< T >().pretty_name() << "]";
				}, [&]()noexcept(calc_noexcept< Accessory, T >())->T{
					return std::invoke(fn_, accessory, value, type);
				});
		}

	private:
		Fn fn_;
	};

	struct stream_parser_t{
		static void verify_istream(std::string_view value, std::istream& is){
			if(!is){
				is.clear();
				throw std::runtime_error("parsing of '" + std::string(value)
					+ "' failed on position " + std::to_string(is.tellg())
					+ ", rest: '"
					+ std::string(value.begin() + is.tellg(), value.end())
					+ "'");
			}

			if(
				!is.eof() &&
				static_cast< std::size_t >(is.tellg()) < value.size()
			){
				std::ostringstream os;
				for(char c = is.get(); is; c = is.get()) os << c;
				throw std::runtime_error("parsing of '" + std::string(value)
					+ "' not exhaustive, rest: '"
					+ os.str() + "'");
			}
		}

		template < typename Accessory, typename T >
		T operator()(
			Accessory const& /*accessory*/,
			std::string_view value,
			hana::basic_type< T > type
		)const{
			if constexpr(type == hana::type_c< std::string >){
				return std::string(value);
			}else{
				std::istringstream is((std::string(value)));
				if constexpr(
					std::is_same_v< T, char > ||
					std::is_same_v< T, signed char > ||
					std::is_same_v< T, unsigned char >
				){
					int result;
					is >> result;
					verify_istream(value, is);
					return static_cast< T >(result);
				}else{
					if constexpr(std::is_same_v< T, bool >){
						is >> std::boolalpha;
					}

					T result;
					is >> result;
					verify_istream(value, is);
					return result;
				}
			}
		}

		template < typename Accessory, typename T >
		std::optional< T > operator()(
			Accessory const& accessory,
			std::string_view value,
			hana::basic_type< std::optional< T > >
		)const{
			return (*this)(accessory, value, hana::type_c< T >);
		}
	};

	constexpr auto stream_parser = parser_fn< stream_parser_t >{};


}


#endif
