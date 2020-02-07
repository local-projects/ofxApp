//
//  ofxAppTuioManager.cpp
//  PW_G8_SpiralApp
//
//  Created by Cameron Erdogan on 2/6/20.
//

#include "ofxAppTuioManager.h"

//these should come from the config
std::vector<int> ofxAppTuioManager::ports = {};
//std::vector<int> ofxAppTuioManager::ports = {
//    3333,
//    3334,
//    3335,
//    3336,
//    3337,
//    3338};

std::map<int, glm::vec2> ofxAppTuioManager::offsets = {};
//std::map<int, glm::vec2> ofxAppTuioManager::offsets = {
//    {3333, glm::vec2{0, 0}},
//    {3334, glm::vec2{ofxAppTuioManager::width, 0}},
//    {3335, glm::vec2{ofxAppTuioManager::width * 2, 0}},
//    {3336, glm::vec2{0, ofxAppTuioManager::height}},
//    {3337, glm::vec2{ofxAppTuioManager::width, ofxAppTuioManager::height}},
//    {3338, glm::vec2{ofxAppTuioManager::width * 2, ofxAppTuioManager::height}}};

int ofxAppTuioManager::width      = 3000;
int ofxAppTuioManager::height     = 1500;

bool ofxAppTuioManager::SWAP_DIMS = false;
bool ofxAppTuioManager::FLIP_X    = false;
bool ofxAppTuioManager::FLIP_Y    = false;

void ofxAppTuioManager::setup()
{
    setup( ofxAppTuioManager::ports );
}

void ofxAppTuioManager::setup( int port )
{
    setup( std::vector<int>( {port} ) );
}

void ofxAppTuioManager::setup( std::vector<int> ports )
{
    for ( auto& port : ports ) {
        PortTouchListener listener;
        m_listeners[port].setup( this, port );
    }
}

void ofxAppTuioManager::setup(std::vector<int> ports, std::map<int, glm::vec2> offsets, int screenWidth, int screenHeight){
    
    this->ports = ports;
    this->offsets = offsets;
    this->width = screenWidth;
    this->height = screenHeight;
    
    for ( auto& port : ports ) {
        PortTouchListener listener;
        m_listeners[port].setup( this, port );
    }
}
    
void ofxAppTuioManager::update()
{
    for (auto& kv : m_listeners) {
        kv.second.update();
    }
}

glm::vec2 ofxAppTuioManager::getConvertedCoord( CursorAtPort& e )
{
    float xFinal = SWAP_DIMS ? e.cursor.getPosition().getY() : e.cursor.getPosition().getX();
    float yFinal = SWAP_DIMS ? e.cursor.getPosition().getX() : e.cursor.getPosition().getY();

    xFinal = xFinal * width + offsets[e.port].x;
    yFinal = yFinal * height + offsets[e.port].y;

    xFinal = FLIP_X ? width - xFinal : xFinal;
    yFinal = FLIP_Y ? height - yFinal : yFinal;

    //std::cout << "initial vals:" << e.cursor.getPosition().getX() << ", " << e.cursor.getPosition().getY() << '\n';
    //std::cout << "converted to:" << xFinal << ", " << yFinal << '\n';

    return glm::vec2( xFinal, yFinal );
}
