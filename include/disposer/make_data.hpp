//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__make_data__hpp_INCLUDED_
#define _disposer__make_data__hpp_INCLUDED_

#include "merge.hpp"

#include <set>
#include <map>


namespace disposer{


	class output_base;


	/// \brief Map from input names to output pointers
	using input_list =
		std::map< std::string, std::tuple< output_base*, bool > >;

	/// \brief Output name lists
	using output_list = std::set< std::string >;

	/// \brief List of parameter name value pairs
	using parameter_list = std::map< std::string, parameter_data >;


	/// \brief Dataset for disposer to construct and initialize a module
	struct make_data{
		/// \brief Name of the module type given via class module_declarant
		std::string const type_name;

		/// \brief Name of the process chain in config file section 'chain'
		std::string const chain;

		/// \brief Position of the module in the process chain
		///
		/// The first module has number 1.
		std::size_t const number;

		/// \brief Map of the input names and corresponding output pointers
		input_list const inputs;

		/// \brief List of the output names
		output_list const outputs;

		/// \brief Parameters from the config file
		parameter_list const parameters;

		/// \brief Header for log messages
		std::string location()const{
			return "chain '" + chain + "' module number "
				+ std::to_string(number) + " (type '" + type_name + "'): ";
		}
	};


}


#endif
