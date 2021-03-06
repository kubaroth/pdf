#include "text.h"

int main() {

    // string path = "../pdfs/test_openoffice.pdf";   // openoffice
    // string path = "../pdfs/test_google_dics.pdf";  // google docs
   
    // parse_page("../dsohowto.pdf", 0);

    // auto textDataPtr = parse_page("non_exist_pdf", 0);
    // cout << "NULL: " << (textDataPtr == nullptr) <<endl;

    // Below tests are primarily for debugging purposes.
    
    auto textDataPtr2 = parse_page("../pdfs/test_openoffice.pdf", 0);
    cout << "read result2:"<< textDataPtr2->text <<endl;

    // textDataPtr = parse_page("../pdfs/test_google_docs.pdf", 0);
    // cout << "read result3:"<< textDataPtr->text <<endl;

    // textDataPtr = parse_page("../pdfs/test_inkspace.pdf", 0);
    // cout << "read result4:"<< textDataPtr->text <<endl;

    // textDataPtr = parse_page("../pdfs/ink_ff_fi_fl_fg.pdf", 0);
    // cout << "read result5:"<< textDataPtr->text <<endl;

    // textDataPtr = parse_page("../pdfs/test_2pages.pdf", 1); // page 2
    // cout << "text: " << textDataPtr->text <<endl;
   
    // textDataPtr = parse_page("../dsohowto.pdf", 1);  // test page 2
    // cout << "read result6:"<< textDataPtr->text <<endl;
    
}
