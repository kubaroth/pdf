#include "text.h"

int main() {

    // string path = "../pdfs/test_openoffice.pdf";   // openoffice
    // string path = "../pdfs/test_google_dics.pdf";  // google docs
   
    // parse_page("../dsohowto.pdf", 0);

    auto textDataPtr = parse_page("non_exist_pdf", 0);
    cout << "NULL: " << (textDataPtr == nullptr) <<endl;

    textDataPtr = parse_page("../pdfs/test_openoffice.pdf", 0);
    cout << "read result2:"<< textDataPtr->text <<endl;

    // textDataPtr = parse_page("../pdfs/test_google_docs.pdf", 0);
    // cout << "read result3:"<< textDataPtr->text <<endl;

    // textDataPtr = parse_page("../pdfs/test_inkspace.pdf", 0);
    // cout << "read result4:"<< textDataPtr->text <<endl;

    // textDataPtr = parse_page("../pdfs/ink_ff_fi_fl_fg.pdf", 0);
    // cout << "read result5:"<< textDataPtr->text <<endl;
    
    // textDataPtr = parse_page("../dsohowto.pdf", 0);
    // cout << "read result3:"<< textDataPtr->text <<endl;

    // textDataPtr = parse_page("../pdfs/abcdef.pdf", 0);
    // cout << "read result3:"<< textDataPtr->text <<endl;

    
    
    // cout << "testing test_openoffice.pdf:" << endl;
    // cout <<g_symLookup.map_bfchars['\001'] << endl; // 'B'
    // cout <<g_symLookup.map_bfchars['\002'] << endl; // ' '
    // cout <<g_symLookup.map_bfchars['\003'] << endl; // 'A'
    // for (auto &text : g_symLookup.text_data)
    //     cout << text;
    // cout << endl;
    
}
