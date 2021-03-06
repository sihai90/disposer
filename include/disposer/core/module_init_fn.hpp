//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__module_init_fn__hpp_INCLUDED_
#define _disposer__core__module_init_fn__hpp_INCLUDED_

#include "module_data.hpp"


namespace disposer{


	/// \brief Ref of a module during enable/disable calls
	template <
		typename TypeList,
		typename Inputs,
		typename Outputs,
		typename Parameters,
		typename ComponentRef >
	class module_init_ref
		: public optional_component< ComponentRef >
		, public logsys::log_base{
	public:
		/// \brief Constructor
		module_init_ref(
			module_data< TypeList, Inputs, Outputs, Parameters > const& data,
			std::string&& log_prefix,
			optional_component< ComponentRef > component
		)
			: optional_component< ComponentRef >(component)
			, logsys::log_base(std::move(log_prefix))
			, data_(data) {}


		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename Name >
		decltype(auto) operator()(Name const& name)const noexcept{
			using name_t = std::remove_reference_t< Name >;
			if constexpr(hana::is_a< input_name_tag, name_t >()){
				auto const index = hana::index_if(data_.inputs,
					[name](auto const& p){ return p.name == name; });

				static_assert(!hana::is_nothing(index),
					"input name doesn't exist");

				return data_.inputs[*index].output_ptr() != nullptr;
			}else if constexpr(hana::is_a< output_name_tag, name_t >()){
				auto const index = hana::index_if(data_.outputs,
					[name](auto const& p){ return p.name == name; });

				static_assert(!hana::is_nothing(index),
					"output name doesn't exist");

				return data_.outputs[*index].use_count() > 0;
			}else if constexpr(hana::is_a< parameter_name_tag, name_t >()){
				auto const index = hana::index_if(data_.parameters,
					[name](auto const& p){ return p.name == name; });

				static_assert(!hana::is_nothing(index),
					"parameter name doesn't exist");

				return data_.parameters[*index].get();
			}else{
				static_assert(detail::false_c< Name >,
					"name is not an input_name, output_name or parameter_name");
			}
		}

		/// \brief Get type by dimension index
		template < std::size_t DI >
		static constexpr auto dimension(hana::size_t< DI > i)noexcept{
			static_assert(DI < TypeList::type_count,
				"module has less then DI dimensions");
			return TypeList::types[i];
		}


	private:
		/// \brief Reference to the module object
		module_data< TypeList, Inputs, Outputs, Parameters > const& data_;
	};


	/// \brief Wrapper for the module enable function
	template < typename Fn = void >
	class module_init_fn{
	public:
		constexpr module_init_fn()
			noexcept(std::is_nothrow_default_constructible_v< Fn >)
			: fn_() {}

 		constexpr explicit module_init_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

 		constexpr explicit module_init_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}

		template <
			typename TypeList,
			typename Inputs,
			typename Outputs,
			typename Parameters,
			typename ComponentRef >
		auto operator()(
			module_init_ref< TypeList, Inputs, Outputs, Parameters,
				ComponentRef > ref
		)const{
			// TODO: calulate noexcept
			if constexpr(std::is_invocable_v< Fn const, module_init_ref<
				TypeList, Inputs, Outputs, Parameters, ComponentRef > >
			){
				static_assert(!std::is_void_v< std::invoke_result_t<
					Fn const, module_init_ref< TypeList,
						Inputs, Outputs, Parameters, ComponentRef > > >,
					"Fn must not return void");
				return std::invoke(fn_, ref);
			}else if constexpr(std::is_invocable_v< Fn const >){
				static_assert(!std::is_void_v< std::invoke_result_t<
					Fn const > >,
					"Fn must not return void");
				(void)ref; // silance GCC
				return std::invoke(fn_);
			}else{
				static_assert(detail::false_c< Fn >,
					"Fn function must be const invokable with "
					"module_init_ref or without an argument");
			}
		}

	private:
		Fn fn_;
	};

	/// \brief Maker specialization for stateless modules
	template <> class module_init_fn< void >{};

	module_init_fn() -> module_init_fn< void >;


	/// \brief Holds the user defined state object of a module
	template < typename StateType, typename ModuleInitFn >
	class module_state{
	public:
		/// \brief Constructor
		module_state(
			module_init_fn< ModuleInitFn > const& module_init_fn
		)noexcept
			: module_init_fn_(module_init_fn) {}

		/// \brief Enables the module for exec calls
		///
		/// Build a users state object.
		template <
			typename TypeList,
			typename Inputs,
			typename Outputs,
			typename Parameters,
			typename ComponentRef >
		void enable(
			module_data< TypeList, Inputs, Outputs, Parameters > const& data,
			optional_component< ComponentRef > component,
			std::string&& log_prefix
		){
			state_.emplace(module_init_fn_(module_init_ref< TypeList,
				Inputs, Outputs, Parameters, ComponentRef >(
					data, std::move(log_prefix), component)));
		}

		/// \brief Disables the module for exec calls
		void disable()noexcept{
			state_.reset();
		}

		/// \brief Get pointer to state object
		StateType* object()noexcept{
			assert(state_);
			return &*state_;
		}


	private:
		/// \brief The function object that is called in enable()
		module_init_fn< ModuleInitFn > module_init_fn_;

		/// \brief The function object that is called in exec()
		std::optional< StateType > state_;
	};


	/// \brief Specialization for stateless modules
	template <>
	class module_state< void, void >{
	public:
		/// \brief Type of the module state object
		using state_type = void;

		/// \brief Constructor
		module_state(module_init_fn< void > const&)noexcept{}

		/// \brief Module is stateless, do nothing
		template <
			typename TypeList,
			typename Inputs,
			typename Outputs,
			typename Parameters,
			typename ComponentRef >
		void enable(
			module_data< TypeList, Inputs, Outputs, Parameters > const&,
			optional_component< ComponentRef >,
			std::string_view
		)noexcept{}

		/// \brief Module is stateless, do nothing
		void disable()noexcept{}

		/// \brief Module is stateless, return nullptr
		void* object()noexcept{ return nullptr; }
	};


}


#endif
