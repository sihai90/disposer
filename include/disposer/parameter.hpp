//-----------------------------------------------------------------------------
// Copyright (c) 2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__parameter__hpp_INCLUDED_
#define _disposer__parameter__hpp_INCLUDED_

#include "parameter_name.hpp"


namespace disposer{


	template < typename Name, typename ... T >
	class parameter{
	public:
		static_assert(hana::is_a< parameter_name_tag, Name >);

		using name_type = Name;

		static constexpr std::string_view name{ name_type::value.c_str() };


		static constexpr auto types = hana::tuple_t< T ... >;

		static constexpr std::size_t type_count = sizeof...(T);


		static_assert(hana::length(types) == hana::length(hana::to_set(types)),
			"disposer::parameter needs all types T to be unique");

		static_assert(type_count != 0,
			"disposer::parameter needs at least on type");

		static_assert(!hana::any_of(types, hana::traits::is_const),
			"disposer::parameter types must not be const");

		static_assert(!hana::any_of(types, hana::traits::is_reference),
			"disposer::parameter types must not be references");


		template < typename ParserFunction >
		parameter(ParserFunction&& parser_fn, std::string const& value):
			type_value_map_(hana::make_map(hana::make_pair(
				hana::type_c< T >, parser_fn(value, hana::type_c< T >)) ...
			)){}


		decltype(auto) operator()()const{
			static_assert(type_count == 1);
			return (*this)(types[hana::int_c< 0 >]);
		}

		template < typename Type >
		decltype(auto) operator()(Type const& type)const{
			return type_value_map_[type];
		}


	private:
		decltype(hana::make_map(hana::make_pair(hana::type_c< T >,
			std::declval< T >()) ... )) const type_value_map_;
	};


	struct param_tag{};


	/// \brief Provid types for constructing an parameter
	template < typename Name, typename ParserFunction, typename ParameterType >
	struct param_t{
		/// \brief Tag for boost::hana
		using hana_tag = param_tag;

		/// \brief Parameter name as compile time string
		using name = Name;

		/// \brief Type of a disposer::parameter
		using type = ParameterType;

		/// \brief Parameter parser
		ParserFunction parser;
	};


	template < char ... C >
	template < typename Types, typename ParserFunction >
	constexpr auto parameter_name< C ... >::operator()(
		Types const& types,
		ParserFunction&& parser_fn
	)const noexcept{
		using name_type = parameter_name< C ... >;

		constexpr auto is_callable_with =
			hana::is_valid([](auto&& parser_fn_type, auto&& type)->
				decltype((void)std::declval<
					typename std::decay_t< decltype(parser_fn_type) >::type >()
					(std::declval< std::string >(), type)){});

		constexpr auto is_parser_callable_with =
			hana::curry< 2 >(is_callable_with)(hana::type_c< ParserFunction >);


		if constexpr(hana::is_a< hana::type_tag, Types >){
			static_assert(is_parser_callable_with(Types{}),
				"need function signature 'T parser_fn(std::string, "
				"hana::basic_type< T >)'");

			using result_type =
				decltype(parser_fn(std::declval< std::string >(), types));

			static_assert(std::is_same_v< typename Types::type, result_type >,
				"need function signature 'T parser_fn(std::string, "
				"hana::basic_type< T >)' (wrong return type)");

			using type_parameter =
				parameter< name_type, typename Types::type >;

			return param_t< name_type, ParserFunction, type_parameter >{
				static_cast< ParserFunction&& >(parser_fn)};
		}else{
			static_assert(hana::Foldable< Types >::value);
			static_assert(hana::all_of(Types{}, hana::is_a< hana::type_tag >));

			static_assert(hana::all_of(Types{}, is_parser_callable_with),
				"need function signature 'T parser_fn(std::string, "
				"hana::basic_type< T >)'");

			constexpr auto valid_return_type = [](auto&& t){
				return std::is_same_v<
					typename decltype(+t)::type,
					decltype(std::declval< ParserFunction >()
						(std::declval< std::string >(), t)) >;
			};
			static_assert(hana::all_of(Types{}, valid_return_type),
				"need function signature 'T parser_fn(std::string, "
				"hana::basic_type< T >)' (wrong return type)");

			auto unpack_types = hana::concat(
				hana::tuple_t< name_type >,
				hana::to_tuple(types));

			auto type_parameter =
				hana::unpack(unpack_types, hana::template_< parameter >);

			return param_t< name_type, ParserFunction,
				typename decltype(type_parameter)::type >{
				static_cast< ParserFunction&& >(parser_fn)};
		}
	}


}


#endif
