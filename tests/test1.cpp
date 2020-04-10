#include "catch.hpp"
#include "../text.h"

TEST_CASE ("Basic check"){
    REQUIRE (1 == 1);
}

TEST_CASE ("Read PDF"){

    parse_page("../pdfs/test_openoffice.pdf", 0);

    cout << "g_symLookup.use_buffer_char: " << g_symLookup.use_buffer_char << " " << g_symLookup.bfchars.size() <<endl;
    cout << "g_symLookup.use_differences: " << g_symLookup.use_differences << " " << g_symLookup.differences_table.size() <<endl;

    cout << "\n\nOutput buffer_char:\n " <<endl;
    if (g_symLookup.use_differences){
        for (auto i=0; i<g_symLookup.differences_table.size(); ++i) {
            cout << i << " -- " << g_symLookup.differences_table[i]  <<endl;
        }
    }
    if (g_symLookup.use_buffer_char) {
        g_symLookup.print_bfchars();
    }

    REQUIRE( g_symLookup.bfchars.size() == 24 );

}
