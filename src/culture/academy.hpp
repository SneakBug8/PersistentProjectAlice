#pragma once
#include <prng.hpp>
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "modifiers.hpp"

namespace academy {
	constexpr inline int32_t positionscount = 8;
	constexpr inline int32_t levelscount = 6;

	const inline std::string PositionNames[] = {"Test1", "Test2", "Test3", "Test4", "Test5", "Test6", "Test7", "Test8"};

	inline int32_t roll_skill(sys::state& state, uint32_t seed) {
		return rng::get_random(state, seed) % 6 - 3;
	}

	inline void apply_modifiers(sys::state& state, dcon::nation_id n) {
		auto academia = state.world.nation_get_academia_from_academia_loyalty(n);

		//state.world.map_of_modifiers.(std::string("easy_ai"), new_modifier);

		//state.world.in_modifier

		for(int32_t i = 0; i < positionscount; i++) {
			
		}
	}
	inline void fill_position(sys::state& state, dcon::nation_id n, int32_t position) {
		auto academia = dcon::fatten(state.world, state.world.nation_get_academia_from_academia_loyalty(n));
		uint32_t seed_base = (uint32_t(n.index()) << 6) ^ uint32_t(academia.id.index());

		auto nationtag = text::produce_simple_string(state, dcon::fatten(state.world, n).get_identity_from_identity_holder().get_name());

		auto skill = roll_skill(state, seed_base + position);
		academia.set_skills(position, skill);

		auto fnames = state.world.culture_get_first_names(state.world.nation_get_primary_culture(n));
		auto lnames = state.world.culture_get_last_names(state.world.nation_get_primary_culture(n));
		auto names_pair = rng::get_random_pair(state, seed_base + position);

		if(fnames.size() > 0 && lnames.size() > 0) {
			auto fname = fnames.at(rng::reduce(uint32_t(names_pair.high), fnames.size()));
			auto lname = lnames.at(rng::reduce(uint32_t(names_pair.high), lnames.size()));

			academia.set_first_names(position, fname );
			academia.set_last_names(position, lname );
		}
		else {
			academia.set_first_names(position, dcon::unit_name_id{});
			academia.set_last_names(position, dcon::unit_name_id{});
		}
	}

	inline void fill_academia(sys::state& state, dcon::nation_id n) {
		auto academia = dcon::fatten(state.world, state.world.create_academia());
		auto academia_loyalty = dcon::fatten(state.world, state.world.try_create_academia_loyalty(n, academia.id));

		academia.resize_first_names(positionscount);
		academia.resize_last_names(positionscount);
		academia.resize_skills(positionscount);
		academia.resize_modifiers(positionscount);

		uint32_t seed_base = (uint32_t(n.index()) << 6) ^ uint32_t(academia.id.index());
		auto names = state.world.culture_get_last_names(state.world.nation_get_primary_culture(n));

		for (int32_t i = 0; i < positionscount; i++) {
			fill_position(state, n, i);
		}

		apply_modifiers(state, n);
	}

	inline void fill_academia(sys::state& state) {
		for(const auto n : state.world.in_nation) {
			fill_academia(state, n);
		}
	}

	inline void update_academia(sys::state& state, dcon::nation_id n) {
		auto academia = state.world.nation_get_academia_from_academia_loyalty(n);

		uint32_t seed_base = (uint32_t(n.index()) << 6) ^ uint32_t(state.current_date.to_raw_value());

		// Chance for replacing academic: 8 rolls a month with 1/100 chance giving average worklife of 8 years
		for (int32_t i = 0; i < positionscount; i++) {
			auto deathroll = rng::get_random(state, seed_base + i) % 100;

			if (deathroll >= 1)
				continue;

			fill_position(state, n, i);
		}
	}

	inline void update_academia(sys::state& state) {
		for(const auto n : state.world.in_nation) {
			update_academia(state, n);
		}
	}

}
