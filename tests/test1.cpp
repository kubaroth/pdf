#include "catch.hpp"
#include "../text.h"

TEST_CASE ("test_openoffice.pdf"){
    auto textDataPtr = parse_page("../pdfs/test_openoffice.pdf", 0);
    REQUIRE( textDataPtr->text.compare("BBB AAA") == 0 );
}

TEST_CASE ("ff_fi_fl_fg.pdf"){
    auto textDataPtr = parse_page("../pdfs/ff_fi_fl_fg.pdf", 0);
    cout << textDataPtr->text <<endl;
    REQUIRE( textDataPtr->text.compare("ff fi fl fg") == 0 );
}

TEST_CASE ("google_docs"){
    auto textDataPtr = parse_page("../pdfs/test_google_docs.pdf", 0);
    cout << textDataPtr->text << endl;
    REQUIRE( textDataPtr->text.compare("AAA BBB ") == 0 );
}

TEST_CASE ("inkspace"){
    auto textDataPtr = parse_page("../pdfs/test_inkspace.pdf", 0);
    cout << "text: " << textDataPtr->text <<endl;
    REQUIRE( textDataPtr->text.compare("AAA\nfile") == 0 );
}

TEST_CASE ("2pages"){
    auto textDataPtr = parse_page("../pdfs/test_2pages.pdf", 1);
    cout << "text: " << textDataPtr->text <<endl;
    REQUIRE( textDataPtr->text.compare("BBB") == 0 );
}

TEST_CASE ("file"){
    auto textDataPtr = parse_page("../pdfs/test_file.pdf", 0);
    cout << "text: " << textDataPtr->text <<endl;
    REQUIRE( textDataPtr->text.compare("\nfile") == 0 );
}
