#pragma once

#include <array>
#include <string>
#include "container_types.hpp"
#include "commands.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include "system_state.hpp"
#include "network.hpp"
#include "parsers.hpp"
#include "simple_fs.hpp"
#include "network.hpp"
#include "demographics.hpp"

#include <text.hpp>
#include <json.hpp>

#define CPPHTTPLIB_NO_EXCEPTIONS
#include <httplib.h>

using json = nlohmann::json;

namespace webui {

// HTTP
httplib::Server svr;

inline void init(sys::state& state) noexcept {

	if(state.defines.alice_expose_webui != 1 || state.network_mode == sys::network_mode_type::client) {
		return;
	}

	svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
		res.set_content("Homepage", "text/plain");
	});

	svr.Get("/date", [&](const httplib::Request& req, httplib::Response& res) {
		auto dt = state.current_date.to_ymd(state.start_date);
		json j = json::object();
		j["year"] = dt.year;
		j["month"] = dt.month;
		j["day"] = dt.day;
		j["date"] = std::to_string(dt.day) + "." + std::to_string(dt.month) + "." + std::to_string(dt.year);

		res.set_content(j.dump(), "text/plain");
	});


	svr.Get("/nations", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto nation : state.world.in_nation) {
				auto nation_name = text::produce_simple_string(state, text::get_name(state, nation.id));

				auto nation_ppp_gdp_text = text::format_float(economy::gdp_adjusted(state, nation.id));
				float population = state.world.nation_get_demographics(nation.id, demographics::total);
				auto nation_ppp_gdp_per_capita_text = text::format_float(economy::gdp_adjusted(state, nation.id) / population * 1000000.f);
				auto nation_sol_text = text::format_float(demographics::calculate_nation_sol(state, nation.id));

				json j = json::object();

				auto reserve_rate = economy::national_bank_reserve_rate(state, nation);
				auto total_assets = economy::national_bank_total_capital(state, nation);
				auto national_bank = state.world.nation_get_national_bank(nation);
				auto state_debt = nations::get_debt(state, nation);

				j["id"] = nation.id.index();
				j["name"] = nation_name;
				j["population"] = population;
				j["nation_ppp_gdp"] = nation_ppp_gdp_text;
				j["nation_ppp_gdp_per_capita"] = nation_ppp_gdp_per_capita_text;
				j["nation_sol"] = nation_sol_text;

				j["national_bank_total_assets"] = total_assets;
				j["state_debt"] = state_debt;

				j["bank_max_debt_financing_share"] = text::format_percentage(economy::national_bank_max_debt_financing_share(state, nation));
				j["bank_private_borrowing"] = text::format_money(economy::national_bank_private_borrowing(state, nation));
				j["bank_reserves"] = text::format_money(reserve_rate * total_assets);
				j["bank_free_capital"] = economy::national_bank_free_capital(state, nation);
				j["bank_assets_total"] = text::format_money(total_assets);
				j["bank_interest_rate_total"] = text::format_percentage(economy::interest_rate(state, nation) * 30.f);
				j["bank_instability_interest_premium"] = text::format_percentage(state.world.nation_get_instability_interest_premium(nation) / 100.f);
				j["bank_added_minimal_interest"] = text::format_percentage(state.world.nation_get_added_minimal_interest(nation) / 100.f);

				j["bank_state_deposit"] = text::format_money(state.world.nation_get_local_deposit(nation));
				j["bank_pops_deposit"] = text::format_money(state.world.nation_get_national_bank(nation));

				jlist.push_back(j);
			}

		res.set_content(jlist.dump(), "text/plain");

	});

	svr.Get("/commodities", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto commodity : state.world.in_commodity) {
				auto id = commodity.id.index();

				auto commodity_name = text::produce_simple_string(state, state.world.commodity_get_name(commodity));
				
				json j = json::object();

				j["id"] = id;
				j["name"] = commodity_name;
				
				json jp = json::array();

				struct tagged_value {
					float v = 0.0f;
					dcon::nation_id n;
				};
				static std::vector<tagged_value> producers;

				producers.clear();

				j["producers"] = jp;

				jlist.push_back(j);
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.Get("/routes", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto cid : state.world.in_commodity) {
			state.world.for_each_trade_route([&](dcon::trade_route_id trade_route) {
				auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
				auto origin =
					current_volume > 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);
				auto target =
					current_volume <= 0.f
					? state.world.trade_route_get_connected_markets(trade_route, 0)
					: state.world.trade_route_get_connected_markets(trade_route, 1);

				auto s_origin = state.world.market_get_zone_from_local_market(origin);
				auto s_target = state.world.market_get_zone_from_local_market(target);

				auto p_origin = state.world.state_instance_get_capital(s_origin);
				auto p_target = state.world.state_instance_get_capital(s_target);

				auto sat = state.world.market_get_direct_demand_satisfaction(origin, cid);

				auto absolute_volume = std::abs(current_volume);
				auto factual_volume = sat * absolute_volume;

				if(absolute_volume <= 0) {
					return;
				}

				bool is_sea = state.world.trade_route_get_distance(trade_route) == state.world.trade_route_get_sea_distance(trade_route);

				auto commodity_name = text::produce_simple_string(state, state.world.commodity_get_name(cid));

				json j = json::object();

				j["commodity_id"] = cid.id.value;
				j["commodity"] = commodity_name;

				j["origin_market_id"] = origin.value;
				j["target_market_id"] = target.value;

				j["origin_state_id"] = s_origin.value;
				j["target_state_id"] = s_target.value;

				j["origin_province_id"] = p_origin.id.value;
				j["target_province_id"] = p_target.id.value;

				j["origin_province_name"] = text::produce_simple_string(state, state.world.province_get_name(p_origin));
				j["target_province_name"] = text::produce_simple_string(state, state.world.province_get_name(p_target));

				auto origin_country = state.world.province_get_nation_from_province_ownership(p_origin);
				auto target_country = state.world.province_get_nation_from_province_ownership(p_target);

				j["origin_country_id"] = origin_country.value;
				j["target_country_id"] = target_country.value;

				j["origin_country_name"] = text::produce_simple_string(state, text::get_name(state, origin_country));
				j["target_country_name"] = text::produce_simple_string(state, text::get_name(state, target_country));

				j["volume"] = text::format_float(factual_volume);
				j["desired_volume"] = text::format_float(absolute_volume);

				j["is_sea"] = is_sea;
				jlist.push_back(j);
			});
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.Get("/provinces", [&](const httplib::Request& req, httplib::Response& res) {
		json jlist = json::array();

		for(auto prov : state.world.in_province) {
			auto id = prov.id.index();

			auto province_name = text::produce_simple_string(state, state.world.province_get_name(prov));

			auto owner = state.world.province_get_nation_from_province_ownership(prov.id);
			auto prov_population = state.world.province_get_demographics(prov.id, demographics::total);

			float num_capitalist = state.world.province_get_demographics(
					prov,
					demographics::to_key(state, state.culture_definitions.capitalists)
			);

			float num_aristocrat = state.world.province_get_demographics(
					prov,
					demographics::to_key(state, state.culture_definitions.aristocrat)
			);

			auto rgo = state.world.province_get_rgo(prov);

			json j = json::object();

			j["id"] = id;
			j["name"] = province_name;
			j["owner"] = text::produce_simple_string(state, text::get_name(state, owner));
			j["population"]["total"] = prov_population;
			j["population"]["capitalist"] = num_capitalist;
			j["population"]["aristocrat"] = num_aristocrat;

			j["rgo"] = text::produce_simple_string(state, state.world.commodity_get_name(rgo));

			json jp = json::array();

			jlist.push_back(j);
		}

		res.set_content(jlist.dump(), "text/plain");
	});

	svr.listen("0.0.0.0", 1234);
}

}

