//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__module_base__hpp_INCLUDED_
#define _disposer__module_base__hpp_INCLUDED_

#include "disposer.hpp"
#include "output_base.hpp"
#include "input_base.hpp"

#include <functional>


namespace disposer{


	struct chain_key;


	/// \brief Base class for all disposer modules
	class module_base{
	public:
		/// \brief List of inputs
		using input_list =
			std::vector< std::reference_wrapper< input_base > >;

		/// \brief List of outputs
		using output_list =
			std::vector< std::reference_wrapper< output_base > >;


		/// \brief Constructor
		module_base(
			std::string const& type_name,
			std::string const& chain,
			std::size_t number,
			input_list&& inputs,
			output_list&& outputs
		);

		/// \brief Modules are not copyable
		module_base(module_base const&) = delete;

		/// \brief Modules are not movable
		module_base(module_base&&) = delete;


		/// \brief Modules are not copyable
		module_base& operator=(module_base const&) = delete;

		/// \brief Modules are not movable
		module_base& operator=(module_base&&) = delete;


		/// \brief Standard virtual destructor
		virtual ~module_base() = default;


		/// \brief Set for next exec ID
		void set_id(chain_key&&, std::size_t id);


		/// \brief Call the actual worker function exec()
		void exec(chain_key&&){ exec(); }


		/// \brief Call the actual enable() function
		void enable(chain_key&&){ enable(); }

		/// \brief Call the actual disable() function
		void disable(chain_key&&)noexcept{ disable(); }


		/// \brief Called for a modules wich failed by exception and all
		///        following modules in the chain instead of exec()
		///
		/// Removes all input data whichs ID is less or equal to the actual ID.
		void cleanup(chain_key&&, std::size_t id)noexcept;


		/// \brief Map from output names to addresses
		std::map< std::string, output_base* > get_outputs(creator_key&&)const;


		/// \brief Name of the module type given via class module_declarant
		std::string const type_name;

		/// \brief Name of the process chain in config file section 'chain'
		std::string const chain;

		/// \brief Position of the module in the process chain
		///
		/// The first module has number 1.
		std::size_t const number;


		/// Read only reference to the ID while exec() does run
		std::size_t const& id;


	protected:
		/// \brief Enables the module for exec calls
		virtual void enable() = 0;

		/// \brief Disables the module for exec calls
		virtual void disable()noexcept = 0;


		/// \brief The actual worker function called one times per trigger
		virtual void exec() = 0;


	private:
		/// Actual ID while exec() does run
		std::size_t id_;


		/// \brief List of inputs
		input_list inputs_;

		/// \brief List of outputs
		output_list outputs_;
	};


}


#endif
