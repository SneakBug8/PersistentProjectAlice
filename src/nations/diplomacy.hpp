namespace nations {

// may create a relationship DO NOT call in a context where two or more such functions may run in parallel
// Makes FROM more indebted towards TO
void adjust_relationship(sys::state& state, dcon::nation_id from, dcon::nation_id to, float delta);

void monthly_adjust_relationship(sys::state& state, dcon::nation_id from, dcon::nation_id to, float delta);

}
