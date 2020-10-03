#include "catch.hpp"
#include "../text.h"

#include "../pdftraverse.h"

#include <string>

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

using namespace std;


TEST_CASE ("test123"){
//    string document_path = "../pdfs/test_openoffice.pdf";
//    int page_number=0;

//    PDFParser parser;
//    InputFile pdfFile;

//    unique_ptr<TextData> textData( new TextData() );
//    SymbolLookup lookup;
//    lookup.m_text_data = std::move(textData);

//    EStatusCode OpenFileStatus = pdfFile.OpenFile(document_path);
//    REQUIRE( OpenFileStatus == eSuccess);

//    EStatusCode StartPDFParsingStatus = parser.StartPDFParsing(pdfFile.GetInputStream());
//    REQUIRE( StartPDFParsingStatus == eSuccess);

//    RefCountPtr<PDFDictionary> page(parser.ParsePage(page_number));

//    RefCountPtr<PDFObject> page_section(parser.QueryDictionaryObject(page.GetPtr(), "Resources"));




}

/// without PDFnode specialization it works only if PDFDictionary is heap allocated !!!??
TEST_CASE ("problematic heap allocation only - for dictionaries") {
    std::string aaa("AAA");           // type 1
    PDFLiteralString aaa_pdf(aaa);
    PDFName aaa_name("AAA");

    // NOTE: to test disable all the spacializations of PDFnode

    // With PDFStreamInput specialization - this works fine
    PDFDictionary dictionary;
    dictionary.Insert(&aaa_name, &aaa_pdf);
    auto pdf_stream = unique_ptr<PDFStreamInput> (new PDFStreamInput(&dictionary, 0));

    // with unique_ptr this fails too
    //auto dictionary = unique_ptr<PDFDictionary>(new PDFDictionary()); // needs heap allocation
    //dictionary->Insert(&aaa_name, &aaa_pdf);
    //auto pdf_stream = unique_ptr<PDFStreamInput> (new PDFStreamInput(dictionary.get(), 0));

    // This standar pointer works but requires explicit delete once we donw with pdf_stream
    // so there is a problem with a liftime of in the destructor of PDFStreamInput?
    //PDFDictionary * dictionary = new PDFDictionary(); // needs heap allocation
    //dictionary->Insert(&aaa_name, &aaa_pdf);
    //auto pdf_stream = unique_ptr<PDFStreamInput> (new PDFStreamInput(dictionary, 0));
    //delete dictionary;


    // remaing of the test
    PDFnode<PDFStreamInput> stream_node(pdf_stream.get());

    auto dictionary_node = stream_node.QueryStreamDictionary();
    auto pdfobject = dictionary_node->QueryDirectObject(aaa_pdf);

    REQUIRE ( ((PDFLiteralString*)pdfobject)->GetValue().compare("AAA") == 0);


}
