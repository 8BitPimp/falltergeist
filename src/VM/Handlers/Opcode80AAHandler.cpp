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

// C++ standard includes

// Falltergeist includes
#include "../../Logger.h"
#include "../../VM/Handlers/Opcode80AAHandler.h"
#include "../../Game/CritterObject.h"
#include "../../VM/VM.h"

// Third party includes

namespace Falltergeist
{

Opcode80AAHandler::Opcode80AAHandler(VM* vm) : OpcodeHandler(vm)
{
}

void Opcode80AAHandler::_run()
{
    auto &debug = Logger::debug("SCRIPT");
    debug << "[80AA] [+] int get_skill_value(GameCritterObject* who, int skill) " << std::endl;
    int skill = _vm->dataStack()->popInteger();
    debug << "    skill = " << skill << std::endl;
    if (skill > 17 || skill < 0)
    {
        _error("get_skill_value - skill out of range: " + std::to_string(skill));
    }
    auto object = _vm->dataStack()->popObject();
    int value = 0;
    if (auto critter = dynamic_cast<Game::CritterObject*>(object))
    {
        value = critter->skillValue((SKILL)skill);
        _vm->dataStack()->push(value);
    }
    else 
    {
        _error("get_skill_value(who, skill): who is not critter");
    }
}

}


