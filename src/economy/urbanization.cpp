#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "demographics.hpp"
#include <economy_stats.hpp>

namespace economy {

// Market urbanization is a number on 0-100% scale representing the share of workers with urban jobs and living in cities
float get_market_urbanization(sys::state& state, dcon::market_id m) {
	auto total = 0.f;
	auto urban = 0.f;

	auto sid = state.world.market_get_zone_from_local_market(m);
	auto n = state.world.state_instance_get_nation_from_state_ownership(sid);

	for(auto po : state.world.nation_get_province_ownership(n)) {
		auto p = po.get_province();

		if(p.get_state_membership() != sid) {
			continue;
		}

		for(auto pop : state.world.province_get_pop_location(p)) {
			auto size = pop.get_pop().get_size();
			total += size;

			// Capitalists are always urban
			auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
			auto capitalists = state.world.province_get_demographics(p, capitalists_key);
			urban += capitalists;
			// Aristocrats live in countryside estates
		}

		// Factory workers are urban workers
		for(auto f : state.world.province_get_factory_location(p)) {
			urban += f.get_factory().get_unqualified_employment()
				* state.world.province_get_labor_demand_satisfaction(p, economy::labor::no_education)
			+ f.get_factory().get_primary_employment()
				* state.world.province_get_labor_demand_satisfaction(p, economy::labor::basic_education)
				+ f.get_factory().get_secondary_employment()
				* state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);
		}

		// Administration workers are urban workers
		urban += state.world.province_get_administration_employment_target(p) * state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);

		for(auto admin : state.world.nation_get_nation_administration(n)) {
			if(admin.get_administration().get_capital() == p) {
				urban += admin.get_administration().get_desired_size() * state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);
			}
		}
	}

	return urban / total;
}

void update_market_urbanization(sys::state& state) {
	for(auto m : state.world.in_market) {
		m.set_urbanization(get_market_urbanization(state, m));
	}
}


// Province urbanization is a number on 0-100% scale representing the share of workers with urban jobs and living in cities
float get_province_urbanization(sys::state& state, dcon::province_id p) {
	auto total = state.world.province_get_demographics(p, demographics::total);
	auto urban = 0.f;
	for(auto pop : state.world.province_get_pop_location(p)) {
		auto size = pop.get_pop().get_size();

		// Capitalists are always urban
		auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
		auto capitalists = state.world.province_get_demographics(p, capitalists_key);
		urban += capitalists;
		// Aristocrats live in countryside estates
	}

	// Factory workers are urban workers
	for(auto f : state.world.province_get_factory_location(p)) {
		urban += f.get_factory().get_unqualified_employment()
			* state.world.province_get_labor_demand_satisfaction(p, economy::labor::no_education)
		+ f.get_factory().get_primary_employment()
			* state.world.province_get_labor_demand_satisfaction(p, economy::labor::basic_education)
			+ f.get_factory().get_secondary_employment()
			* state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);
	}

	// Administration workers are urban workers
	urban += state.world.province_get_administration_employment_target(p) * state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);

	auto n = state.world.province_get_nation_from_province_ownership(p);
	for(auto admin : state.world.nation_get_nation_administration(n)) {
		if(admin.get_administration().get_capital() == p) {
			urban += admin.get_administration().get_desired_size() * state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);
		}
	}

	return urban / total;
}

float calculate_urban_needs_scaling(sys::state& state, float base, dcon::commodity_id c, dcon::market_id m) {
	auto is_urban = state.world.commodity_get_is_urban(c);

	if(!is_urban) {
		return base;
	}

	auto urbanization = get_market_urbanization(state, m);

	return base * urbanization;
}

float get_effective_pop_life_needs(sys::state& state, dcon::pop_type_id pop_type, dcon::commodity_id c, dcon::market_id m) {
	return calculate_urban_needs_scaling(state, state.world.pop_type_get_life_needs(pop_type, c), c, m);
}
float get_effective_pop_everyday_needs(sys::state& state, dcon::pop_type_id pop_type, dcon::commodity_id c, dcon::market_id m) {
	return calculate_urban_needs_scaling(state, state.world.pop_type_get_everyday_needs(pop_type, c), c, m);
}
float get_effective_pop_luxury_needs(sys::state& state, dcon::pop_type_id pop_type, dcon::commodity_id c, dcon::market_id m) {
	return calculate_urban_needs_scaling(state, state.world.pop_type_get_luxury_needs(pop_type, c), c, m);
}

}
