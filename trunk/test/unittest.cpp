// Copyright (c) 2009 Dmitry Goncharov
//
// Distributed under the BSD License.
// (See accompanying file COPYING).

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

void init_test_signalfd();
void init_test_signal_handler();

namespace {

bool init_unit_test()
{
    init_test_signalfd();
    init_test_signal_handler();
    return true;
}

} // namespace

int main(int argc, char* argv[])
{
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

