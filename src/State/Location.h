/*
 * Copyright 2012-2015 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FALLTERGEIST_Location_H
#define FALLTERGEIST_Location_H

// C++ standard includes
#include <memory>
#include <list>

// Falltergeist includes
#include "../State/State.h"
#include "../Input/Mouse.h"
#include "../UI/ImageButton.h"
#include "../Game/Object.h"
#include "../Game/Timer.h"

// Third party includes

namespace Falltergeist
{
namespace Game
{
    class Object;
}
namespace UI
{
    class Animation;
    class Image;
    class PlayerPanel;
    class Tile;
    class TileMap;
}
class Hexagon;
class HexagonGrid;
class LocationCamera;
class VMStackValue;
class VM;

namespace State
{
    
/**
 * Location state works whenever player is on some map.
 * This state holds pointers to all object on current map, hexagon grid and other map-specific information.
 * 
 * pushed: when player starts new game or loads new game
 * popped: when player closes the game, exits to main menu or loads the game
 */
class Location : public State
{
public:
    Location();
    ~Location() override;

    void setLocation(const std::string& name);

    void init();
    void think() override;
    void handle(Event::Event* event) override;
    void render() override;

    void handleByGameObjects(Event::Mouse* event);

    HexagonGrid* hexagonGrid();
    LocationCamera* camera();

    void setMVAR(unsigned int number, int value);
    int MVAR(unsigned int number);

    std::map<std::string, VMStackValue>* EVARS();

    static void moveObjectToHexagon(Game::Object* object, Hexagon* hexagon);
    void destroyObject(Game::Object* object);
    void centerCameraAtHexagon(Hexagon* hexagon);
    void centerCameraAtHexagon(int tileNum);
    void handleAction(Game::Object* object, Input::Mouse::Icon action);
    void toggleCursorMode();

    void displayMessage(const std::string& message);

    void addTimerEvent(Game::Object* obj, int delay, int fixedParam = 0);
    void removeTimerEvent(Game::Object* obj);
    void removeTimerEvent(Game::Object* obj, int fixedParam);

    void onBackgroundClick(Event::Mouse* event);
    void onObjectMouseEvent(Event::Mouse* event, Game::Object* object);
    void onObjectHover(Event::Mouse* event, Game::Object* object);
    void onKeyDown(Event::Keyboard* event) override;
    void onMouseUp(Event::Mouse* event);
    void onMouseDown(Event::Mouse* event);
    void onMouseMove(Event::Mouse* event);

    void onStateActivate(Event::State* event) override;
    void onStateDeactivate(Event::State* event) override;

    UI::PlayerPanel* playerPanel();

protected:
    struct TimerEvent
    {
        Game::Object* object;
        Game::GameTimer timer;
        int fixedParam;
    };
    
    static const int KEYBOARD_SCROLL_STEP;
    static const int DROPDOWN_DELAY;

    // Timers
    unsigned int _scrollTicks = 0;
    unsigned int _scriptsTicks = 0;
    unsigned int _actionCursorTicks = 0;
    unsigned int _mouseMoveTicks = 0;
    Game::Timer _actionCursorTimer;
    Game::Timer _ambientSfxTimer;
    // for VM opcode add_timer_event
    std::list<TimerEvent> _timerEvents;
    // TODO: move to Game::Location class?
    std::map<std::string, unsigned char> _ambientSfx;

    std::unique_ptr<HexagonGrid> _hexagonGrid;
    std::unique_ptr<LocationCamera> _camera;
    std::unique_ptr<UI::TileMap> _floor;
    std::unique_ptr<UI::TileMap> _roof ;
    std::unique_ptr<VM> _locationScript;
    std::vector<int> _MVARS;
    std::map<std::string, VMStackValue> _EVARS;
    std::vector<UI::Base*> _floatMessages;

    bool _locationEnter = true;
    unsigned int _currentElevation = 0;
    unsigned int _lastClickedTile = 0;
    Game::Object* _objectUnderCursor = NULL;
    Game::Object* _actionCursorLastObject = NULL;
    bool _actionCursorButtonPressed = false;
    UI::PlayerPanel* _playerPanel;

    bool _scrollLeft = false;
    bool _scrollRight = false;
    bool _scrollTop = false;
    bool _scrollBottom = false;

    std::vector<std::unique_ptr<Game::Object>> _objects;
    std::unique_ptr<UI::TextArea> _hexagonInfo;

    Event::MouseHandler _mouseDownHandler, _mouseUpHandler, _mouseMoveHandler;
    
    std::vector<Input::Mouse::Icon> getCursorIconsForObject(Game::Object* object);

};

}
}

#endif // FALLTERGEIST_Location_H
