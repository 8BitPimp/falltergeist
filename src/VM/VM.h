/*
 * Copyright 2012-2014 Falltergeist Developers.
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

#ifndef FALLTERGEIST_VM_H
#define FALLTERGEIST_VM_H

// C++ standard includes
#include <string>

// Falltergeist includes
#include "../VM/VMStack.h"
#include "../VM/VMStackValue.h"

// Third party includes
#include <libfalltergeist/Int/File.h>
#include <libfalltergeist/Int/Procedure.h>

namespace Falltergeist
{
namespace Game
{
    class Object;
}

/*
 * VM class represents Virtual Machine for running vanilla Fallout scripts.
 * VM uses 2 stacks (return stack and data stack).
 * Each operator from .INT script is handled by one of the Handler classes and it manipulates one or both stacks in some way.
 * Typical scripting command takes 0 or more arguments from the data stack and puts one return value to the same stack.
 */
class VM
{
protected:
    Game::Object* _owner = nullptr;
    Game::Object* _sourceObject = nullptr;
    Game::Object* _targetObject = nullptr;
    int _fixedParam = 0;
    int _actionUsed = 0;
    libfalltergeist::Int::File* _script = 0;
    bool _initialized = false;
    bool _overrides = false;
    VMStack _dataStack;
    VMStack _returnStack;
    std::vector<VMStackValue> _LVARS;
    unsigned int _programCounter = 0;
    int _DVAR_base = 0;
    int _SVAR_base = 0;

public:
    VM(libfalltergeist::Int::File* script, Game::Object* owner);
    VM(const std::string& filename, Game::Object* owner);
    virtual ~VM();
    void run();
    void initialize();

    bool initialized();
    bool overrides();
    void setOverrides(bool Value);
    void setInitialized(bool value);
    std::string msgMessage(int msg_file_num, int msg_num);
    /*
     * Returns filename of an .int script file 
     */
    std::string filename();
    bool hasFunction(const std::string& name);

    void call(const std::string& name);
    libfalltergeist::Int::File* script();

    Game::Object* owner();

    unsigned int programCounter();
    void setProgramCounter(unsigned int value);

    VMStack* dataStack();
    VMStack* returnStack();
    std::vector<VMStackValue>* LVARS();

    int DVARbase();
    void setDVARBase(int Value);
    int SVARbase();
    void setSVARbase(int Value);
    
    VM* setFixedParam(int _fixedParam);
    int fixedParam() const;
    VM* setTargetObject(Game::Object* _targetObject);
    Game::Object* targetObject() const;
    VM* setSourceObject(Game::Object* _sourceObject);
    Game::Object* sourceObject() const;
    VM* setActionUsed(int _actionUsed);
    int actionUsed() const;

};

}
#endif // FALLTERGEIST_VM_H
