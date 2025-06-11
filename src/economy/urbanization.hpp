#include "dcon_generated.hpp"
#include "system_state.hpp"

#include "demographics.hpp"

namespace economy {

float get_market_urbanization(sys::state& state, dcon::market_id m);
float get_province_urbanization(sys::state& state, dcon::province_id p);

float get_effective_pop_life_needs(sys::state& state, dcon::pop_type_id pop_type, dcon::commodity_id c, dcon::market_id m);
float get_effective_pop_everyday_needs(sys::state& state, dcon::pop_type_id pop_type, dcon::commodity_id c, dcon::market_id m);
float get_effective_pop_luxury_needs(sys::state& state, dcon::pop_type_id pop_type, dcon::commodity_id c, dcon::market_id m);

}
