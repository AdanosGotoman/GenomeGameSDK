/*
Copyright(c) 2016-2021 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ===================
#include <type_traits>
#include <memory>
#include "Spartan_Definitions.h"
//==============================

namespace Genome
{
    class Context;

    class GENOME_CLASS ISubsystem : public std::enable_shared_from_this<ISubsystem>
    {        
    public:
        ISubsystem(Context* context) { m_context = context; }
        virtual ~ISubsystem() = default;
        virtual bool Initialize() { return true; }
        virtual void Tick(float delta_time) {}

        template <typename T>
        std::shared_ptr<T> GetPtrShared() { return std::dynamic_pointer_cast<T>(shared_from_this()); }

    protected:
        Context* m_context;
    };

    template<typename T>
    constexpr void validate_subsystem_type() { static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubystem"); }
}
