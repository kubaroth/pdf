#include "text.h"

int main() {

    // string path = "../pdfs/test_openoffice.pdf";   // openoffice
    // string path = "../pdfs/test_google_dics.pdf";  // google docs
   
    // parse_page("../dsohowto.pdf", 0);

    auto textDataPtr = parse_page("non_exist_pdf", 0);
    cout << "NULL: " << (textDataPtr == nullptr) <<endl;

    // textDataPtr = parse_page("../pdfs/test_openoffice.pdf", 0);
    // cout << "read result2:"<< textDataPtr->text <<endl;

    textDataPtr = parse_page("../pdfs/test_google_docs.pdf", 0);
    
    // parse_page("../pdfs/ff_fi_fl_fg.pdf", 0);
    
    cout << "g_symLookup.use_buffer_char: " << g_symLookup.use_buffer_char << " " << g_symLookup.bfchars.size() <<endl;
    cout << "g_symLookup.use_differences: " << g_symLookup.use_differences << " " << g_symLookup.differences_table.size() <<endl;
    if (g_symLookup.use_differences){
        for (auto i=0; i<g_symLookup.differences_table.size(); ++i) {
            cout << i << " -- " << g_symLookup.differences_table[i]  <<endl;
        }
    }
    // Option 2 
    if (g_symLookup.use_buffer_char) {
        g_symLookup.print_bfchars();
    }
    
    // cout << "testing test_openoffice.pdf:" << endl;
    // cout <<g_symLookup.map_bfchars['\001'] << endl; // 'B'
    // cout <<g_symLookup.map_bfchars['\002'] << endl; // ' '
    // cout <<g_symLookup.map_bfchars['\003'] << endl; // 'A'
    // for (auto &text : g_symLookup.text_data)
    //     cout << text;
    // cout << endl;
    
}
