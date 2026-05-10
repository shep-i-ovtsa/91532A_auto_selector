#ifndef SELECTOR_HPP
#define SELECTOR_HPP
#include "pros/colors.hpp"
#include "pros/rtos.hpp"
#include "pros/screen.h"
#include "pros/screen.hpp"
#include "C:\Users\fenix\Documents\lemlib\src\bad_apple_player\bad_gif.hpp"
#include "C:\Users\fenix\Documents\lemlib\src\bad_apple_player\compositor.hpp"
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

//! for the poor soul trying to understand my madness... im very sorry

struct routine {
    public:
    std::string description;
    std::string routine_name;
    std::function<void()> routine_function;
    routine(const std::string& name, std::function<void()> in)
        : routine_name(name), routine_function(in) {}
    routine(const std::string& name, const std::string& desc, std::function<void()> in)
        : routine_name(name), description(desc), routine_function(in) {}
};

class selector {
    public:

    struct position {
        int16_t x = 0;
        int16_t y = 0;
    };

    struct element {
        bool is_image = false;
        bool visible = true;
        position chord1;
        position chord2;
        pros::Color Background_Color = pros::Color::white;
        pros::Color Text_Color = pros::Color::black;
        std::string content;
        std::function<void(element&)> on_render;

        void set_background_color(pros::Color c) { Background_Color = c; }
        void set_text_color(pros::Color c) { Text_Color = c; }

        void fill() {
            if (current_color != Background_Color) {
                current_color = Background_Color;
                pros::screen::set_pen(Background_Color);
            }
            pros::screen::fill_rect(chord1.x, chord1.y, chord2.x, chord2.y);
        }

        element(position c1, position c2) : chord1(c1), chord2(c2) {}
    };

    struct image {
        position chord1;
        position chord2;
        bool visible = true;

        std::shared_ptr<video_stream> vid = std::make_shared<video_stream>();

        image(position c1, position c2) : chord1(c1), chord2(c2) {}


        bool load(const std::string& path, bool animated = true , bool open = false) {
            vid->x = chord1.x;
            vid->y = chord1.y;
            vid->draw_width= chord2.x - chord1.x;
            vid->draw_height = chord2.y - chord1.y;
            vid->looping = animated;

            (open) ? vid->load(path) : vid->open(path);
            return true;
        }

        void set_bounds(position c1, position c2) {
            chord1 = c1;
            chord2 = c2;
            vid->x = c1.x;
            vid->y = c1.y;
            vid->draw_width= c2.x - c1.x;
            vid->draw_height = c2.y - c1.y;
        }

        void set_visible(bool v){ visible = v; vid->visible = v;}
        bool is_done() const{ return vid->ended; }
        int frame_count() const { return vid->frame_count();}
    };

    struct button : element {
        std::function<void()> action;
        void on_click(std::function<void()> cb) { action = cb; }
        bool was_pressed = false;
        bool highlighted= false;
        pros::Color highlighted_color = pros::Color::gray;
        void set_highlighted_color(pros::Color c) { highlighted_color = c; }
        button(position c1, position c2) : element(c1, c2) {}
    };

    selector() {}

    void add_routine(const std::string& name, std::function<void()> in) {
        route_list.emplace_back(name, in);
        total++;
    }

    void add_routine(const std::string& name, const std::string& description,std::function<void()> in) {
        route_list.emplace_back(name, description, in);
        total++;
    }

    void add_screen_element(const element& e) { screen_elements.emplace_back(e); }
    void add_button(const button& b){ button_list.emplace_back(b); }


    void add_image(const image& img) { image_list.emplace_back(img); }

    void select_routine(routine chosen) { selected_route = chosen; }
    void run(){ if (!route_list.empty()) selected_route.routine_function(); }
    void stop() { running = false; }

    static void start(void* param) {
        selector* self = static_cast<selector*>(param);


        element background({0, 0}, {self->screen_x, self->screen_y});
        background.on_render = [&](selector::element&) { background.fill(); };
        background.set_background_color(pros::Color::light_pink);
        self->add_screen_element(background);

        element info_bar({5, 5}, {475, 25});
        info_bar.on_render = [&](selector::element& el) {
            info_bar.fill();
            self->draw_text(el.chord1.x + 5, el.chord1.y + 5, "Auton Selector");
        };
        info_bar.set_background_color(pros::Color::light_salmon);
        self->add_screen_element(info_bar);

        element description_box({240, 30}, {475, 235});
        description_box.on_render = [&](selector::element& el) {
            description_box.fill();
            self->draw_text(el.chord1.x + 5, el.chord1.y + 5,
                            self->selected_route.description);
        };
        description_box.set_background_color(pros::Color::light_salmon);
        self->add_screen_element(description_box);

        element auton_list({5, 30}, {235, 235});
        auton_list.set_background_color(pros::Color::light_salmon);
        self->add_screen_element(auton_list);

        int16_t y_offset = 35;
        for (routine& route : self->route_list) {
            button b({10, y_offset}, {230, static_cast<int16_t>(y_offset + 25)});
            b.set_background_color(pros::Color::pale_turquoise);

            b.on_render = [self, route](selector::element& el) {
                self->draw_text(el.chord1.x + 5, el.chord1.y + 5,
                route.routine_name);
            };

            b.on_click([self, route]() {
                self->selected_route = route;
                pros::Color temp = static_cast<pros::Color>(pros::screen::get_pen());
                pros::screen::set_pen(pros::Color::black);
                pros::screen::fill_rect(0, 0, 480, 240);
                pros::screen::set_pen(temp);
            });

            self->add_button(b);
            y_offset += 30;
        }
        image description_gif({240, 60}, {240+100, 235});
        description_gif.load("/usd/9mm.apl",true);
        self -> add_image(description_gif);
        image description_gif2({240+118, 60}, {475, 235});
        description_gif2.load("/usd/apple.apl", true);
        self -> add_image(description_gif2);
        while (1) {
            pros::screen_touch_status_s_t touch = pros::screen::touch_status();

            self->currently_pressed.x = touch.x;
            self->currently_pressed.y = touch.y;
            self->currently_taped =
                touch.touch_status == pros::E_TOUCH_PRESSED ||
                touch.touch_status == pros::E_TOUCH_HELD;

            self->tick();

            pros::delay(300);
            if (!self->running) break;
        }
    }

    void tick() {

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

    void draw_text(int16_t x, int16_t y, const std::string& text) {
        pros::screen::print(pros::E_TEXT_MEDIUM, x, y, text.c_str());
    }

    static pros::Color current_color;

    private:

    static void bug_breakers() {}
    routine bug_breaker = routine(
        "bug breaker",
        "because i have this itsy bitsy bug in the auto selectr i just make a "
        "void function a stand in so it dosent cry",
        bug_breakers
    );

    bool currently_taped = false;
    pros::screen_touch_status_s_t screen_state{};
    bool running = true;
    int  total   = 0;

    routine selected_route = bug_breaker;
    std::vector<routine> route_list;
    std::vector<element> screen_elements;
    std::vector<button> button_list;
    std::vector<image> image_list;

    compositor comp; 

    const int16_t screen_x = 480;
    const int16_t screen_y = 240;
    const int16_t spacing= 5;
    bool dirty = true;

    position currently_pressed{0, 0};
};

#endif