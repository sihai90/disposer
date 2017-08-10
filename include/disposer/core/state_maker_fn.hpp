//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__state_maker_fn__hpp_INCLUDED_
#define _disposer__core__state_maker_fn__hpp_INCLUDED_

#include "module_data.hpp"

#include "../tool/add_log.hpp"
#include "../tool/false_c.hpp"


namespace disposer{


	/// \brief Accessory of a module during enable/disable calls
	template < typename List >
	class state_accessory: public add_log< state_accessory< List > >{
	public:
		/// \brief Constructor
		state_accessory(
			module_data< List > const& data,
			std::string_view location
		)
			: data_(data)
			, location_(location) {}


		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename Name >
		auto const& operator()(Name const& name)const noexcept{
			return data_(name);
		}


		/// \brief Implementation of the log prefix
		void log_prefix(log_key&&, logsys::stdlogb& os)const{
			os << location_;
		}


	private:
		/// \brief Reference to the module object
		module_data< List > const& data_;

		/// \brief Location for log messages
		std::string_view location_;
	};


	/// \brief Wrapper for the module enable function
	template < typename StateMakerFn >
	class state_maker_fn{
	public:
		state_maker_fn(StateMakerFn&& maker_fn)
			: maker_fn_(static_cast< StateMakerFn&& >(maker_fn)) {}

		template < typename List >
		auto operator()(state_accessory< List > const& accessory)const{
			if constexpr(std::is_invocable_v< StateMakerFn const,
				state_accessory< List > const& >
			){
				static_assert(!std::is_void_v< std::invoke_result_t<
					StateMakerFn const,
					state_accessory< List > const& > >,
					"StateMakerFn must not return void");
				return maker_fn_(accessory);
			}else if constexpr(std::is_invocable_v< StateMakerFn const >){
				static_assert(!std::is_void_v< std::invoke_result_t<
					StateMakerFn const > >,
					"StateMakerFn must not return void");
				(void)accessory; // silance GCC
				return maker_fn_();
			}else{
				static_assert(detail::false_c< StateMakerFn >,
					"StateMakerFn function must be invokable with "
					"state_accessory const& or without an argument");
			}
		}

	private:
		StateMakerFn maker_fn_;
	};

	/// \brief Maker specialization for stateless modules
	template <> class state_maker_fn< void >{};


	/// \brief Holds the user defined state object of a module
	template < typename List, typename StateMakerFn >
	class state{
	public:
		/// \brief Constructor
		state(state_maker_fn< StateMakerFn > const& state_maker_fn)noexcept
			: state_maker_fn_(state_maker_fn) {}

		/// \brief Enables the module for exec calls
		///
		/// Build a users state object.
		void enable(module_data< List > const& data, std::string_view location){
			state_.emplace(state_maker_fn_(
				state_accessory< List >(data, location)));
		}

		/// \brief Disables the module for exec calls
		void disable()noexcept{
			state_.reset();
		}


	private:
		/// \brief Type of the module state object
		using state_type = std::invoke_result_t<
			state_maker_fn< StateMakerFn >,
			state_accessory< List >&& >;

		static_assert(!std::is_void_v< state_type >,
			"state_maker function must not return void");


		/// \brief The function object that is called in enable()
		state_maker_fn< StateMakerFn > state_maker_fn_;

		/// \brief The function object that is called in exec()
		std::optional< state_type > state_;
	};


	/// \brief Specialization for stateless modules
	template < typename List >
	class state< List, void >{
	public:
		/// \brief Constructor
		state(state_maker_fn< void > const&)noexcept{}

		/// \brief Module is stateless, do nothing
		void enable(module_data< List > const&, std::string_view)noexcept{}

		/// \brief Module is stateless, do nothing
		void disable()noexcept{}
	};


}


#endif