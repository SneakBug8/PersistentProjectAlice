#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "academy.hpp"

namespace ui {
    class academia_window_button : public button_element_base {
    public:
        void button_action(sys::state& state) noexcept override {
        }

        tooltip_behavior has_tooltip(sys::state& state) noexcept override {
            return tooltip_behavior::variable_tooltip;
        }

        void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
            auto box = text::open_layout_box(contents, 0);

            auto academia = dcon::fatten(state.world, state.world.nation_get_academia_from_academia_loyalty(state.local_player_nation));

            for (int32_t i = 0; i < academy::positionscount; i++) {
                auto positionname = text::produce_simple_string(state, academy::PositionNames[i]);
                auto fname = text::produce_simple_string(state, state.to_string_view(academia.get_first_names(i)));
				auto lname = text::produce_simple_string(state, state.to_string_view(academia.get_last_names(i)));

                auto skill = text::format_wholenum(academia.get_skills(i));
                text::add_line(state, contents, positionname + ": " + fname + " " + lname + " (" + skill + ")", 0);
            }
        }
    };
}
