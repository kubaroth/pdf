#include "text.h"

int main() {

    // string path = "../pdfs/dsohowto.pdf";
    // string path = "../pdfs/test_fu.pdf";   // openoffice
    // string path = "../pdfs/test_fu_aaa.pdf";  // google docs
   
    // parse_page("../pdfs/dsohowto.pdf", 0);
    parse_page("../pdfs/test_fu.pdf", 0);
    // TODO: ff fi fl fg

    cout <<"!!!!"<<endl;
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
