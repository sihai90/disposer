//-----------------------------------------------------------------------------
// Copyright (c) 2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__config_fn__hpp_INCLUDED_
#define _disposer__config_fn__hpp_INCLUDED_

#include <type_traits>

#include <boost/hana/core/is_a.hpp>
#include <boost/hana/all_of.hpp>
#include <boost/hana/tuple.hpp>

#include "output_info.hpp"


namespace disposer{


	struct enable_always{
		template < typename IOP_List, typename T >
		constexpr bool operator()(
			IOP_List const& /* iop_list */,
			hana::basic_type< T > /*type*/
		)const noexcept{
			return true;
		}
	};

	template < typename Fn >
	class enable_fn{
	public:
		constexpr enable_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		constexpr enable_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		template < typename IOP_List, typename T >
		constexpr bool operator()(
			IOP_List const& iop_list,
			hana::basic_type< T > type
		)const noexcept(noexcept(fn_(iop_list, type))){
			return fn_(iop_list, type);
		}

	private:
		Fn const fn_;
	};

	template < typename Fn >
	constexpr auto enable(Fn&& fn)
		noexcept(std::is_nothrow_constructible_v< Fn, Fn&& >){
		return enable_fn< std::remove_reference_t< Fn > >(
			static_cast< Fn&& >(fn));
	}


	struct verify_connection_always{
		template < typename IOP_List >
		constexpr void operator()(
			IOP_List const& /* iop_list */,
			bool /*connected*/
		)const noexcept{}
	};

	template < typename Fn >
	class verify_connection_fn{
	public:
		constexpr verify_connection_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		constexpr verify_connection_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		template < typename IOP_List >
		constexpr void operator()(
			IOP_List const& iop_list,
			bool connected
		)const noexcept(noexcept(fn_(iop_list, connected))){
			fn_(iop_list, connected);
		}

	private:
		Fn const fn_;
	};

	template < typename Fn >
	constexpr auto verify_connection(Fn&& fn)
		noexcept(std::is_nothrow_constructible_v< Fn, Fn&& >){
		return verify_connection_fn< std::remove_reference_t< Fn > >(
			static_cast< Fn&& >(fn));
	}


	struct verify_type_always{
		template < typename IOP_List, typename T >
		constexpr void operator()(
			IOP_List const& /* iop_list */,
			hana::basic_type< T > /*type*/,
			output_info const& /*info*/
		)const noexcept{}
	};

	template < typename Fn >
	class verify_type_fn{
	public:
		constexpr verify_type_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		constexpr verify_type_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		template < typename IOP_List, typename T >
		constexpr void operator()(
			IOP_List const& iop_list,
			hana::basic_type< T > type,
			output_info const& info
		)const noexcept(noexcept(fn_(iop_list, type, info))){
			fn_(iop_list, type, info);
		}

	private:
		Fn const fn_;
	};

	template < typename Fn >
	constexpr auto verify_type(Fn&& fn)
		noexcept(std::is_nothrow_constructible_v< Fn, Fn&& >){
		return verify_type_fn< std::remove_reference_t< Fn > >(
			static_cast< Fn&& >(fn));
	}


	struct verify_value_always{
		template < typename IOP_List, typename T >
		constexpr void operator()(
			IOP_List const& /* iop_list */,
			T const& /*value*/
		)const noexcept{}
	};

	template < typename Fn >
	class verify_value_fn{
	public:
		constexpr verify_value_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		constexpr verify_value_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		template < typename IOP_List, typename T >
		constexpr void operator()(
			IOP_List const& iop_list,
			T const& value
		)const noexcept(noexcept(fn_(iop_list, value))){
			fn_(iop_list, value);
		}

	private:
		Fn const fn_;
	};

	template < typename Fn >
	constexpr auto verify_value(Fn&& fn)
		noexcept(std::is_nothrow_constructible_v< Fn, Fn&& >){
		return verify_value_fn< std::remove_reference_t< Fn > >(
			static_cast< Fn&& >(fn));
	}


	struct stream_parser{
		template < typename T >
		T operator()(
			std::string_view  value ,
			hana::basic_type< T > type
		)const{
			if constexpr(type == hana::type_c< std::string >){
				return std::string(value);
			}else{
				std::istringstream is((std::string(value)));
				T result;
				if constexpr(std::is_same_v< T, bool >){
					is >> std::boolalpha;
				}
				is >> result;
				return result;
			}
		}
	};

	template < typename Fn >
	class parser_fn{
	public:
		constexpr parser_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		constexpr parser_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		template < typename IOP_List, typename T >
		constexpr bool operator()(
			std::string_view value,
			hana::basic_type< T > type
		)const noexcept(noexcept(fn_(value, type))){
			return fn_(value, type);
		}

	private:
		Fn const fn_;
	};

	template < typename Fn >
	constexpr auto parser(Fn&& fn)
		noexcept(std::is_nothrow_constructible_v< Fn, Fn&& >){
		return parser_fn< std::remove_reference_t< Fn > >(
			static_cast< Fn&& >(fn));
	}


	template < typename T >
	using self_t = T;


	/// \brief Create a hana::tuple of hana::type's with a given hana::type or
	///        a hana::Sequence of hana::type's
	template < typename Types >
	constexpr auto to_typelist(Types const&)noexcept{
		if constexpr(hana::is_a< hana::type_tag, Types >){
			return hana::make_tuple(Types{});
		}else{
			static_assert(hana::Foldable< Types >::value);
			static_assert(hana::all_of(Types{}, hana::is_a< hana::type_tag >));
			return hana::to_tuple(Types{});
		}
	}


}


#endif
