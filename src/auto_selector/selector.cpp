#include "selector.hpp"
pros::Color selector::current_color = pros::Color::alice_blue;

void selector::render(){
    
        for (button& btn : button_list) {
            if (!btn.visible) continue;

            bool inside =
                currently_pressed.x >= btn.chord1.x &&
                currently_pressed.x <= btn.chord2.x &&
                currently_pressed.y >= btn.chord1.y &&
                currently_pressed.y <= btn.chord2.y;

            bool pressed = currently_taped;
            btn.highlighted = inside && pressed;

            if (pressed && inside && !btn.was_pressed) {
                if (btn.action) btn.action();
                btn.was_pressed = true;
            }
            if (!pressed) btn.was_pressed = false;
        }

        for (element& el : screen_elements) {
            if (!el.visible) continue;
            if (el.on_render) el.on_render(el);
        }

        for (button& btn : button_list) {
            if (!btn.visible) continue;
            if (btn.on_render) btn.on_render(btn);
        }

        if (!image_list.empty()) {
            comp.clear();

            for (image& img : image_list) {
                if (!img.visible) continue;
                img.vid->update(pros::millis());
                img.vid->renderTo(
                    comp.screen,
                    compositor::SCREEN_W,
                    compositor::SCREEN_H
                );
            }

            comp.present();
        }
}