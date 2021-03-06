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

#ifndef FALLTERGEIST_UI_MONTHCOUNTER_H
#define FALLTERGEIST_UI_MONTHCOUNTER_H

// C++ standard includes

// Falltergeist includes
#include "../UI/ImageList.h"

// Third party includes

namespace Falltergeist
{
namespace UI
{

class Image;

class MonthCounter : public ImageList
{
public:

    enum class Month
    {
        JANUARY = 0,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER
    };

    MonthCounter(const Point& pos = Point(0, 0));
    MonthCounter(Month month, const Point& pos = Point(0, 0));

    Month month() const;
    void setMonth(Month month);

private:
    Month _month;
};

}
}
#endif // FALLTERGEIST_UI_MONTHCOUNTER_H
