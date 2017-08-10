//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__input_maker__hpp_INCLUDED_
#define _disposer__core__input_maker__hpp_INCLUDED_

#include "input.hpp"
#include "config_fn.hpp"

#include "../tool/validate_arguments.hpp"


namespace disposer{


	/// \brief Provids types for constructing an input
	template <
		typename InputType,
		typename ConnectionVerifyFn,
		typename TypeVerifyFn >
	struct input_maker{
		/// \brief Tag for boost::hana
		using hana_tag = input_maker_tag;

		/// \brief Input name as compile time string
		using name_type = typename InputType::name_type;

		/// \brief Name as hana::string
		static constexpr auto name = name_type::value;

		/// \brief Type of a disposer::input
		using type = InputType;

		/// \brief Function which verifies the connection with an output
		verify_connection_fn< ConnectionVerifyFn > verify_connection;

		/// \brief Function which verifies the active types
		verify_type_fn< TypeVerifyFn > verify_type;
	};


	/// \brief Helper function for \ref input_name::operator()
	template <
		typename Name,
		typename Types,
		typename TypeTransformFn,
		typename ConnectionVerifyFn,
		typename TypeVerifyFn >
	constexpr auto create_input_maker(
		Name const&,
		Types const&,
		type_transform_fn< TypeTransformFn >&&,
		verify_connection_fn< ConnectionVerifyFn >&& verify_connection,
		verify_type_fn< TypeVerifyFn >&& verify_type
	){
		constexpr auto typelist = to_typelist(Types{});

		constexpr auto unpack_types =
			hana::concat(hana::tuple_t< Name, TypeTransformFn >, typelist);

		constexpr auto type_input =
			hana::unpack(unpack_types, hana::template_< input >);

		return input_maker< typename decltype(type_input)::type,
			ConnectionVerifyFn, TypeVerifyFn >{
				std::move(verify_connection),
				std::move(verify_type)
			};
	}


	/// \brief Creates a \ref input_maker object
	template < char ... C, typename Types, typename ... Args >
	constexpr auto make(
		input_name< C ... >,
		Types const& types,
		Args&& ... args
	){
		detail::validate_arguments<
				type_transform_fn_tag,
				verify_connection_fn_tag,
				verify_type_fn_tag
			>(args ...);

		auto arg_tuple = hana::make_tuple(static_cast< Args&& >(args) ...);

		return create_input_maker(
			input_name< C ... >{},
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



}


#endif