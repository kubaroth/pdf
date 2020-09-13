#include "catch.hpp"

#include "../text.h"
#include "../pdftraverse.h"

TEST_CASE ("traverse_template_PDFLiteralStringNode"){
    std::string aaa("AAA");
    PDFLiteralString aaa_pdf(aaa);
    PDFnode<PDFLiteralString> aaa_node(aaa_pdf);
    REQUIRE( (aaa_node.GetType() == PDFObject::ePDFObjectLiteralString) == 1 );
}

TEST_CASE ("traverse_PDFLiteralStringNode"){
    PDFLiteralStringNode bbb("BBB");
    REQUIRE( (bbb.GetType() == PDFObject::ePDFObjectLiteralString) == 1 );
}

