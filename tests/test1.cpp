#include "catch.hpp"
#include "../text.h"

TEST_CASE ("Basic check"){
    REQUIRE (1 == 1);
}

// TEST_CASE ("Read PDF"){

//     parse_page("../pdfs/test_openoffice.pdf", 0);

//     cout << "g_symLookup.use_buffer_char: " << g_symLookup.use_buffer_char << " " << g_symLookup.bfchars.size() <<endl;
//     cout << "g_symLookup.use_differences: " << g_symLookup.use_differences << " " << g_symLookup.differences_table.size() <<endl;

//     cout << "\n\nOutput buffer_char:\n " <<endl;
//     if (g_symLookup.use_differences){
//         for (auto i=0; i<g_symLookup.differences_table.size(); ++i) {
//             cout << i << " -- " << g_symLookup.differences_table[i]  <<endl;
//         }
//     }
//     if (g_symLookup.use_buffer_char) {
//         g_symLookup.print_bfchars();
//     }

//     REQUIRE( g_symLookup.bfchars.size() == 24 );

// }

// TEST_CASE ("test_openoffice.pdf"){
//     auto textDataPtr = parse_page("../pdfs/test_openoffice.pdf", 0);
//     REQUIRE( textDataPtr->text.compare("BBB AAA") == 0 );

//     // TODO: Lookup is now internal
//     // REQUIRE( lookup.map_bfchars['\001'] == 'B' );
//     // REQUIRE( lookup.map_bfchars['\002'] == ' ' );
//     // REQUIRE( lookup.map_bfchars['\003'] == 'A' );
// }

// TEST_CASE ("ff_fi_fl_fg.pdf"){
//     auto textDataPtr = parse_page("../pdfs/ff_fi_fl_fg.pdf", 0);
//     cout << textDataPtr->text <<endl;
//     REQUIRE( textDataPtr->text.compare("ff fi fl fg") == 0 );
// }

// TEST_CASE ("google_docs"){
//     auto textDataPtr = parse_page("../pdfs/test_google_docs.pdf", 0);
//     // cout << textDataPtr.get() << endl;
//     cout << textDataPtr->text <<endl;
//     // REQUIRE( textDataPtr->text.compare("ff fi fl fg") == 0 );
// }



TEST_CASE ("inkspace"){
    auto textDataPtr = parse_page("../pdfs/test_inkspace.pdf", 0);
    cout << "text" << textDataPtr->text <<endl;
    // REQUIRE( textDataPtr->text.compare("ff fi fl fg") == 0 );
}

/*


  -rw-r--r-- 1 pi pi 6892 Apr 11 15:58 test_2pages.pdf
  -rw-r--r-- 1 pi pi 7461 Apr 11 15:58 test_file.pdf
  -rw-r--r-- 1 pi pi 7694 Apr 11 15:58 test_inkspace.pdf

*/
