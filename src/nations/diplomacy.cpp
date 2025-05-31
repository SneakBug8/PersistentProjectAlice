#pragma once

namespace nations {

// may create a relationship DO NOT call in a context where two or more such functions may run in parallel
// Makes FROM more indebted towards TO
void adjust_relationship(sys::state& state, dcon::nation_id from, dcon::nation_id to, float delta) {
	if(state.world.nation_get_owned_province_count(from) == 0 || state.world.nation_get_owned_province_count(to) == 0)
		return;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(to, from);
	if(!rel) {
		rel = state.world.force_create_unilateral_relationship(to, from);
	}
	auto& val = state.world.unilateral_relationship_get_opinion(rel);
	val = std::clamp(val + delta, -300.f, 300.f);
}

void monthly_adjust_relationship(sys::state& state, dcon::nation_id from, dcon::nation_id to, float delta) {
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(to, from);
	if(!rel) {
		rel = state.world.force_create_unilateral_relationship(to, from);
	}
	auto& val = state.world.unilateral_relationship_get_opinion(rel);
	val = std::clamp(val + delta, -300.f, std::max(val, 100.f));
}


}
