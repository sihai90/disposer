//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__exec_fn__hpp_INCLUDED_
#define _disposer__core__exec_fn__hpp_INCLUDED_

#include "input_name.hpp"
#include "output_name.hpp"
#include "parameter_name.hpp"

#include "../tool/add_log.hpp"
#include "../tool/false_c.hpp"


namespace disposer{


	/// \brief Reference to a component or empty struct
	template < typename Component >
	struct optional_component{
		constexpr optional_component(
			Component& component,
			std::size_t& usage_count
		)noexcept
			: component(component)
			, usage_count(usage_count) {}

		constexpr optional_component(optional_component const& other)noexcept
			: component(other.component)
			, usage_count(other.usage_count) {}

		Component& component;
		std::size_t& usage_count;
	};

	/// \brief Empty struct
	template <>
	struct optional_component< void >{};


	template < typename Component >
	struct optional_component_accessory{
		optional_component_accessory(optional_component< Component > c)
			: component(c.component.accessory){}

		decltype(std::declval< Component >().accessory) component;
	};

	template <>
	struct optional_component_accessory< void >{
		optional_component_accessory(optional_component< void >){}
	};


	/// \brief Accessory of a module during exec calls
	template <
		typename TypeList,
		typename State,
		typename ExecInputs,
		typename ExecOutputs,
		typename Parameters,
		typename Component >
	class module_accessory
		: public optional_component_accessory< Component >
		, public add_log< module_accessory<
			TypeList, State, ExecInputs, ExecOutputs, Parameters, Component > >
	{
	public:
		/// \brief Constructor
		module_accessory(
			std::size_t id,
			TypeList,
			State* state,
			ExecInputs& inputs,
			ExecOutputs& outputs,
			Parameters const& parameters,
			std::string_view location,
			optional_component< Component > component
		)noexcept
			: optional_component_accessory< Component >(component)
			, id_(id)
			, state_(state)
			, inputs_(inputs)
			, outputs_(outputs)
			, parameters_(parameters)
			, location_(location) {}


		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename Name >
		auto const& operator()(Name const& name)const noexcept{
			return get(*this, name);
		}

		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename Name >
		auto& operator()(Name const& name)noexcept{
			return get(*this, name);
		}

		/// \brief Get type by dimension index
		template < std::size_t DI >
		static constexpr auto dimension(hana::size_t< DI > i)noexcept{
			static_assert(DI < TypeList::type_count,
				"module has less then DI dimensions");
			return TypeList::types[i];
		}

		/// \brief Get access to the state object if one exists
		auto& state()noexcept{
			static_assert(!std::is_void_v< State >, "Module has no state.");
			return *state_;
		}

		/// \brief ID of this exec
		std::size_t id()noexcept{ return id_; }


		/// \brief Implementation of the log prefix
		void log_prefix(log_key&&, logsys::stdlogb& os)const{
			os << location_;
		}


	private:
		template < typename This, typename Name >
		static auto& get(This& this_, Name const& name)noexcept{
			using name_t = std::remove_reference_t< Name >;
			if constexpr(hana::is_a< input_name_tag, name_t >()){
				auto const index = hana::index_if(this_.inputs_,
					[name](auto const& p){ return p.name == name; });

				static_assert(!hana::is_nothing(index),
					"input name doesn't exist");

				return this_.inputs_[*index];
			}else if constexpr(hana::is_a< output_name_tag, name_t >()){
				auto const index = hana::index_if(this_.outputs_,
					[name](auto const& p){ return p.name == name; });

				static_assert(!hana::is_nothing(index),
					"output name doesn't exist");

				return this_.outputs_[*index];
			}else if constexpr(hana::is_a< parameter_name_tag, name_t >()){
				auto const index = hana::index_if(this_.parameters_,
					[name](auto const& p){ return p.name == name; });

				static_assert(!hana::is_nothing(index),
					"parameter name doesn't exist");

				return this_.parameters_[*index].get();
			}else{
				static_assert(detail::false_c< Name >,
					"name is not an input_name, output_name or parameter_name");
			}
		}


		/// \brief Current exec id
		std::size_t id_;

		/// \brief Module state
		///
		/// nullptr-pointer to void if module is stateless.
		State* state_;

		/// \brief hana::tuple of exec_inputs
		ExecInputs& inputs_;

		/// \brief hana::tuple of exec_outputs
		ExecOutputs& outputs_;

		/// \brief hana::tuple of parameters
		Parameters const& parameters_;

		/// \brief Prefix for log messages
		std::string_view location_;
	};


	/// \brief Wrapper for the module exec function
	template < typename Fn >
	class exec_fn{
	public:
		constexpr exec_fn()
			noexcept(std::is_nothrow_default_constructible_v< Fn >)
			: fn_() {}

		constexpr explicit exec_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		constexpr explicit exec_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		template <
			typename TypeList,
			typename State,
			typename ExecInputs,
			typename ExecOutputs,
			typename Parameters,
			typename Component >
		void operator()(
			module_accessory< TypeList, State, ExecInputs, ExecOutputs,
				Parameters, Component >& accessory
		){
			// TODO: calulate noexcept
			if constexpr(
				std::is_invocable_v< Fn, module_accessory< TypeList, State,
					ExecInputs, ExecOutputs, Parameters, Component >& >
			){
				std::invoke(fn_, accessory);
			}else if constexpr(std::is_invocable_v< Fn >){
				(void)accessory; // silance GCC
				std::invoke(fn_);
			}else{
				static_assert(detail::false_c< State >,
					"Fn must be invokable with module_accessory& or "
					"without arguments");
			}
		}


	private:
		Fn fn_;
	};


}


#endif
