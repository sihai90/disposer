//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <disposer/module_base.hpp>


namespace disposer{


	module_base::module_base(make_data const& data, std::vector< std::reference_wrapper< input_base > >&& inputs, std::vector< std::reference_wrapper< output_base > >&& outputs):
		type_name(data.type_name),
		chain(data.chain),
		name(data.name),
		number(data.number),
		id_increase(1),
		id(id_),
		id_(0),
		inputs_(std::move(inputs)),
		outputs_(std::move(outputs))
		{}

	module_base::module_base(make_data const& data, std::vector< std::reference_wrapper< output_base > >&& outputs, std::vector< std::reference_wrapper< input_base > >&& inputs):
		module_base(data, std::move(inputs), std::move(outputs)){}


	void module_base::cleanup(std::size_t id)noexcept{
		for(auto& input: inputs_){
			input.get().cleanup(id);
		}
	}

	void module_base::set_id(std::size_t id){
		id_ = id;
		for(auto& input: inputs_){
			input.get().id_ = id;
		}
		for(auto& output: outputs_){
			output.get().id_ = id;
		}
	}


}
