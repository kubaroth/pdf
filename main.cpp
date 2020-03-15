#include "text.h"

int main() {

    // string path = "../pdfs/test_openoffice.pdf";   // openoffice
    // string path = "../pdfs/test_google_dics.pdf";  // google docs
   
    // parse_page("../pdfs/dsohowto.pdf", 0);
    // parse_page("../pdfs/test_openoffice.pdf", 0);
    parse_page("../pdfs/ff_fi_fl_fg.pdf", 0);
    
#ifdef LOG
    cout <<"!!!!"<<endl;
#endif
    cout << "g_sl.use_buffer_char: " << g_sl.use_buffer_char << " " << g_sl.bfchars.size() <<endl;
    cout << "g_sl.use_differences: " << g_sl.use_differences << " " << g_sl.differences_table.size() <<endl;
    if (g_sl.use_differences){
        for (auto i=0; i<g_sl.differences_table.size(); ++i) {
            cout << i << " -- " << g_sl.differences_table[i]  <<endl;
        }
    }
    if (g_sl.use_buffer_char) {
        g_sl.print_bfchars();
    }
}
