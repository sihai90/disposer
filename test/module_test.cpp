#include <disposer/module.hpp>

#include <iostream>
#include <iomanip>


namespace hana = boost::hana;

using namespace hana::literals;
using namespace disposer::interface::module;


void check_impl(std::string_view name, bool enabled, bool expected){
	if(enabled == expected){
		std::cout << " \033[0;32msuccess:\033[0m ";
	}else{
		std::cout << " \033[0;31mfail:\033[0m ";
	}
	std::cout << name << " = " << std::boolalpha << expected << "\n";
}

template < typename IO >
void check(IO& io, bool expected){
	check_impl(io.name, io.is_enabled(), expected);
}

template < typename IO, typename Type >
void check(IO& io, Type const& type, bool expected){
	check_impl(io.name, io.is_enabled(type), expected);
}



struct exec_fn{
	template < typename T >
	constexpr void operator()(T& /*module*/, std::size_t /*id*/)const{}
};

struct enable_fn{
	template < typename T >
	constexpr auto operator()(T const& /*module*/)const{
		return exec_fn{};
	}
};

int main(){
	using namespace hana::literals;

	using ident =
		decltype(hana::typeid_(hana::template_< disposer::self_t >))::type;



	std::size_t error_count = 0;

	try{
		{
			auto const register_fn = disposer::make_register_fn(
				disposer::configure(),
				enable_fn()
			);

			static_assert(std::is_same_v< decltype(register_fn.maker),
				disposer::module_maker< hana::tuple<
				>, enable_fn > >);

			auto object = register_fn.maker(disposer::make_data{});

			static_assert(std::is_same_v< decltype(object),
				std::unique_ptr< disposer::module<
					decltype(hana::make_map()),
					decltype(hana::make_map()),
					decltype(hana::make_map()),
					enable_fn
				> > >);
		}

		{
			auto const register_fn = disposer::make_register_fn(
				disposer::configure(
					"v"_in(hana::type_c< int >)
				),
				enable_fn()
			);

			static_assert(std::is_same_v< decltype(register_fn.maker),
				disposer::module_maker< hana::tuple<
					disposer::input_maker<
						decltype("v"_in),
						disposer::input< decltype("v"_in), ident, int >,
						disposer::verify_connection_always,
						disposer::verify_type_always
					>
				>, enable_fn > >);

			auto object = register_fn.maker(disposer::make_data{});

			static_assert(std::is_same_v< decltype(object),
				std::unique_ptr< disposer::module<
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval< disposer::input<
							decltype("v"_in), ident, int > >())
					)),
					decltype(hana::make_map()),
					decltype(hana::make_map()),
					enable_fn
				> > >);
		}


		{
			auto const register_fn = disposer::make_register_fn(
				disposer::configure(
					"v"_out(hana::type_c< int >)
				),
				enable_fn()
			);

			static_assert(std::is_same_v< decltype(register_fn.maker),
				disposer::module_maker< hana::tuple<
					disposer::output_maker<
						decltype("v"_out),
						disposer::output< decltype("v"_out), ident, int >,
						disposer::enable_always
					>
				>, enable_fn > >);

			auto object = register_fn.maker(disposer::make_data{});

			static_assert(std::is_same_v< decltype(object),
				std::unique_ptr< disposer::module<
					decltype(hana::make_map()),
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval< disposer::output<
							decltype("v"_out), ident, int > >())
					)),
					decltype(hana::make_map()),
					enable_fn
				> > >);
		}


		{
			auto const register_fn = disposer::make_register_fn(
				disposer::configure(
					"v"_param(hana::type_c< int >)
				),
				enable_fn()
			);

			static_assert(std::is_same_v< decltype(register_fn.maker),
				disposer::module_maker< hana::tuple<
					disposer::parameter_maker<
						decltype("v"_param),
						disposer::parameter< decltype("v"_param), int >,
						disposer::verify_value_always,
						disposer::enable_always,
						disposer::stream_parser,
						decltype(hana::make_map(
							hana::make_pair(hana::type_c< int >, "sint32"_s)
						))
					>
				>, enable_fn > >);

			auto object = register_fn.maker(disposer::make_data{});

			static_assert(std::is_same_v< decltype(object),
				std::unique_ptr< disposer::module<
					decltype(hana::make_map()),
					decltype(hana::make_map()),
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval<
							disposer::parameter< decltype("v"_param), int > >())
					)),
					enable_fn
				> > >);
		}

		{
			auto const register_fn = disposer::make_register_fn(
				disposer::configure(
					"v"_in(hana::type_c< int >),
					"v"_out(hana::type_c< int >),
					"v"_param(hana::type_c< int >)
				),
				enable_fn()
			);

			static_assert(std::is_same_v< decltype(register_fn.maker),
				disposer::module_maker< hana::tuple<
					disposer::input_maker<
						decltype("v"_in),
						disposer::input< decltype("v"_in), ident, int >,
						disposer::verify_connection_always,
						disposer::verify_type_always
					>,
					disposer::output_maker<
						decltype("v"_out),
						disposer::output< decltype("v"_out), ident, int >,
						disposer::enable_always
					>,
					disposer::parameter_maker<
						decltype("v"_param),
						disposer::parameter< decltype("v"_param), int >,
						disposer::verify_value_always,
						disposer::enable_always,
						disposer::stream_parser,
						decltype(hana::make_map(
							hana::make_pair(hana::type_c< int >, "sint32"_s)
						))
					>
				>, enable_fn > >);

			auto object = register_fn.maker(disposer::make_data{});

			static_assert(std::is_same_v< decltype(object),
				std::unique_ptr< disposer::module<
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval< disposer::input<
							decltype("v"_in), ident, int > >())
					)),
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval< disposer::output<
							decltype("v"_out), ident, int > >())
					)),
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval<
							disposer::parameter< decltype("v"_param), int > >())
					)),
					enable_fn
				> > >);
		}

		{
			constexpr auto enable_out = [](auto const& get, auto type){
				bool active = get("v"_in).is_enabled(type);
				assert(!active);
				return active;
			};

			constexpr auto enable_in_c = [](auto const&, bool connected){
				assert(!connected);
			};

			constexpr auto enable_param = [](auto const& get, auto type){
				bool active1 = get("v"_in).is_enabled(type);
				bool active2 = get("v"_out).is_enabled(type);
				assert(!active1 && !active2);
				return false;
			};

			constexpr auto parser = [](std::string_view /*value*/, auto type){
				static_assert(type == hana::type_c< int >);
				return 5;
			};

			constexpr auto enable_in_t =
				[](auto const& get, auto type, disposer::output_info const&){
					bool active1 = get("v"_in).is_enabled(type);
					bool active2 = get("v"_out).is_enabled(type);
					bool active3 = get("v"_param).is_enabled(type);
					assert(!active1 && !active2 && !active3);
				};

			auto const register_fn = disposer::make_register_fn(
				disposer::configure(
					"v"_in(hana::type_c< int >),
					"v"_out(hana::type_c< int >, ident{}, enable_out),
					"v"_param(hana::type_c< int >, disposer::verify_value_always(),
						enable_param, parser, hana::make_tuple(7)),
					"w"_in(hana::type_c< int >, ident{}, enable_in_c, enable_in_t)
				),
				enable_fn()
			);

			static_assert(std::is_same_v< decltype(register_fn.maker),
				disposer::module_maker< hana::tuple<
					disposer::input_maker<
						decltype("v"_in),
						disposer::input< decltype("v"_in), ident, int >,
						disposer::verify_connection_always,
						disposer::verify_type_always
					>,
					disposer::output_maker<
						decltype("v"_out),
						disposer::output< decltype("v"_out), ident, int >,
						decltype(enable_out)
					>,
					disposer::parameter_maker<
						decltype("v"_param),
						disposer::parameter< decltype("v"_param), int >,
						disposer::verify_value_always,
						decltype(enable_param),
						decltype(parser),
						decltype(hana::make_map(
							hana::make_pair(hana::type_c< int >, "sint32"_s)
						))
					>,
					disposer::input_maker<
						decltype("w"_in),
						disposer::input< decltype("w"_in), ident, int >,
						decltype(enable_in_c),
						decltype(enable_in_t)
					>
				>, enable_fn > >);

			auto object = register_fn.maker(disposer::make_data{});

			static_assert(std::is_same_v< decltype(object),
				std::unique_ptr< disposer::module<
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval< disposer::input<
							decltype("v"_in), ident, int > >()),
						hana::make_pair("w"_s, std::declval< disposer::input<
							decltype("w"_in), ident, int > >())
					)),
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval< disposer::output<
							decltype("v"_out), ident, int > >())
					)),
					decltype(hana::make_map(
						hana::make_pair("v"_s, std::declval<
							disposer::parameter< decltype("v"_param), int > >())
					)),
					enable_fn
				> > >);
		}

		if(error_count == 0){
			std::cout << "\033[0;32mSUCCESS\033[0m\n";
		}else{
			std::cout << "\033[0;31mFAILS:\033[0m " << error_count << '\n';
		}
	}catch(std::exception const& e){
		std::cerr << "Unexpected exception: " << e.what() << '\n';
	}catch(...){
		std::cerr << "Unexpected unknown exception\n";
	}

	return error_count;
}
