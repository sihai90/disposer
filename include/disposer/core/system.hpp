//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__system__hpp_INCLUDED_
#define _disposer__core__system__hpp_INCLUDED_

#include "directory.hpp"
#include "chain.hpp"

#include "../config/embedded_config.hpp"

#include <unordered_set>


namespace disposer{


	/// \brief Main class of the disposer software
	class system{
	public:
		/// \brief Constructor
		system() = default;

		/// \brief Destructor
		~system();


		/// \brief Not copyable
		system(system const&) = delete;

		/// \brief Not movable
		system(system&&) = delete;


		/// \brief Not copyable
		system& operator=(system const&) = delete;

		/// \brief Not movable
		system& operator=(system&&) = delete;


		/// \brief Load and parse the config file
		void load_config_file(std::string const& filename);


		/// \brief Create components and chains as in config_ defined
		void update_config();


		/// \brief The directory object
		disposer::directory& directory(){
			return directory_;
		}

		/// \brief The directory object
		disposer::directory const& directory()const{
			return directory_;
		}


		/// \brief List of all chaines
		std::unordered_set< std::string > chains()const;


		/// \brief Get a reference to the chain, throw if it does not exist
		chain& get_chain(std::string const& chain);


	private:
		/// \brief Configuration data to create components and chains
		types::embedded_config::config config_;


		/// \brief Component and module generators
		disposer::directory directory_;


		/// \brief List of all components (map from name to object)
		std::unordered_map< std::string, component_ptr > components_;

		/// \brief List of all inactive chains
		std::unordered_set< std::string > inactive_chains_;

		/// \brief List of all active chains (map from name to object)
		std::unordered_map< std::string, chain > chains_;

		/// \brief List of id_generators (map from name to object)
		std::unordered_map< std::string, id_generator > id_generators_;
	};


}


#endif
