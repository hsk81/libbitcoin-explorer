/*
 * Copyright (c) 2011-2014 sx developers (see AUTHORS)
 *
 * This file is part of sx.
 *
 * sx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "precompile.hpp"
#include "command.hpp"

SX_USING_NAMESPACES()

BOOST_AUTO_TEST_SUITE(wrap_decode__invoke)

// vectors
#define WRAP_DECODE_WRAPPED_DATA "2a031bab84e687e36514eeaf5a017c30d32c1f59dd4ea6629da7970ca374513dd006298eebe4"

// expectations
#define WRAP_DECODE_NATIVE "wrapper\n{\n    version 42\n    payload 031bab84e687e36514eeaf5a017c30d32c1f59dd4ea6629da7970ca374513dd006\n    checksum 3840642601\n}\n"
#define WRAP_DECODE_JSON "{\n    \"wrapper\": {\n        \"version\": \"42\",\n        \"payload\": \"031bab84e687e36514eeaf5a017c30d32c1f59dd4ea6629da7970ca374513dd006\",\n        \"checksum\": \"3840642601\"\n    }\n}\n"
#define WRAP_DECODE_XML "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<wrapper><version>42</version><payload>031bab84e687e36514eeaf5a017c30d32c1f59dd4ea6629da7970ca374513dd006</payload><checksum>3840642601</checksum></wrapper>"

BOOST_AUTO_TEST_CASE(wrap_decode__invoke__native__okay_output)
{
    SX_DECLARE_COMMAND(wrap_decode);
    command.set_format_option({ encoding_engine::native });
    command.set_wrapped_argument({ WRAP_DECODE_WRAPPED_DATA });
    SX_REQUIRE_OKAY(command.invoke(output, error));
    SX_REQUIRE_OUTPUT(WRAP_DECODE_NATIVE);
}

BOOST_AUTO_TEST_CASE(wrap_decode__invoke__info__okay_output)
{
    SX_DECLARE_COMMAND(wrap_decode);
    command.set_format_option({ encoding_engine::info });
    command.set_wrapped_argument({ WRAP_DECODE_WRAPPED_DATA });
    SX_REQUIRE_OKAY(command.invoke(output, error));
    SX_REQUIRE_OUTPUT(WRAP_DECODE_NATIVE);
}

BOOST_AUTO_TEST_CASE(wrap_decode__invoke__json__okay_output)
{
    SX_DECLARE_COMMAND(wrap_decode);
    command.set_format_option({ encoding_engine::json });
    command.set_wrapped_argument({ WRAP_DECODE_WRAPPED_DATA });
    SX_REQUIRE_OKAY(command.invoke(output, error));
    SX_REQUIRE_OUTPUT(WRAP_DECODE_JSON);
}

BOOST_AUTO_TEST_CASE(wrap_decode__invoke__xml__okay_output)
{
    SX_DECLARE_COMMAND(wrap_decode);
    command.set_format_option({ encoding_engine::xml });
    command.set_wrapped_argument({ WRAP_DECODE_WRAPPED_DATA });
    SX_REQUIRE_OKAY(command.invoke(output, error));
    SX_REQUIRE_OUTPUT(WRAP_DECODE_XML);
}

BOOST_AUTO_TEST_SUITE_END()