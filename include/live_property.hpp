#pragma once

/*
MIT License
Copyright (c) 2017 Arlen Keshabyan (arlen.albert@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <string>
#include <string_view>
#include "signalslot.hpp"

namespace nstd
{
template<typename T>
class live_property
{
public:
    using value_type = T;

    struct value_changing_context
    {
        const live_property &property;
        const value_type &new_value;
        bool cancel = false;
    };

    live_property(const std::string &name, const value_type &value = value_type()) : _name{ name }, _value{ value } {}

    live_property(const live_property &other)
    {
        operator= (other._value);
    }

    live_property(live_property &&other)
    {
        operator=(std::forward<live_property>(other));
    }

    live_property &operator=(value_type &&value)
    {
        return move_value(std::forward<value_type>(value));
    }

    live_property &operator=(live_property &&other)
    {
        return operator=(std::move(other._value));
    }

    live_property &operator=(const live_property &other)
    {
        return operator= (other._value);
    }

    live_property &operator=(const value_type &value)
    {
        return assign_value(value);
    }

    std::string_view name() const
    {
        return _name;
    }

    const value_type &value() const
    {
        return _value;
    }

    operator value_type() const
    {
        return _value;
    }

    live_property &operator +=(const value_type &value)
    {
        return move_value(_value + value);
    }

    live_property &operator +=(const live_property &other)
    {
        return operator += (other._value);
    }

    live_property &operator -=(const value_type &value)
    {
        return move_value(_value - value);
    }

    live_property &operator -=(const live_property &other)
    {
        return operator -= (other._value);
    }

    live_property &operator *=(const value_type &value)
    {
        return move_value(_value * value);
    }

    live_property &operator *=(const live_property &other)
    {
        return operator *= (other._value);
    }

    live_property &operator /=(const value_type &value)
    {
        return move_value(_value / value);
    }

    live_property &operator /=(const live_property &other)
    {
        return operator /= (other._value);
    }

    live_property &operator >>=(const value_type &value)
    {
        return move_value(_value >> value);
    }

    live_property &operator >>=(const live_property &other)
    {
        return operator >>= (other._value);
    }

    live_property &operator <<=(const value_type &value)
    {
        return move_value(_value << value);
    }

    live_property &operator <<=(const live_property &other)
    {
        return operator <<= (other._value);
    }

    live_property &operator &=(const value_type &value)
    {
        return move_value(_value & value);
    }

    live_property &operator &=(const live_property &other)
    {
        return operator &= (other._value);
    }

    live_property &operator |=(const value_type &value)
    {
        return move_value(_value | value);
    }

    live_property &operator |=(const live_property &other)
    {
        return operator |= (other._value);
    }

    live_property &operator ^=(const value_type &value)
    {
        return move_value(_value ^ value);
    }

    live_property &operator ^=(const live_property &other)
    {
        return operator ^= (other._value);
    }

    live_property &operator %=(const value_type &value)
    {
        return move_value(_value % value);
    }

    live_property &operator %=(const live_property &other)
    {
        return operator %= (other._value);
    }

    live_property &operator ++()
    {
        auto value { _value };

        if (emit_changing(++value))
        {
            ++_value;

            emit_changed();
        }

        return *this;
    }

    live_property operator ++(int)
    {
        live_property return_value{ _name, _value };

        return operator ++(), return_value;
    }

    live_property &operator --()
    {
        auto value { _value };

        if (emit_changing(--value))
        {
            --_value;

            emit_changed();
        }

        return *this;
    }

    live_property operator --(int)
    {
        live_property return_value{ _name, _value };

        return operator --(), return_value;
    }

    nstd::signalslot::signal<value_changing_context&> value_changing;
    nstd::signalslot::signal<const live_property&> value_changed;

private:
    live_property &assign_value(const value_type &value)
    {
        if (emit_changing(value))
        {
            _value = value;

            emit_changed();
        }

        return *this;
    }

    live_property &move_value(value_type &&value)
    {
        if (emit_changing(value))
        {
            _value = std::forward<value_type>(value);

            emit_changed();
        }

        return *this;
    }

    bool emit_changing(const value_type &value)
    {
        value_changing_context context{ *this, value };

        value_changing.emit(context);

        return !context.cancel;
    }

    void emit_changed()
    {
        value_changed.emit(*this);
    }

    std::string _name;
    value_type _value;
};
}
