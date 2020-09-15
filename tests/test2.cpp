#include "catch.hpp"
#include "../text.h"

#include <string>

using namespace std;


TEST_CASE ("test123"){
    string document_path = "../pdfs/test_openoffice.pdf";
    int page_number=0;

    PDFParser parser;
    InputFile pdfFile;

    unique_ptr<TextData> textData( new TextData() );
    SymbolLookup lookup;
    lookup.m_text_data = std::move(textData);

    EStatusCode OpenFileStatus = pdfFile.OpenFile(document_path);
    REQUIRE( OpenFileStatus == eSuccess);

    EStatusCode StartPDFParsingStatus = parser.StartPDFParsing(pdfFile.GetInputStream());
    REQUIRE( StartPDFParsingStatus == eSuccess);

    RefCountPtr<PDFDictionary> page(parser.ParsePage(page_number));

    RefCountPtr<PDFObject> page_section(parser.QueryDictionaryObject(page.GetPtr(), "Resources"));

}
