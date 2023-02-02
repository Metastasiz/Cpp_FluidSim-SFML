#include <iostream>
#include <SFML/Graphics.hpp>
#include "Fluid.h"

using namespace sf;

Fluid fluid{};

int main()
{
    const static int w = fluid.w; const static int h = fluid.h;
    //
    RenderWindow window(VideoMode(w, h), "Fluid Simulation");
    Text control, start;
    Font font;
    Image image;
    Texture texture;
    Sprite sprite;
    Event event;
    //
    if (!font.loadFromFile("/Users/mark/CLionProjects/FluidSimSFML/arial.ttf")){}
    //simple setup
    start.setFont(font);
    start.setCharacterSize(h/12);
    start.setColor(Color::Red);
    start.setString("Click to Start");
    start.setStyle(Text::Bold);
    start.setPosition(w/2-h*7/24,0);
    //
    control.setFont(font);
    control.setCharacterSize(h/12);
    control.setColor(Color::White);
    control.setString(
                      "R: to reset fluid sim\n"
                      "D: to switch draw mode\n"
                      "G: to turn on/off gravity\n"
                      "S: to turn on/off spawning waves\n"
                      "1,2,3: to turn on/off using R,G,B\n"
                      "RGB is NOT finished\n"
                      "4: to reset RGB\n"
                      "M: to increase wave chance\n"
                      "N: to decrease wave chance\n"
                      );
    control.setPosition(0,h*3/24);
    //
    int prevX = w*Mouse::getPosition(window).x/window.getSize().x, prevY = h*Mouse::getPosition(window).y/window.getSize().y;
    int max = 1300, min = 850, step = (max-min)/155, times = 155;
    //
    bool hold = false, grav = false, draw = false, menu = true;
    bool red = false, blue = false, green = false;
    double gravity = 0.01;
    //
    Uint8 *pixels = new Uint8[w*h*4];
    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){
            pixels[4*((y*w)+x)+0] = 0;    //r
            pixels[4*((y*w)+x)+1] = 0;    //g
            pixels[4*((y*w)+x)+2] = 0;    //b
            pixels[4*((y*w)+x)+3] = 255;    //a
        }
    }
    //
    while (window.isOpen())
    {
        while (menu){
            while (window.pollEvent(event))
            {
                if (event.type == Event::EventType::MouseButtonPressed){
                    menu = false;break;
                }
            }
            window.clear();
            window.draw(start);
            window.draw(control);
            window.display();
        }
        fluid.runIns();
        for (int y = 0; y < h; y++){
            for (int x = 0; x < w; x++){
                if (fluid.den[y][x]>=fluid.outOfBound){
                    pixels[4*((y*w)+x)+0] = 255;    //r
                    pixels[4*((y*w)+x)+1] = 0;    //g
                    pixels[4*((y*w)+x)+2] = 0;    //b
                    pixels[4*((y*w)+x)+3] = 255;    //a
                }else {
                    int inc    = ((max-fluid.den[y][x])/step);
                    inc        = inc<0?0:inc>255?255:inc;
                    int incR    = ((max-fluid.inkR[y][x])/step);
                    incR        = incR<0?0:incR>255?255:incR;
                    int incG    = ((max-fluid.inkG[y][x])/step);
                    incG        = incG<0?0:incG>255?255:incG;
                    int incB    = ((max-fluid.inkB[y][x])/step);
                    incB        = incB<0?0:incB>255?255:incB;
                    pixels[4 * ((y * w) + x) + 0] = 0 + inc;    //r
                    pixels[4 * ((y * w) + x) + 1] = 0 + inc;    //g
                    pixels[4 * ((y * w) + x) + 2] = 0 + inc;    //b
                    pixels[4 * ((y * w) + x) + 3] = 255;    //a
                }
            }
        }
        while (window.pollEvent(event))
        {
            switch (event.type){
                case Event::Closed:
                    window.close();break;
                case Event::EventType::MouseButtonPressed:
                    hold = true;break;
                case Event::EventType::MouseButtonReleased:
                    hold = false;break;
                case Event::EventType::KeyPressed:
                    switch (event.key.code){
                        case Keyboard::G: grav = !grav; break;
                        case Keyboard::R: fluid.INIT(); break;
                        case Keyboard::D: draw = !draw; break;
                        case Keyboard::S: fluid.spawn = !fluid.spawn; break;
                        case Keyboard::Num1: red = !red; break;
                        case Keyboard::Num2: green = !green; break;
                        case Keyboard::Num3: blue = !blue; break;
                        case Keyboard::Num4: red = false;green = false;blue = false; break;
                        case Keyboard::M: fluid.chance = fluid.chance>=100?100:fluid.chance+1; break;
                        case Keyboard::N: fluid.chance = fluid.chance<=0?0:fluid.chance-1; break;
                    }
            }
        }
        if (draw && hold){
            int x = w*Mouse::getPosition(window).x/window.getSize().x;
            int y = h*Mouse::getPosition(window).y/window.getSize().y;
            fluid.den[y][x] = fluid.outOfBound;
            fluid.vxn[y][x] = 0;
            fluid.vyn[y][x] = 0;
        }
        if (!draw && hold){
            int clrT = 1500;
            int x = w*Mouse::getPosition(window).x/window.getSize().x;
            int y = h*Mouse::getPosition(window).y/window.getSize().y;
            int h = prevY - y;int w = prevX - x;
            long deg = atan2(h,w);
            if (red)    {fluid.inkR[prevY][prevX] -= clrT;fluid.inkR[y][x] += clrT;}
            if (green)  {fluid.inkG[prevY][prevX] -= clrT;fluid.inkG[y][x] += clrT;}
            if (blue)   {fluid.inkB[prevY][prevX] -= clrT;fluid.inkB[y][x] += clrT;}
            fluid.vxn[y][x] += -1*cos(deg);
            fluid.vyn[y][x] += 1*sin(deg);
            prevX = x;prevY = y;

            //
            //
            //
        }
        if (grav){
            for (int y = 0; y < h; y++){
                for (int x = 0; x < w; x++){
                    if (fluid.den[y][x]>=fluid.outOfBound)continue;
                    fluid.vyn[y][x] = fluid.vyn[y][x]<-gravity?fluid.vyn[y][x]:fluid.vyn[y][x]-gravity;
                }
            }
        }
        image.create(w,h,pixels);
        texture.create(w,h);
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.clear();
        window.draw(sprite);
        window.display();
    }
}