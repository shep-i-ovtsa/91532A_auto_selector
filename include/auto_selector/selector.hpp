#ifndef SELECTOR_HPP
#define SELECTOR_HPP
#include "pros/colors.hpp"
#include "pros/rtos.hpp"
#include "pros/screen.h"
#include "pros/screen.hpp"
#include "bad_gif/bad_gif.hpp"
#include "bad_gif/compositor.hpp"
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <deque>
//! for the poor soul trying to understand my madness... im very sorry

struct routine {
    public:
    std::string description;
    std::string routine_name;
    std::function<void()> routine_function;
    routine(const std::string& name, std::function<void()> in) : routine_name(name), routine_function(in) {}
    routine(const std::string& name, const std::string& desc, std::function<void()> in) : routine_name(name), description(desc), routine_function(in) {}
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


        void load(const std::string& path, bool animated = true) {
            vid->x = chord1.x;
            vid->y = chord1.y;
            vid->draw_width= chord2.x - chord1.x;
            vid->draw_height = chord2.y - chord1.y;
            vid->looping = animated;
            vid->load(path);
        }
        void open(const std::string& path, bool animated = true){
            vid->x = chord1.x;
            vid->y = chord1.y;
            vid->draw_width= chord2.x - chord1.x;
            vid->draw_height = chord2.y - chord1.y;
            vid->looping = animated;
            vid->open(path);
        }
        void set_bounds(position c1, position c2) {
            chord1 = c1;
            chord2 = c2;
            vid->x = c1.x;
            vid->y = c1.y;
            vid->draw_width= c2.x - c1.x;
            vid->draw_height = c2.y - c1.y;
        }
        void reset() { vid->reset(); }
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
    std::function<void()> user_menu;

    void add_routine(const std::string& name, std::function<void()> in) {
        route_list.emplace_back(name, in);
        total++;
    }

    void add_routine(const std::string& name, const std::string& description,std::function<void()> in) {
        route_list.emplace_back(name, description, in);
        total++;
    }

    void select_routine(routine chosen) { selected_route = chosen; }
    void run(){ if (!route_list.empty()) selected_route.routine_function(); }
    void stop() { running = false; }
    uint16_t refresh_speed = 300;
    void set_refresh_speed(uint16_t new_speed){refresh_speed = new_speed;}
    static void start(void* param) {
        selector* self = static_cast<selector*>(param);
        (self -> user_menu) ? self -> user_menu() : self -> default_menu(); //so this is a neat trick where if there is something in the function it returns true and runs the user defined menu. or else it just uses the default
        while (1) {
            self -> capture_touch();
            self->render();

            pros::delay(self -> refresh_speed);
            if (!self->running) break;
        }
    }
    void default_menu() {
        element background({0, 0}, {screen_x, screen_y});
        background.on_render = [&](selector::element&) {
            background.fill();
        };
        background.set_background_color(pros::Color::light_pink);
//--------------------------------------------------------------------------------
        element info_bar({5, 5}, {475, 25});
        info_bar.on_render = [&](selector::element& el) {
            info_bar.fill();
            draw_text(el.chord1.x + 5,el.chord1.y + 5,"Auton Selector");
        };
        info_bar.set_background_color(pros::Color::light_salmon);
//-------------------------------------------------------------------------------------
        element description_box({240, 30}, {475, 235});
        description_box.on_render = [&](selector::element& el) {
            description_box.fill();
            draw_text(el.chord1.x + 5,el.chord1.y + 5,selected_route.description);
        };
        description_box.set_background_color(pros::Color::light_salmon);
//------------------------------------------------------------------------------------------------
        element auton_list({5, 30}, {235, 235});
        auton_list.set_background_color( pros::Color::light_salmon);
//------------------------------------------------------------------------------------------------
        int16_t y_offset = 35;
        for (routine& route : route_list) {
            button b({10, y_offset},{230, static_cast<int16_t>(y_offset + 25)});

            b.set_background_color(pros::Color::pale_turquoise);
            b.on_render = [&](selector::element& el) {
                b.fill();
                draw_text(el.chord1.x + 5,el.chord1.y + 5,route.routine_name);
            };
            b.on_click([&, route]() {
                selected_route = route;
                pros::Color temp =static_cast<pros::Color>(pros::screen::get_pen());
                pros::screen::set_pen(pros::Color::black);
                pros::screen::fill_rect(0,0,480,240);
                pros::screen::set_pen(temp);
            });
            y_offset += 30;
        }
//---------------------------------------------------------------------------------------------------
        image description_gif({240, 60},{340, 235});
        description_gif.load("/usd/9mm.apl",true);
//------------------------------------------------------------------
        image description_gif2({358, 60}, {475, 235});
        description_gif2.load("/usd/apple.apl",true);
//---------------------------------------------------------------------------------------------------

    }

    //since we dont want to leave dangling memory open to users we should avoid having local variables open to them

    //lets just make some factory methods for this instead


    void capture_touch(){
        pros::screen_touch_status_s_t touch = pros::screen::touch_status();
          currently_pressed.x = touch.x;
            currently_pressed.y = touch.y;
            currently_taped =touch.touch_status == pros::E_TOUCH_PRESSED || touch.touch_status == pros::E_TOUCH_HELD;
    }
    void render();

    static void draw_text(int16_t x, int16_t y, const std::string& text) {
        pros::screen::print(pros::E_TEXT_MEDIUM, x, y, text.c_str());
    }

    static pros::Color current_color;
    routine selected_route = bug_breaker;
    std::vector<routine> route_list;

    private:

    static void bug_breakers() {}
    routine bug_breaker = routine("bug breaker","because i have this itsy bitsy bug in the auto selectr i just make a ""void function a stand in so it dosent cry",bug_breakers);
    bool currently_taped = false;
    pros::screen_touch_status_s_t screen_state{};
    bool running = true;
    int  total   = 0;


    compositor comp; 

    const int16_t screen_x = 480;
    const int16_t screen_y = 240;
    const int8_t spacing= 5;
    bool dirty = true;

    position currently_pressed{0, 0};
    std::deque<element> screen_elements;
    std::deque<button> button_list;
    std::deque<image> image_list;
    public:
    element& make_element(position chord1, position chord2){
        element new_element(chord1 , chord2);
        screen_elements.emplace_back(new_element);
        return screen_elements.back();
    }
    button& make_button(position chord1, position chord2){
        button new_button(chord1 , chord2);
        button_list.emplace_back(new_button);
        return button_list.back();
    }
    image& make_image(position chord1, position chord2, const std::string& link, bool open = false){
        image new_image(chord1 , chord2);
        (!open) ? new_image.open(link) : new_image.load(link);
        image_list.emplace_back(new_image);
        return image_list.back();
    }
    routine& get_selected(){
        return selected_route;
    }
};

#endif