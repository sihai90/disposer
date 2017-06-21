//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__module__hpp_INCLUDED_
#define _disposer__module__hpp_INCLUDED_

#include "output_info.hpp"
#include "input.hpp"
#include "output.hpp"
#include "parameter.hpp"
#include "module_base.hpp"
#include "add_log.hpp"
#include "module_make_data.hpp"

#include <type_traits>
#include <atomic>


namespace disposer{


	/// \brief Accessory of a \ref module without log
	template <
		typename Inputs,
		typename Outputs,
		typename Parameters >
	class module_config{
	public:
		/// \brief Constructor
		module_config(
			Inputs&& inputs,
			Outputs&& outputs,
			Parameters&& parameters
		)
			: inputs_(std::move(inputs))
			, outputs_(std::move(outputs))
			, parameters_(std::move(parameters)) {}


		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename IOP >
		decltype(auto) operator()(IOP&& iop)noexcept{
			return get(*this, static_cast< IOP&& >(iop));
		}

		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename IOP >
		decltype(auto) operator()(IOP&& iop)const noexcept{
			return get(*this, static_cast< IOP&& >(iop));
		}


	private:
		/// \brief Implementation for \ref operator()
		template < typename Config, typename IOP >
		static decltype(auto) get(Config& config, IOP&&)noexcept{
			using io_t =
				std::remove_cv_t< std::remove_reference_t< IOP > >;
			static_assert(
				hana::is_a< input_name_tag, io_t > ||
				hana::is_a< output_name_tag, io_t > ||
				hana::is_a< parameter_name_tag, io_t >,
				"parameter io must be an input_name, an output_name or a"
				"parameter_name");

			if constexpr(hana::is_a< input_name_tag, io_t >){
				static_assert(have_input(io_t::value),
					"input doesn't exist");
				return config.inputs_[io_t::value];
			}else if constexpr(hana::is_a< output_name_tag, io_t >){
				static_assert(have_output(io_t::value),
					"output doesn't exist");
				return config.outputs_[io_t::value];
			}else{
				static_assert(have_parameter(io_t::value),
					"parameter doesn't exist");
				return config.parameters_[io_t::value];
			}
		}


		/// \brief hana::true_ if input_name exists
		template < typename String >
		constexpr static auto have_input(String){
			return
				hana::contains(decltype(hana::keys(inputs_))(), String());
		}

		/// \brief hana::true_ if output_name exists
		template < typename String >
		constexpr static auto have_output(String){
			return
				hana::contains(decltype(hana::keys(outputs_))(), String());
		}

		/// \brief hana::true_ if parameter_name exists
		template < typename String >
		constexpr static auto have_parameter(String){
			return
				hana::contains(decltype(hana::keys(parameters_))(), String());
		}


		/// \brief hana::map from input_name's to input's
		Inputs inputs_;

		/// \brief hana::map from output_name's to output's
		Outputs outputs_;

		/// \brief hana::map from parameter_name's to parameter's
		Parameters parameters_;


		/// \brief std::vector with references to all input's (input_base)
		friend auto generate_input_list(module_config& config){
			return hana::unpack(config.inputs_, [](auto& ... input){
				return std::vector< std::reference_wrapper< input_base > >{
					std::ref(static_cast< input_base& >(hana::second(input)))
					... };
			});
		}

		/// \brief std::vector with references to all output's (output_base)
		friend auto generate_output_list(module_config& config){
			return hana::unpack(config.outputs_, [](auto& ... output){
				return std::vector< std::reference_wrapper< output_base > >{
					std::ref(static_cast< output_base& >(hana::second(output)))
					... };
			});
		}
	};


	/// \brief Accessory of a module during enable/disable calls
	template <
		typename Inputs,
		typename Outputs,
		typename Parameters >
	class module_accessory
		: public module_config< Inputs, Outputs, Parameters >
		, public add_log< module_accessory< Inputs, Outputs, Parameters > >
	{
	public:
		/// \brief Constructor
		module_accessory(
			module_base& module,
			Inputs&& inputs,
			Outputs&& outputs,
			Parameters&& parameters
		)
			: module_config< Inputs, Outputs, Parameters >(
					std::move(inputs),
					std::move(outputs),
					std::move(parameters)
				)
			, module_(module) {}


		/// \brief Implementation of the log prefix
		void log_prefix(log_key&&, logsys::stdlogb& os)const{
			os << "chain(" << module_.chain << ") module(" << module_.number
				<< ":" << module_.type_name << "): ";
		}


	protected:
		module_base& module_;
	};


	/// \brief Accessory of a module during exec calls
	template <
		typename Inputs,
		typename Outputs,
		typename Parameters >
	class module_exec_accessory
		: public module_accessory< Inputs, Outputs, Parameters >
		, public add_log< module_exec_accessory< Inputs, Outputs, Parameters > >
	{
	private:
		using log_class =
			add_log< module_exec_accessory< Inputs, Outputs, Parameters > >;

	public:
		using module_accessory< Inputs, Outputs, Parameters >::module_accessory;

		using log_class::log;
		using log_class::exception_catching_log;

		/// \brief Implementation of the log prefix
		void log_prefix(log_key&&, logsys::stdlogb& os)const{
			os << "id(" << this->module_.id << ") chain("
				<< this->module_.chain << ") module(" << this->module_.number
				<< ":" << this->module_.type_name << ") exec: ";
		}
	};


	/// \brief Transfrom a \ref module_accessory to a \ref module_exec_accessory
	template < typename Accessory >
	struct to_exec_accessory;

	/// \brief Transfrom a \ref module_accessory to a \ref module_exec_accessory
	template < typename Inputs, typename Outputs, typename Parameters >
	struct to_exec_accessory< module_accessory< Inputs, Outputs, Parameters > >{
		using type = module_exec_accessory< Inputs, Outputs, Parameters >;
	};

	/// \brief Transfrom a \ref module_accessory to a \ref module_exec_accessory
	template < typename Accessory >
	using to_exec_accessory_t = typename to_exec_accessory< Accessory >::type;


	/// \brief Wrapper for the module enable function
	template < typename ModuleEnableFn >
	class module_enable{
	public:
		module_enable(ModuleEnableFn&& module_fn)
			: module_fn_(static_cast< ModuleEnableFn&& >(module_fn)) {}

		template < typename Accessory >
		auto operator()(Accessory const& accessory)const{
// TODO: remove result_of-version as soon as libc++ supports invoke_result_t
#if __clang__
			if constexpr(std::is_callable_v<
				ModuleEnableFn const(Accessory const&) >
			){
				return module_fn_(accessory);
			}else if constexpr(std::is_callable_v< ModuleEnableFn const() >){
				return module_fn_();
			}else{
				static_assert(false_c< Accessory >,
					"module_enable function must be invokable with module& or "
					"without an argument");
			}
	#else
			if constexpr(
				std::is_invocable_v< ModuleEnableFn const, Accessory const& >
			){
				return module_fn_(accessory);
			}else if constexpr(std::is_invocable_v< ModuleEnableFn const >){
				(void)accessory; // silance GCC
				return module_fn_();
			}else{
				static_assert(false_c< Accessory >,
					"module_enable function must be invokable with module& or "
					"without an argument");
			}
#endif
		}

	private:
		ModuleEnableFn const module_fn_;
	};

	/// \brief Wrapper for the module enable function
	template < typename ModuleExecFn >
	class module_exec{
	public:
		module_exec(ModuleExecFn&& module_fn)
			: module_fn_(static_cast< ModuleExecFn&& >(module_fn)) {}

		template < typename Accessory >
		auto operator()(Accessory& accessory, std::size_t id){
// TODO: remove result_of-version as soon as libc++ supports invoke_result_t
#if __clang__
			if constexpr(
				std::is_callable_v< ModuleExecFn(Accessory&, std::size_t) >
			){
				return module_fn_(accessory, id);
			}else if constexpr(
				std::is_callable_v< ModuleExecFn(Accessory&) >
			){
				return module_fn_(accessory);
			}else if constexpr(
				std::is_callable_v< ModuleExecFn(std::size_t) >
			){
				return module_fn_(id);
			}else if constexpr(std::is_callable_v< ModuleExecFn() >){
				return module_fn_();
			}else{
				static_assert(false_c< Accessory >,
					"exec_fn is not invokable with module& and/or id or "
					"without arguments");
			}
#else
			(void)id; // silance GCC
			(void)accessory; // silance GCC
			if constexpr(
				std::is_invocable_v< ModuleExecFn, Accessory&, std::size_t >
			){
				return module_fn_(accessory, id);
			}else if constexpr(
				std::is_invocable_v< ModuleExecFn, Accessory& >
			){
				return module_fn_(accessory);
			}else if constexpr(
				std::is_invocable_v< ModuleExecFn, std::size_t >
			){
				return module_fn_(id);
			}else if constexpr(std::is_invocable_v< ModuleExecFn >){
				return module_fn_();
			}else{
				static_assert(false_c< Accessory >,
					"exec_fn is not invokable with module& and/or id or "
					"without arguments");
			}
#endif
		}

	private:
		ModuleExecFn module_fn_;
	};


	/// \brief The actual module type
	template <
		typename Inputs,
		typename Outputs,
		typename Parameters,
		typename EnableFn >
	class module: public module_base{
	public:
		/// \brief Type for enable_fn
		using accessory_type =
			module_accessory< Inputs, Outputs, Parameters >;

		/// \brief Type for exec_fn
		using exec_accessory_type =
			module_exec_accessory< Inputs, Outputs, Parameters >;


// TODO: remove result_of-version as soon as libc++ supports invoke_result_t
#if __clang__
		static_assert(std::is_callable_v<
			module_enable< EnableFn >(accessory_type const&) >,
			"enable_fn is not invokable with const module&");

		using exec_fn_t = module_exec< std::result_of_t<
			module_enable< EnableFn >(accessory_type const&) > >;
#else
		static_assert(std::is_invocable_v<
			module_enable< EnableFn >, accessory_type const& >,
			"enable_fn is not invokable with const module&");

		using exec_fn_t = module_exec< std::invoke_result_t<
			module_enable< EnableFn >, accessory_type const& > >;
#endif


		/// \brief Constructor
		module(
			std::string const& module_type,
			std::string const& chain,
			std::size_t number,
			Inputs&& inputs,
			Outputs&& outputs,
			Parameters&& parameters,
			module_enable< EnableFn > const& enable_fn
		)
			: module_base(
					module_type, chain, number,
					// these two functions can be called prior initialzation
					generate_input_list(accessory_),
					generate_output_list(accessory_)
				)
			, accessory_(*this,
					std::move(inputs),
					std::move(outputs),
					std::move(parameters)
				)
			, enable_fn_(enable_fn) {}


	private:
		/// \brief Enables the module for exec calls
		virtual void enable()override{
			exec_fn_.emplace(enable_fn_(
				static_cast< accessory_type const& >(accessory_)));
		}

		/// \brief Disables the module for exec calls
		virtual void disable()noexcept override{
			exec_fn_.reset();
		}


		/// \brief The actual worker function called one times per trigger
		virtual void exec()override{
			if(exec_fn_){
				(*exec_fn_)(accessory_, id);
			}else{
				throw std::logic_error("module is not enabled");
			}
		}


		/// \brief Module access object for exec_fn_
		exec_accessory_type accessory_;

		/// \brief The function object that is called in enable()
		module_enable< EnableFn > enable_fn_;

		/// \brief The function object that is called in exec()
		std::optional< exec_fn_t > exec_fn_;
	};


	/// \brief Maker function for \ref module in a std::unique_ptr
	template <
		typename Inputs,
		typename Outputs,
		typename Parameters,
		typename EnableFn >
	auto make_module_ptr(
		std::string const& type_name,
		std::string const& chain,
		std::size_t number,
		Inputs&& inputs,
		Outputs&& outputs,
		Parameters&& parameters,
		module_enable< EnableFn > const& enable_fn
	){
		return std::make_unique< module<
				std::remove_const_t< std::remove_reference_t< Inputs > >,
				std::remove_const_t< std::remove_reference_t< Outputs > >,
				std::remove_const_t< std::remove_reference_t< Parameters > >,
				EnableFn
			> >(
				type_name, chain, number,
				static_cast< Inputs&& >(inputs),
				static_cast< Outputs&& >(outputs),
				static_cast< Parameters&& >(parameters),
				enable_fn
			);
	}


// 	template < typename IOP_Tuple >
// 	struct module_iop{
// 		template < typename MakeData, std::size_t I >
// 		struct iops_accessory{
// 		public:
// 			constexpr iops_accessory(
// 				IOP_Tuple const& iop_tuple,
// 				MakeData const& make_data,
// 				hana::size_t< I >
// 			)noexcept
// 				: iop_tuple_(hana::slice_c< 0, I >(
// 					hana::transform(iop_tuple, as_reference_list{})) {}
//
// 			/// \brief Get reference to an input-, output- or parameter-object
// 			///        via its corresponding compile time name
// 			template < typename IOP >
// 			decltype(auto) operator()(IOP&& iop)const noexcept{
// 				using iop_t =
// 					std::remove_cv_t< std::remove_reference_t< IOP > >;
// 				static_assert(
// 					hana::is_a< input_name_tag, iop_t > ||
// 					hana::is_a< output_name_tag, iop_t > ||
// 					hana::is_a< parameter_name_tag, iop_t >,
// 					"parameter iop must be an input_name, an output_name or a "
// 					"parameter_name");
//
// 				using iop_tag = typename iop_t::hana_tag;
//
// 				auto iop_ref = hana::find_if(iop_tuple_, [](auto ref){
// 					using tag = typename decltype(ref)::type::hana_tag;
// 					return hana::type_c< iop_tag > == hana::type_c< tag >
// 						&& ref.get().name.value == iop.value;
// 				})
//
// 				auto is_iop_valid = iop_ref != hana::nothing;
// 				static_assert(is_iop_valid,
// 					"parameter iop doesn't exist (yet)");
//
// 				return iop_ref->get();
// 			}
//
// 		private:
// 			IOP_Tuple const& iop_tuple_;
// 		};
//
//
// 		template < typename MakeData, std::size_t I ... >
// 		module_iop(
// 			MakeData const& make_data,
// 			std::index_sequence< I ... >
// 		)
// 			: iop_tuple_(
// 				iops_accessory(iop_tuple_, make_data, hana::size_c< I >) ...) {}
//
//
// 		IOP_Tuple iop_tuple_;
// 	};


	/// \brief Provids types for constructing an module
	template <
		typename IOP_MakerList,
		typename EnableFn >
	struct module_maker{
		/// \brief Tuple of input/output/parameter-maker objects
		IOP_MakerList makers;

		/// \brief The function object that is called in enable()
		module_enable< EnableFn > enable_fn;


		/// \brief Create an module object
		auto operator()(module_make_data const& data)const{
			// Check config file data for undefined inputs, outputs and
			// parameters, warn about parameters, throw for inputs and outputs
			auto const location = data.location();
			{
				auto inputs = invalid_inputs(location, makers, data.inputs);
				auto outputs = invalid_outputs(location, makers, data.outputs);
				check_parameters(location, makers, data.parameters);

				if(!inputs.empty() || !outputs.empty()){
					throw std::logic_error(location + "some inputs or "
						"outputs don't exist, see previos log messages for "
						"more details");
				}
			}

			std::string const basic_location = data.basic_location();

			// create inputs, outputs and parameter in the order of there
			// definition in the module
			auto list = hana::fold_left(makers, hana::make_tuple(),
				[&data, &location, &basic_location](auto&& iop, auto&& maker){
					auto is_input =
						hana::is_a< input_maker_tag >(maker);
					auto is_output =
						hana::is_a< output_maker_tag >(maker);
					auto is_parameter =
						hana::is_a< parameter_maker_tag >(maker);

					static_assert(is_input || is_output || is_parameter,
						"maker is not an iop (this is a bug in disposer!)");

					auto make_accessory = [&basic_location, &maker, &iop]
						(std::string_view type){
							return iop_accessory(iop_log{basic_location,
								type, {to_std_string_view(maker.name)}}, iop);
						};

					using type = typename
						decltype(hana::typeid_(maker))::type::type;

					if constexpr(is_input){
						return hana::append(
							static_cast< decltype(iop)&& >(iop),
							type(input_make_data(
								maker, make_accessory("input"),
								make_output_info(data.inputs,
									to_std_string(maker.name)))));
					}else if constexpr(is_output){
						return hana::append(
							static_cast< decltype(iop)&& >(iop),
							type(output_make_data(
								maker, make_accessory("output"))));
					}else{
						return hana::append(
							static_cast< decltype(iop)&& >(iop),
							type(parameter_make_data(
								maker, make_accessory("parameter"),
								make_parameter_value_map(location, maker,
									data.parameters))));
					}
				}
			);

			// TODO: Don't move or copy iop's after creation
// 			auto list_type = hana::unpack(makers, [](auto const& ... maker){
// 				return hana::type_c< decltype(hana::make_tuple(
// 						std::declval< typename
// 							decltype(hana::typeid_(maker))::type::type >() ...
// 					)) >;
// 			});
// 			static_assert(list_type == hana::typeid_(list));

			// Create the module
			return make_module_ptr(
					data.type_name, data.chain, data.number,
					as_iop_map(hana::filter(
						std::move(list), hana::is_a< input_tag >)),
					as_iop_map(hana::filter(
						std::move(list), hana::is_a< output_tag >)),
					as_iop_map(hana::filter(
						std::move(list), hana::is_a< parameter_tag >)),
					enable_fn
				);
		}
	};


	/// \brief Wraps all given IOP configurations into a hana::tuple
	template < typename ... IOP_Makers >
	constexpr auto module_configure(IOP_Makers&& ... list){
		static_assert(hana::and_(hana::true_c, hana::or_(
			hana::is_a< input_maker_tag, IOP_Makers >(),
			hana::is_a< output_maker_tag, IOP_Makers >(),
			hana::is_a< parameter_maker_tag, IOP_Makers >()) ...),
			"at least one of the module configure arguments is not a disposer "
			"input, output or parameter maker");

		return hana::make_tuple(static_cast< IOP_Makers&& >(list) ...);
	}

	struct unit_test_key;

	/// \brief Hana Tag for \ref module_register_fn
	struct module_register_fn_tag{};

	/// \brief Registers a module configuration in the \ref disposer
	template <
		typename IOP_MakerList,
		typename EnableFn >
	class module_register_fn{
	public:
		/// \brief Hana tag to identify module register functions
		using hana_tag = module_register_fn_tag;

		/// \brief Constructor
		constexpr module_register_fn(
			IOP_MakerList&& list,
			module_enable< EnableFn >&& enable_fn
		)
			: called_flag_(false)
			, maker_{
				static_cast< IOP_MakerList&& >(list),
				std::move(enable_fn)
			}
			{}

		/// \brief Call this function to register the module with the given type
		///        name via the given module_declarant
		void operator()(std::string const& module_type, module_declarant& add){
			if(!called_flag_.exchange(true)){
				add(module_type,
					[maker{std::move(maker_)}](module_make_data const& data){
						return maker(data);
					});
			}else{
				throw std::runtime_error("called module register function '"
					+ module_type + "' more than once");
			}
		}


	private:
		/// \brief Operator must only called once!
		std::atomic< bool > called_flag_;

		/// \brief The module_maker object
		module_maker< IOP_MakerList, EnableFn > maker_;

		friend struct unit_test_key;
	};


}


#endif
