//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#include <disposer/config/parse_config.hpp>

#include <io_tools/mask_non_print.hpp>

#include <boost/optional/optional_io.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>


namespace disposer{ namespace types{ namespace parse{

	std::ostream& operator<<(std::ostream& os, std::string const& v){
		os << '"';
		std::operator<<(os, v);
		os << '"';
		return os;
	}

	template < typename T >
	std::ostream& operator<<(std::ostream& os, std::optional< T > const& v){
		os << '{';
		if(v) std::operator<<(os, *v);
		os << '}';

		return os;
	}

	template < typename T >
	std::ostream& operator<<(std::ostream& os, std::vector< T > const& v){
		os << '{';
		auto iter = v.cbegin();
		if(iter != v.cend()) os << *iter++;
		while(iter != v.cend()) os << ',' << *iter++;
		os << "}";
		return os;
	}


	std::ostream& operator<<(std::ostream& os, specialized_parameter const& v){
		return os << "{" << v.type << "," << v.value << "}";
	}

	std::ostream& operator<<(std::ostream& os, parameter const& v){
		return os << "{" << v.key << "," << v.generic_value << ","
			<< v.specialized_values << "}";
	}

	std::ostream& operator<<(std::ostream& os, parameter_set const& v){
		return os << "{" << v.name << "," << v.parameters << "}";
	}

	std::ostream& operator<<(std::ostream& os, in const& v){
		os << "{" << v.name << ",";
		switch(v.transfer){
			case disposer::in_transfer::copy: os << '&'; break;
			case disposer::in_transfer::move: os << '<'; break;
		}
		return os << "," << v.variable << "}";
	}

	std::ostream& operator<<(std::ostream& os, out const& v){
		return os << "{" << v.name << "," << v.variable << "}";
	}

	std::ostream& operator<<(std::ostream& os, parameters const& v){
		return os << "{" << v.parameter_sets << "," << v.parameters << "}";
	}

	std::ostream& operator<<(std::ostream& os, module const& v){
		return os << "{" << v.type_name << "," << v.parameters << ","
			<< v.inputs << "," << v.outputs << "}";
	}

	std::ostream& operator<<(std::ostream& os, chain const& v){
		return os << "{" << v.name << ","
			<< *v.id_generator << "," << v.modules << "}";
	}

	std::ostream& operator<<(std::ostream& os, component const& v){
		return os << "{" << v.type_name << "," << v.parameters << "}";
	}

	std::ostream& operator<<(std::ostream& os, config const& v){
		return os << "{" << v.sets << "," << v.components
			<< "," << v.chains << "}";
	}

	bool operator==(
		specialized_parameter const& l,
		specialized_parameter const& r
	){
		return l.type == r.type
			&& l.value == r.value;
	}

	bool operator==(
		parameter const& l,
		parameter const& r
	){
		return l.key == r.key
			&& l.generic_value == r.generic_value
			&& l.specialized_values == r.specialized_values;
	}

	bool operator==(
		parameter_set const& l,
		parameter_set const& r
	){
		return l.name == r.name
			&& l.parameters == r.parameters;
	}

	bool operator==(
		in const& l,
		in const& r
	){
		return l.name == r.name
			&& l.transfer == r.transfer
			&& l.variable == r.variable;
	}

	bool operator==(
		out const& l,
		out const& r
	){
		return l.name == r.name
			&& l.variable == r.variable;
	}

	bool operator==(
		parameters const& l,
		parameters const& r
	){
		return l.parameter_sets == r.parameter_sets
			&& l.parameters == r.parameters;
	}

	bool operator==(
		module const& l,
		module const& r
	){
		return l.type_name == r.type_name
			&& l.parameters == r.parameters
			&& l.inputs == r.inputs
			&& l.outputs == r.outputs;
	}

	bool operator==(
		chain const& l,
		chain const& r
	){
		return l.name == r.name
			&& l.id_generator == r.id_generator
			&& l.modules == r.modules;
	}

	bool operator==(
		component const& l,
		component const& r
	){
		return l.type_name == r.type_name
			&& l.parameters == r.parameters;
	}

	bool operator==(
		config const& l,
		config const& r
	){
		return l.sets == r.sets
			&& l.components == r.components
			&& l.chains == r.chains;
	}

} } }

using disposer::types::parse::config;

int success(std::size_t i){
	std::cout << std::setw(3) << i << " \033[0;32msuccess:\033[0m";;
	return 0;
}

int fail2(std::size_t i, config const& ref, config const& test){
	std::ostringstream os1;
	std::ostringstream os2;
	os1 << ref;
	os2 << test;
	std::cout << std::setw(3) << i << " \033[0;31mfail:\033[0m "
		<< "\n" << io_tools::mask_non_print(os1.str())
		<< "\n" << io_tools::mask_non_print(os2.str()) << "\n";
	return 1;
}

int fail1(std::size_t i, std::string const& msg){
	std::cout << std::setw(3) << i << " \033[0;31mfail:\033[0m "
		<< io_tools::mask_non_print(msg) << "\n";
	return 1;
}


std::vector< std::pair< std::string, config > > tests{
	{
R"file(parameter_set
	ps1
		param1 = v1
			uint8 = 4
		param2
			uint16 = 2
component
	component_name = component_type
		parameter_set = ps1
		param3 = v3
			uint8 = 4
		param4
			uint16 = 2
chain
	chain1
		dmod1
			wait_on = 0:dmod0
			parameter
				parameter_set = ps1
				param3 = v3
					uint8 = 4
				param4
					uint16 = 2
			->
				out = >x1
)file"
	,
		config{
			{
				{
					"ps1",
					{
						{"param1", {"v1"}, {{"uint8", "4"}}},
						{"param2", {}, {{"uint16", "2"}}}
					}
				}
			},
			{
				{
					"component_name",
					"component_type",
					{
						{
							{"ps1"}
						}, {
							{"param3", {"v3"}, {{"uint8", "4"}}},
							{"param4", {}, {{"uint16", "2"}}}
						}
					}
				}
			},
			{
				{
					"chain1",
					{},
					{
						{
							"dmod1",
							{
								{0, "dmod0"}
							},
							{
								{
									{"ps1"}
								}, {
									{"param3", {"v3"}, {{"uint8", "4"}}},
									{"param4", {}, {{"uint16", "2"}}}
								}
							},
							{},
							{
								{"out", "x1"}
							}
						}
					}
				}
			}
		}
	}
};

int parse(std::size_t i, std::string content, config const& conf){
	try{
		std::istringstream file(content);
		auto result = disposer::parse(file);
		if(result == conf){
			return success(i);
		}else{
			return fail2(i, conf, result);
		}
	}catch(std::exception const& e){
		return fail1(i, e.what());
	}catch(...){
		return fail1(i, "Unknown exception");
	}
}

int main(){
	std::cout << std::setfill('0');
	std::size_t i = 0;
	std::size_t r = 0;
	for(auto const& v: tests){
		r += parse(i++, v.first, v.second);
	}

	if(r == 0){
		std::cout << "\033[0;32mSUCCESS\033[0m\n";
	}else{
		std::cout << "\033[0;31mFAILS:\033[0m " << r << '\n';
	}
}
