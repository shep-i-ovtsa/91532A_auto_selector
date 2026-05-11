#include "main.h"
#include "auto_selector/selector.hpp"
#include "pros/colors.hpp"
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
    new_selector.user_menu = [&]() { //! THIS MY CHILDREN, is the tutorial on how to use my ui system! good luck

    //make_element creates the element directly inside the selector and gives
    //you back a reference to it. It lives as long as the selector does so you dont have to worry about dangling copies trying to call from cleared memory
    auto& background = new_selector.make_element({0, 0}, {480, 240}); //theese brackets define our chordinate 1 and 2 from top left corner to top right
    background.set_background_color(pros::Color::midnight_blue); //elements have some functions in them you can call like background color
    background.on_render = [&](selector::element& el) {
        background.fill(); //the fill function just fills in the area that it contains
    };

    // Elements render in the order they were made. so drawn first  is the one that was made first. so if you want something to have a higher layer than something else you either move their position in the stack or just order them right
    // so i personally recoomend you make your backgrounds before overlays
    auto& title_bar = new_selector.make_element({0, 0}, {480, 22});
    title_bar.set_background_color(pros::Color::steel_blue);
    title_bar.on_render = [&](selector::element& el) { //the on render function is a function the rendering pipeline calls on all elements and buttons when they get passed over. if you want to run code
                                                                //when the thing gets rendered do it here. elements arent inherently rendered so what happens on render is decided by YOU
        title_bar.fill();
        selector::draw_text(8, 4, "Auton Selector  >.<  tap a routine to select"); //this is just a helper function to draw text on the screen
    };




    //make image follows the same pattern. you set the boundary points, but this time we actually have to put in the link to our video file
    //since we use my bad apple player(modified for this ofc) you need to convert videos into the right format using our converter but its super easy
    auto& logif = new_selector.make_image({252, 140}, {474, 234},"/usd/9mm.apl" , true);
    //logif.open("/usd/9mm.apl"); //by default we load files into memory which is fine for small files, but if your file is too huge for memory use open to stream it instead
    auto& auto2 = new_selector.make_image({252, 140}, {474, 234},"/usd/apple.apl", true);
    logif.set_visible(false);
    auto2.set_visible(false); 


    // get_selected() lets you read whichever routine the user has tapped because this is an auto selector ofcourse
    auto& info_panel = new_selector.make_element({248, 28}, {474, 234});
    info_panel.set_background_color(pros::Color::dark_blue);
    info_panel.on_render = [&](selector::element& el) { //you might have noticed, what are theese weird structures were using for on render. you see its actually a lambda function
                                                                //which is just essentially creating a function in line since theese are functions after all
        info_panel.fill();
        selector::draw_text(el.chord1.x + 6, el.chord1.y + 8,  "Selected:"); //if your element has some sort of data like its coordinates. you need to call it trough whatever you name your reference
        selector::draw_text(el.chord1.x + 6, el.chord1.y + 26,new_selector.get_selected().routine_name);
        selector::draw_text(el.chord1.x + 6, el.chord1.y + 50,new_selector.get_selected().description);
    };

    // make_button works the same way lives in the selector
    //Capture route by value so each loop iteration closes over its own copy of the routine
    int16_t y = 30;
    for (routine& route : new_selector.route_list) {
        auto& b = new_selector.make_button({6, y}, {240, static_cast<int16_t>(y + 28)});
        b.set_background_color(pros::Color::cadet_blue);

        b.on_render = [&, route](selector::element& el) {
            pros::screen::set_pen(b.highlighted ? pros::Color::light_cyan: b.Background_Color);
            pros::screen::fill_rect(b.chord1.x, b.chord1.y,b.chord2.x, b.chord2.y);
            selector::draw_text(b.chord1.x + 6, b.chord1.y + 7,route.routine_name);
        };

        b.on_click([&, route]() {
            new_selector.select_routine(route);
            pros::screen::set_pen(pros::Color::white);
            pros::screen::fill_rect(0, 0, 480, 240);
            for(selector::image i : new_selector.image_list){
                i.set_visible(0); //this is an example of looping trough our stacks. if you want to go trough every image and make them invisible, we just go trough every image registered and make them inviisble
            }
            if(route.routine_name == "loli_run"){logif.set_visible(1);}
            if(route.routine_name == "apple_run"){auto2.set_visible(1);} //then we come over here and make them visible if they match the selected routine
        });

        y += 33;
    }

    
    //now getting into buttons and actually adding functionality
    //the rendering pipeline will call on buttons whos boundaries are over a click trough the .onclick function. we can use this for functionality like setting things invisible or not
    auto& toggle_btn = new_selector.make_button({252, 110}, {474, 135});
    toggle_btn.set_background_color(pros::Color::slate_blue);
    toggle_btn.on_render = [&](selector::element& el) {
        toggle_btn.fill();
        selector::draw_text(toggle_btn.chord1.x + 6, toggle_btn.chord1.y + 6, "hide gif");
    };
    toggle_btn.on_click([&]() {
        for(selector::image i : new_selector.image_list){
            i.set_visible(0);
        }
    });


    //note that this screen setup happens before anything is rendered so there wont be any weird flickers or unpredictability from all of this being created and configed at once
    //since we use a mix of factory methods and call and response structure you litterally just make a new object and then dont worry about it
};  //now you might not see this test screen because of this tutorial but if this function dosent get defined. its ok, because the default test screen will play instead 

/* now for reference heres all the functions you need to know -----------------

 elements
     make_element({x1,y1},{x2,y2})
     creates an element in the selector and gives you a reference to it top left corner to bottom right corner

     .set_background_color(pros::Color::x)
      sets the color that fill() will use

     .set_text_color(pros::Color::x) 
     sets the color text will draw in

     .fill()                                
     fills the elements bounding box with its background color

     .visible                               
      bool set to false to skip it in the render pipeline entirely good for layered menus

     .on_render = [&](selector::element& el) 
     the function the pipeline calls every tick nothing happens to your element unless you put it here

     .chord1.x or .chord1.y                 
      top left corner coordinates
    .chord2.x or .chord2.y                 
    bottom right corner coordinates

 buttons (everything elements have plus these)
     make_button({x1,y1},{x2,y2} )           
     same as make_element but gives you a button

     .on_click([&](){ })                    
     fires once per tap

     .highlighted                           
     automatically true while a finger is held over it

 images
     make_image({x1,y1},{x2,y2}, path , open = false)      
     creates the image already pointed at your file and loaded
    if you want to open the file instead of load it(look below for the difference) set open to true instead of false like its defaulted to

     .load(path, looping)                   
     loads the whole file into memory. fast playback, use for small files (we default to this in the make_image)

     .open(path)
     streams frame by frame off the sd card instead. use when your file is too big to fit in memory

     .set_visible(bool)                      
     hides or shows the image

     .is_done()                             
     returns true when a non looping animation has finished playing. useful for chaining things off the end of an intro

     .frame_count()                         
     how many frames are in the loaded file

     .set_bounds({x1,y1},{x2,y2})           
     moves and resizes the image after creation if you need to

    fpr complex maniputlation of videos. use the video stream exposed to you


 deques holding your ui elements
     new_selector.screen_elements           
     everything made with make_element, added to the back

     new_selector.button_list               
     everything made with make_button

     new_selector.image_list                
     everything made with make_image

     new_selector.route_list                
     all the routines you added with add_routine, loop over this to build your buttons

 selector functions 
     add_routine(name, description, function)   
     registers an autonomous routine. the name and description show up wherever you draw them

     get_selected()                             
     returns the currently selected routine so you can read its name, description, or just check if anything is picked

     select_routine(route)                      
     manually set the selected routine, this is what your button on_clicks should call

     run()                                      
     runs whatever is currently selected. call this from autonomous()

     stop()                                     
     kills the render loop. call this when you are done with the menu

     pros::Task(new_selector.start, &new_selector) 
     how you kick the whole thing off in initialize() dont forget to pass the reference or it will DIEEEE A VIOLENT DEATH!
 rendering tips and functions
    change_refresh_speed(uint16_t new_speed)
    just changes the speed at which we refresh the screen. your gifs are also presented frame by frame at this speed btw so make sure their frame rates are acceptable for the speed you choose

    to change our image renderers pallete go to compositor.hpp in include/bad_gif but note that the default pallete and the one in the compositor is the same one the converter uses so i wouldnt drastically change colors
    unless you really want that effect
*/
    pros::lcd::initialize();
    new_selector.add_routine("loli_run","just a test lmao1",test_auto);
    new_selector.add_routine("apple_run","just a test lmao2",test_auto);
    new_selector.add_routine("new_routine3","just a test lmao3",test_auto);
    new_selector.add_routine("new_routine4","just a test lmao4",test_auto);
    new_selector.add_routine("new_routine5","just a test lmao5",test_auto); //this adds a new routine of name new routine5 , a description then the function you want it to run
    new_selector.add_routine("new_routine6","just a test lmao6",test_auto);
    new_selector.add_routine("new_routine7","just a test lmao7",test_auto);
    new_selector.add_routine("new_routine8","just a test lmao8",test_auto);    
    pros::Task(new_selector.start, &new_selector);  //important you pass the selector reference aswell or it will DIEEE
    //its super important that you dont put the start in the wrong place or it just will NOT do anything come competition

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
