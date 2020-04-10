#include "text.h"

int main() {

    // string path = "../pdfs/test_openoffice.pdf";   // openoffice
    // string path = "../pdfs/test_google_dics.pdf";  // google docs
   
    // parse_page("../pdfs/dsohowto.pdf", 0);
    // parse_page("../pdfs/test_openoffice.pdf", 0);
    parse_page("../pdfs/ff_fi_fl_fg.pdf", 0);
    
    cout << "g_symLookup.use_buffer_char: " << g_symLookup.use_buffer_char << " " << g_symLookup.bfchars.size() <<endl;
    cout << "g_symLookup.use_differences: " << g_symLookup.use_differences << " " << g_symLookup.differences_table.size() <<endl;
    if (g_symLookup.use_differences){
        for (auto i=0; i<g_symLookup.differences_table.size(); ++i) {
            cout << i << " -- " << g_symLookup.differences_table[i]  <<endl;
        }
    }
    if (g_symLookup.use_buffer_char) {
        g_symLookup.print_bfchars();
    }
}
