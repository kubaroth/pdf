#include "catch.hpp"
#include "../text.h"

TEST_CASE ("Basic check"){
    REQUIRE (1 == 1);
}

TEST_CASE ("Read PDF"){

    parse_page("../pdfs/test_fu.pdf", 0);

    cout << "g_sl.use_buffer_char: " << g_sl.use_buffer_char << " " << g_sl.bfchars.size() <<endl;
    cout << "g_sl.use_differences: " << g_sl.use_differences << " " << g_sl.differences_table.size() <<endl;

    cout << "\n\nOutput buffer_char:\n " <<endl;
    if (g_sl.use_differences){
        for (auto i=0; i<g_sl.differences_table.size(); ++i) {
            cout << i << " -- " << g_sl.differences_table[i]  <<endl;
        }
    }
    if (g_sl.use_buffer_char) {
        g_sl.print_bfchars();
    }

    REQUIRE( g_sl.bfchars.size() == 24 );

}
