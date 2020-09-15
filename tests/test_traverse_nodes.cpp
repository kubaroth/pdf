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

    PDFnode<PDFArray*> array_node(&array);
    REQUIRE( array_node.GetLength() == 3);
    REQUIRE( (array_node.QueryObject(0)->GetType() == PDFObject::ePDFObjectLiteralString) == true );
    REQUIRE( (array_node.QueryObject(1)->GetType() == PDFObject::ePDFObjectHexString) == true );
    REQUIRE( (array_node.QueryObject(2)->GetType() == PDFObject::ePDFObjectReal) == true );

    // check for the size of the input array?
    REQUIRE( array.GetLength() == 3 );      // still 3
    // the object is still valid
    REQUIRE ( (array_node.QueryObject(2)->GetType() == PDFObject::ePDFObjectReal) == true);
}

TEST_CASE ("traverse_template_PDFDictionarygNode_ptr"){
    std::string aaa("AAA");           // type 1
    PDFLiteralString aaa_pdf(aaa);
    PDFName aaa_name("AAA");

    std::string bbb("FF");            // type 2
    PDFHexString bbb_pdf(aaa);
    PDFName bbb_name("FF");

    PDFReal ccc_pdf(2.0);             // type 6
    PDFName ccc_name("2");

    PDFDictionary * dictionary = new PDFDictionary(); // needs heap allocation ???
    dictionary->Insert(&aaa_name, &aaa_pdf);
    dictionary->Insert(&bbb_name, &bbb_pdf);
    dictionary->Insert(&ccc_name, &ccc_pdf);

    PDFnode<PDFDictionary*> dictionary_node(dictionary);

    auto aaa_result = (PDFLiteralString*) dictionary_node.QueryDirectObject(aaa_name);
    REQUIRE( (aaa_result->GetType() == PDFObject::ePDFObjectLiteralString) == true );
    auto bbb_result = (PDFHexString*) dictionary_node.QueryDirectObject(bbb_name);
    REQUIRE( (bbb_result->GetType() == PDFObject::ePDFObjectHexString) == true );
    auto ccc_result = (PDFName*) dictionary_node.QueryDirectObject(ccc_name);
    REQUIRE( (ccc_result->GetType() == PDFObject::ePDFObjectReal) == true );
}

TEST_CASE ("traverse_template_PDFIndirectObjectReferencegNode_ptr"){

    PDFIndirectObjectReference indirectRef(2,3);
    PDFnode<PDFIndirectObjectReference*> indirectRef_node(&indirectRef);

    REQUIRE( indirectRef_node.mObjectID == 2);
    REQUIRE( indirectRef_node.mVersion == 3);
}

TEST_CASE ("traverse_template_PDFStreamInputNode_ptr"){
    std::string aaa("AAA");           // type 1
    PDFLiteralString aaa_pdf(aaa);
    PDFName aaa_name("AAA");

    PDFDictionary * dictionary = new PDFDictionary(); // needs heap allocation
    dictionary->Insert(&aaa_name, &aaa_pdf);

    PDFStreamInput * pdf_stream = new PDFStreamInput(dictionary, 1);

    PDFnode<PDFStreamInput*> stream_node(pdf_stream);

    auto dictionary_node = stream_node.QueryStreamDictionary();
    auto pdfobject = dictionary_node->QueryDirectObject(aaa_pdf);

    REQUIRE( ((PDFLiteralString*)pdfobject)->GetValue().compare("AAA") == 0);

}

TEST_CASE ("traverse_template_PDFSymbolNode_ptr"){

}


TEST_CASE ("test visitor"){
    std::string aaa("AAA");
    PDFLiteralString aaa_pdf(aaa);
    PDFnode<PDFLiteralString> aaa_node(std::move(aaa_pdf));

    TestVistor tv;
    aaa_node.accept(tv);


}
