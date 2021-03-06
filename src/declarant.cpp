//-----------------------------------------------------------------------------
// Copyright (c) 2017-2018 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <disposer/core/declarant.hpp>
#include <disposer/core/directory.hpp>

#include <logsys/stdlogb.hpp>
#include <logsys/log.hpp>


namespace disposer{


	void declarant::operator()(
		std::string const& type_name,
		component_maker_fn&& fn,
		std::string&& help_text
	){
		logsys::log([&type_name](logsys::stdlogb& os){
			os << "add component type name '" << type_name << "'";
		}, [this, &type_name, &fn]{
			[[maybe_unused]] auto [i, success] = directory_
				.component_maker_list_.emplace(type_name, std::move(fn));

			if(!success){
				throw std::logic_error(
					"component type name '" + type_name
					+ "' has been added more than one time!");
			}
		});

		logsys::log([&type_name](logsys::stdlogb& os){
			os << "add help for component type name '" << type_name << "'";
		}, [this, &type_name, &help_text]{
			[[maybe_unused]] auto [i, success] = directory_
				.component_help_list_.emplace(type_name, std::move(help_text));

			assert(success);
		});
	}

	void declarant::operator()(
		std::string const& component_name,
		std::string const& module_type_name,
		component_module_maker_fn&& fn
	){
		logsys::log([&component_name, &module_type_name](logsys::stdlogb& os){
			os << "add component module type name '"
				<< component_name << "//" << module_type_name << "'";
		}, [&]{
			auto [module_list, unused] = directory_
				.component_module_maker_list_.try_emplace(component_name);
			(void)unused;

			[[maybe_unused]] auto [i, success] = module_list->second
				.emplace(module_type_name, std::move(fn));

			assert(success);
		});
	}

	void declarant::operator()(
		std::string const& type_name,
		module_maker_fn&& fn,
		std::string&& help_text
	){
		logsys::log([&type_name](logsys::stdlogb& os){
			os << "add module type name '" << type_name << "'";
		}, [this, &type_name, &fn]{
			[[maybe_unused]] auto [i, success] = directory_
				.module_maker_list_.emplace(type_name, std::move(fn));

			if(!success){
				throw std::logic_error(
					"module type name '" + type_name
					+ "' has been added more than one time!");
			}
		});

		logsys::log([&type_name](logsys::stdlogb& os){
			os << "add help for module type name '" << type_name << "'";
		}, [this, &type_name, &help_text]{
			[[maybe_unused]] auto [i, success] = directory_
				.module_help_list_.emplace(type_name, std::move(help_text));

			assert(success);
		});
	}


}
