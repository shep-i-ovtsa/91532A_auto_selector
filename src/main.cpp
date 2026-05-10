#include "main.h"
#include "auto_selector/selector.hpp"
#include "pros/misc.h"
#include "pros/misc.hpp"
#include "pros/screen.h"
#include "pros/screen.hpp"

    selector new_selector; //its important to have this as an object where your methods can reach them obs
void test_auto(){
    pros::screen::set_pen(pros::Color::white);
    pros::screen::fill_rect(0,0,200,480);
    pros::screen::print(pros::E_TEXT_LARGE, 0, "test auto");
    pros::screen::print(pros::E_TEXT_SMALL, 2, "just a test lmao");
}
void initialize() {
    pros::lcd::initialize();
    new_selector.add_routine("new_routine","just a test lmao1",test_auto);
    new_selector.add_routine("new_routine2","just a test lmao2",test_auto);
    new_selector.add_routine("new_routine3","just a test lmao3",test_auto);
    new_selector.add_routine("new_routine4","just a test lmao4",test_auto);
    new_selector.add_routine("new_routine5","just a test lmao5",test_auto); //this adds a new routine of name new routine5 , a description then the function you want it to run
    new_selector.add_routine("new_routine6","just a test lmao6",test_auto);
    new_selector.add_routine("new_routine7","just a test lmao7",test_auto);
    new_selector.add_routine("new_routine8","just a test lmao8",test_auto);    
    pros::Task(new_selector.start, &new_selector);  //important you pass the selector reference aswell or it will DIEEE

}

void disabled() {}

void autonomous() {
    new_selector.run(); //obviously runs whatever is selected
    new_selector.stop();
}

void opcontrol() {
    pros::Controller master = pros::Controller(pros::E_CONTROLLER_MASTER);
    while(1){
        if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)){
            autonomous();
            new_selector.stop(); //stops the menu loop
        }
        pros::delay(20);
    }
}
