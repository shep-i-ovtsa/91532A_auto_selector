#pragma once
#include "main.h"
#include "pros/colors.hpp"
#include "pros/screen.hpp"
#include <array>
#include <vector>

class compositor {
public:
    static constexpr int SCREEN_W = 480;
    static constexpr int SCREEN_H = 240;
    std::vector<uint8_t> screen;
    compositor() {
        screen.resize(static_cast<size_t>(SCREEN_W * SCREEN_H), 255);
    }
    void clear() {
        std::fill(screen.begin(), screen.end(), static_cast<uint8_t>(255));
    }
    void present() {
        for (int row = 0; row < SCREEN_H; ++row) {
            const int row_off = row * SCREEN_W;
            int run_start = 0;
            uint8_t run_color = screen[row_off];
            for (int col = 1; col <= SCREEN_W; ++col) {
                uint8_t cur = (col < SCREEN_W) ? screen[row_off + col]
                                               : static_cast<uint8_t>(~run_color);

                if (cur != run_color) {
                    if (run_color != 255) {pros::screen::set_pen(palette[run_color]);pros::screen::fill_rect(run_start,row,col,row +1);}
                    run_start = col;
                    run_color = cur;
                }
            }
        }
    }

private:
    std::array<pros::Color, 64> palette = {{
        pros::Color::black,              // 0
        pros::Color::white,              // 1 //binary color scheme
        pros::Color::red,                // 2
        pros::Color::lime,               // 3
        pros::Color::blue,               // 4
        pros::Color::yellow,             // 5
        pros::Color::cyan,               // 6
        pros::Color::magenta,            // 7
        pros::Color::orange,             // 8
        pros::Color::purple,             // 9
        pros::Color::pink,               // 10
        pros::Color::maroon,             // 11
        pros::Color::green,              // 12
        pros::Color::navy,               // 13
        pros::Color::teal,               // 14
        pros::Color::olive,              // 15
        pros::Color::coral,              // 16 //16 bit color scheeme
        pros::Color::salmon,             // 17
        pros::Color::gold,               // 18
        pros::Color::yellow_green,       // 19
        pros::Color::medium_aquamarine,  // 20
        pros::Color::sky_blue,           // 21
        pros::Color::steel_blue,         // 22
        pros::Color::violet,             // 23
        pros::Color::indigo,             // 24
        pros::Color::orchid,             // 25
        pros::Color::hot_pink,           // 26
        pros::Color::crimson,            // 27
        pros::Color::tomato,             // 28
        pros::Color::sienna,             // 29
        pros::Color::tan,                // 30
        pros::Color::silver,             // 31
        pros::Color::dark_red,           // 32
        pros::Color::dark_orange,        // 33
        pros::Color::dark_green,         // 34
        pros::Color::dark_blue,          // 35
        pros::Color::dark_cyan,          // 36
        pros::Color::dark_magenta,       // 37
        pros::Color::dark_violet,        // 38
        pros::Color::dark_orchid,        // 39
        pros::Color::dark_salmon,        // 40
        pros::Color::dark_khaki,         // 41
        pros::Color::dark_gray,          // 42
        pros::Color::medium_slate_blue,  // 43
        pros::Color::dark_slate_gray,    // 44
        pros::Color::dark_turquoise,     // 45
        pros::Color::dark_goldenrod,     // 46
        pros::Color::dark_sea_green,     // 47
        pros::Color::medium_blue,        // 48
        pros::Color::medium_purple,      // 49
        pros::Color::medium_orchid,      // 50
        pros::Color::medium_violet_red,  // 51
        pros::Color::medium_sea_green,   // 52
        pros::Color::medium_slate_blue,  // 53
        pros::Color::medium_spring_green,// 54
        pros::Color::medium_turquoise,   // 55
        pros::Color::light_blue,         // 56
        pros::Color::light_coral,        // 57
        pros::Color::light_cyan,         // 58
        pros::Color::light_green,        // 59
        pros::Color::light_gray,         // 60
        pros::Color::light_pink,         // 61
        pros::Color::light_salmon,       // 62
        pros::Color::light_yellow,       // 63 64 color scheme 
    }};
};