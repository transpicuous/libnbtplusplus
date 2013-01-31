/*
    libnbt++ - A library for the Minecraft Named Binary Tag format.
    Copyright (C) 2013 ljfa-ag

    This file is part of libnbt++.

    libnbt++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libnbt++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libnbt++.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tag_byte_array.h"
#include "tag_int_array.h"
#include "endian_str.h"

namespace nbt
{

void tag_byte_array::write_payload(std::ostream& os) const
{
    endian::write_big(os, size());
    os.write(reinterpret_cast<const char*>(data.data()), size());
}

void tag_int_array::write_payload(std::ostream& os) const
{
    endian::write_big(os, size());
    for(int32_t i: data)
        endian::write_big(os, i);
}

}
