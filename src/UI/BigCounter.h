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

#ifndef FALLTERGEIST_UI_BIGCOUNTER_H
#define FALLTERGEIST_UI_BIGCOUNTER_H

// C++ standard includes
#include <memory>

// Falltergeist includes
#include "../UI/Base.h"

// Third party includes

namespace Falltergeist
{
namespace UI
{

class Image;

class BigCounter : public Falltergeist::UI::Base
{
public:
    enum class Color
    {
        WHITE = 1,
        RED
    };

    BigCounter(int x, int y, unsigned int length = 2);
    BigCounter(const Point& pos = Point(), unsigned int length = 2);
    ~BigCounter() override;

    Graphics::Texture* texture() const override;

    void setColor(Color color);
    Color color();

    void setNumber(unsigned int number);
    unsigned int number();

protected:
    Color _color = Color::WHITE;
    unsigned int _number = 0;
    unsigned int _length = 2;
    mutable std::unique_ptr<Graphics::Texture> _textureOnDemand;

    void setTexture(Graphics::Texture* texture) override; // We should override this method to prevent changing old _texture field.

private:
    using Falltergeist::UI::Base::_texture; // Hide unused field from childs.

    BigCounter(const BigCounter&) = delete;
    void operator=(const BigCounter&) = delete;
};

}
}
#endif // FALLTERGEIST_UI_BIGCOUNTER_H
