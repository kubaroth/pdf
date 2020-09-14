#include "catch.hpp"

#include "../text.h"
#include "../pdftraverse.h"

// Import of PDF objects in the order they appear in the enum
#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFBoolean.h>
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFNull.h>
#include <PDFWriter/PDFName.h>
#include <PDFWriter/PDFInteger.h>
#include <PDFWriter/PDFReal.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFStreamInput.h>
#include <PDFWriter/PDFSymbol.h>

// TODO: all the casses will primarilly deal with passing pointers into corresponding decorators

TEST_CASE ("traverse_template_PDFBooleanNode_ptr"){

}

TEST_CASE ("traverse_template_PDFLiteralStringNode"){
    std::string aaa("AAA");
    PDFLiteralString aaa_pdf(aaa);
    PDFnode<PDFLiteralString> aaa_node(std::move(aaa_pdf));
    REQUIRE( (aaa_node.GetType() == PDFObject::ePDFObjectLiteralString) == true );
}

TEST_CASE ("traverse_template_PDFLiteralStringNode_ptr"){
    std::string aaa("AAA");
    PDFLiteralString aaa_pdf(aaa);
    PDFnode<PDFLiteralString*> aaa_node(std::move(&aaa_pdf));
    REQUIRE( (aaa_node.GetType() == PDFObject::ePDFObjectLiteralString) == true );
}


TEST_CASE ("traverse_template_PDFHexStringNode_ptr"){
    std::string aaa("FF");
    PDFHexString aaa_pdf(aaa);
    PDFnode<PDFHexString*> aaa_node(&aaa_pdf);
    REQUIRE( (aaa_node.GetType() == PDFObject::ePDFObjectHexString) == true );
}

TEST_CASE ("traverse_template_PDFNullNode_ptr"){
    PDFNull aaa_pdf(void);
//    auto aaa_ptr = &aaa_pdf;
//    PDFnode<PDFNull*> aaa_node(void);
//    REQUIRE( (aaa_pdf.GetType() == PDFObject::ePDFObjectNull) == true );
}

TEST_CASE ("traverse_template_PDFNameNode_ptr"){
    std::string aaa("SomeName");
    PDFName aaa_pdf(aaa);
    PDFnode<PDFName*> aaa_node(&aaa_pdf);
    REQUIRE( (aaa_node.GetType() == PDFObject::ePDFObjectName) == true );
}

// type 5
TEST_CASE ("traverse_template_PDFIntegerNode_ptr"){
    PDFInteger aaa_pdf(3);
    PDFnode<PDFInteger*> aaa_node(&aaa_pdf);
    REQUIRE( (aaa_node.GetType() == PDFObject::ePDFObjectInteger) == true );
}

// type 6
TEST_CASE ("traverse_template_PDFRealNode_ptr"){
    PDFReal aaa_pdf(2.0);
    PDFnode<PDFReal*> aaa_node(&aaa_pdf);
    REQUIRE( (aaa_node.GetType() == PDFObject::ePDFObjectReal) == true );
}

TEST_CASE ("traverse_template_PDFArrayNode_ptr"){
    std::string aaa("AAA");           // type 1
    PDFLiteralString aaa_pdf(aaa);
    std::string bbb("FF");            // type 2
    PDFHexString bbb_pdf(aaa);
    PDFReal ccc_pdf(2.0);             // type 6

    PDFArray array;
    array.AppendObject(&aaa_pdf);
    array.AppendObject(&bbb_pdf);
    array.AppendObject(&ccc_pdf);

    PDFnode<PDFArray*> aaa_node(&array);
    REQUIRE( aaa_node.GetLength() == 3);
    REQUIRE( (aaa_node.QueryObject(0)->GetType() == PDFObject::ePDFObjectLiteralString) == true );
    REQUIRE( (aaa_node.QueryObject(1)->GetType() == PDFObject::ePDFObjectHexString) == true );
    REQUIRE( (aaa_node.QueryObject(2)->GetType() == PDFObject::ePDFObjectReal) == true );

}

TEST_CASE ("traverse_template_PDFDictionarygNode_ptr"){

}

TEST_CASE ("traverse_template_PDFIndirectObjectReferencegNode_ptr"){

}

TEST_CASE ("traverse_template_PDFStreamInputNode_ptr"){

}

TEST_CASE ("traverse_template_PDFSymbolNode_ptr"){

}

