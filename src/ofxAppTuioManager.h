//
//  ofxAppTuioManager.hpp
//  PW_G8_SpiralApp
//
//  Created by Cameron Erdogan on 2/6/20.
//

#pragma once

#include <stdio.h>
#include <functional>
#include <vector>

#include "ofxTuio.h"

struct TouchAtPort
{
    glm::vec2 pos;
    int port;
    int sid;
};

struct CursorAtPort
{
    ofxTuioCursor cursor;
    int port;
};

class PortTouchListener;

class ofxAppTuioManager
{
public:
    static ofxAppTuioManager& get()
    {
        static ofxAppTuioManager touchMgr = ofxAppTuioManager();
        return touchMgr;
    }
    ~ofxAppTuioManager() {}

    void setup();
    void setup( int port );
    void setup( std::vector<int> ports );
    void setup( std::vector<int> ports, std::map<int, glm::vec2> offsets, int screenWidth, int screenHeight);
    glm::vec2 getConvertedCoord( CursorAtPort& cursor );
    void update();
    
    inline const std::map<std::string, TouchAtPort>& getTouches()
    {
        return m_touches;
    }

    //
    inline void addCursorAtPort( CursorAtPort& e )
    {
        auto touch = TouchAtPort{getConvertedCoord( e ), e.port, e.cursor.getSessionId()};
        ofNotifyEvent( onAddTouchAtPort, touch );

        m_touches.emplace( getCursorId( e.port, e.cursor.getSessionId() ), touch );
    }

    inline void updateCursorAtPort( CursorAtPort& e )
    {
        auto touch = TouchAtPort{getConvertedCoord( e ), e.port, e.cursor.getSessionId() };
        ofNotifyEvent( onUpdateTouchAtPort, touch );

        m_touches[getCursorId( e.port, e.cursor.getSessionId() )] = touch;
    }

    inline void removeCursorAtPort( CursorAtPort& e )
    {
        auto touch = TouchAtPort{getConvertedCoord( e ), e.port, e.cursor.getSessionId() };
        ofNotifyEvent( onRemoveTouchAtPort, touch );

        m_touches.erase( getCursorId( e.port, e.cursor.getSessionId() ) );
    }

    inline std::string getCursorId( long port, int cursorId )
    {
        return std::to_string( port ) + "-" + std::to_string( cursorId );
    }


    ofEvent<TouchAtPort> onAddTouchAtPort;
    ofEvent<TouchAtPort> onUpdateTouchAtPort;
    ofEvent<TouchAtPort> onRemoveTouchAtPort;

    static bool SWAP_DIMS;
    static bool FLIP_X;
    static bool FLIP_Y;

    static int width;
    static int height;

    static std::vector<int> ports;
    static std::map<int, glm::vec2> offsets;

private:
    ofxAppTuioManager() {}

    std::map<int, PortTouchListener> m_listeners;
    std::map<std::string, TouchAtPort> m_touches;  // key = combined port and sessionId
};

class PortTouchListener
{
public:
    inline void setup( ofxAppTuioManager* managerPtr, int port )
    {
        m_managerPtr = managerPtr;
        m_port       = port;
        
        m_clientPtr = new ofxTuioClient();
        m_clientPtr->start(port);
        ofAddListener(m_clientPtr->cursorAdded, this, &PortTouchListener::cursorAdded);
        ofAddListener(m_clientPtr->cursorRemoved, this, &PortTouchListener::cursorRemoved);
        ofAddListener(m_clientPtr->cursorUpdated, this, &PortTouchListener::cursorUpdated);
        
        std::cout << "tuio port " << m_port << " touch listener connected? maybe" << std::endl;
    }
    
    inline void update(){
        m_clientPtr->getMessage();
    }

    inline void cursorAdded( ofxTuioCursor& cursor )
    {
        CursorAtPort e = CursorAtPort{cursor, m_port};
        m_managerPtr->addCursorAtPort( e );
    }

    inline void cursorUpdated( ofxTuioCursor& cursor )
    {
        CursorAtPort e = CursorAtPort{cursor, m_port};
        m_managerPtr->updateCursorAtPort( e );
    }

    inline void cursorRemoved( ofxTuioCursor& cursor )
    {
        CursorAtPort e = CursorAtPort{cursor, m_port};
        m_managerPtr->removeCursorAtPort( e );
    }

private:
    int m_port{0};

    //use to replace reciever
    ofxTuioClient* m_clientPtr;
    //replace
//    ofxTuioReceiver m_receiver;
    ofxAppTuioManager* m_managerPtr;
};

