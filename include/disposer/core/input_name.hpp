//-----------------------------------------------------------------------------
// Copyright (c) 2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__input_name__hpp_INCLUDED_
#define _disposer__core__input_name__hpp_INCLUDED_

#include "input_maker.hpp"

#include "../tool/ct_name.hpp"
#include "../tool/validate_arguments.hpp"


namespace disposer{


	/// \brief A compile time string type for inputs
	template < char ... C >
	struct input_name: ct_name< C ... >{
		/// \brief Hana tag to identify input names
		using hana_tag = input_name_tag;

		/// \brief Creates a \ref input_maker object
		template < typename Types, typename ... Args >
		constexpr auto operator()(Types const& types, Args&& ... args)const{
			detail::validate_arguments<
					type_transform_fn_tag,
					verify_connection_fn_tag,
					verify_type_fn_tag
				>(args ...);

			auto arg_tuple = hana::make_tuple(static_cast< Args&& >(args) ...);

			{
				auto const count = hana::count_if(arg_tuple,
					hana::is_a< type_transform_fn_tag >) <= hana::size_c< 1 >;
				static_assert(count, "more than one type_transform_fn");
			}
			{
				auto const count = hana::count_if(arg_tuple,
					hana::is_a< verify_connection_fn_tag >) <= hana::size_c< 1 >;
				static_assert(count, "more than one verify_connection_fn");
			}
			{
				auto const count = hana::count_if(arg_tuple,
					hana::is_a< verify_type_fn_tag >) <= hana::size_c< 1 >;
				static_assert(count, "more than one verify_type_fn");
			}

			return create_input_maker(
				(*this),
				types,
				get_or_default(std::move(arg_tuple),
					hana::is_a< type_transform_fn_tag >,
					no_type_transform),
				get_or_default(std::move(arg_tuple),
					hana::is_a< verify_connection_fn_tag >,
					required),
				get_or_default(std::move(arg_tuple),
					hana::is_a< verify_type_fn_tag >,
					verify_type_always)
			);
		}
	};

	/// \brief Make a \ref input_name object
	template < char ... C >
	input_name< C ... > input_name_c{};


	/// \brief Make a \ref input_name object by a hana::string object
	template < char ... C > constexpr input_name< C ... >
	to_input_name(hana::string< C ... >)noexcept{ return {}; }


}

#define DISPOSER_INPUT(s) \
	::disposer::to_input_name(BOOST_HANA_STRING(s))


#ifdef BOOST_HANA_CONFIG_ENABLE_STRING_UDL
namespace disposer::literals{


	/// \brief Make a \ref input_name object via a user defined literal
	template < typename CharT, CharT ... c >
	constexpr auto operator"" _in(){
		static_assert(std::is_same_v< CharT, char >);
		return input_name_c< c ... >;
	}


}
#endif


#endif
