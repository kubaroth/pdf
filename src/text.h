# pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <memory>
#include <queue>
#include <cmath>        // std::abs

// Pdf - standard
#include <PDFWriter/PDFWriter.h>
#include <PDFWriter/PDFPage.h>
#include <PDFWriter/PageContentContext.h>
#include <PDFWriter/PDFFormXObject.h>
#include <PDFWriter/ResourcesDictionary.h>

// pdfwriter - Traversal
#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/RefCountPtr.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFObjectCast.h>
#include <PDFWriter/PDFInteger.h>
#include <PDFWriter/PDFReal.h>
#include <PDFWriter/PDFName.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFStreamInput.h>
#include <PDFWriter/IByteReader.h>
#include <PDFWriter/EStatusCode.h>
#include <PDFWriter/PDFObject.h>  // ePDFObjectArray

#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/IDocumentContextExtender.h>  // dictionary
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFBoolean.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFNull.h>
#include <PDFWriter/PDFSymbol.h>
#include <PDFWriter/PDFStreamInput.h>

#include "textdata.h"
#include "pdftraverse.h"

using namespace std;
using namespace PDFHummus;

// 0: no output
// 1: main messages
// 2: useful debug
// 3: all (currently disabled, as these execution paths are not used)
#define LOG 0

inline unique_ptr<TextData> parse_page(string document_path, int page_number){

    cout << "Extracting text from: " <<  document_path << " page: "<< page_number << endl;

    PDFParser * parser = new PDFParser();
    InputFile pdfFile;

    EStatusCode status = pdfFile.OpenFile(document_path);
    if(status != eSuccess) {
        cout << "No Document Found... " << document_path << endl;
        return nullptr;
    }

    status = parser->StartPDFParsing(pdfFile.GetInputStream());
    if(status != eSuccess) {
        return nullptr;
    }

    ResourcesVisitor * vt = new ResourcesVisitor(*parser);

    struct Impl {
        /// Print top level keys of the page
        static void  topLevelKeys(RefCountPtr<PDFDictionary> page, RefCountPtr<PDFObject> page_section) {
            auto it = page.GetPtr()->GetIterator();
            while(it.MoveNext()) {
                PDFName* name = it.GetKey();
                PDFObject* obj = it.GetValue();
                if (LOG>=1) cout << "name " << name->GetValue() << endl;
                if (obj->GetType() == PDFObject::ePDFObjectName){
                    if (LOG>=1) cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFName*)obj)->GetValue() <<endl;
                }
            }
            if (LOG>=1) cout << "page section type: " << page_section->scPDFObjectTypeLabel(page_section->GetType()) << endl;

        }
        /// page section "Resources" , "Contents"
        static void parseSection(PDFParser& parser,
                                 RefCountPtr<PDFDictionary> page,
                                 std::string section_string="Resource",
                                 NodeVisitor *vt = nullptr){
            RefCountPtr<PDFObject> page_section(parser.QueryDictionaryObject(page.GetPtr(), section_string));

            topLevelKeys(page, page_section);

            /// top level is a Dictionary
            if (page_section->GetType() == PDFObject::ePDFObjectDictionary){

                PDFnode<PDFDictionary> dictObject((PDFDictionary*)page_section.GetPtr());
                auto it = dictObject.GetIterator();

                while(it.MoveNext()) {
                    PDFName* name = it.GetKey();
                    PDFObject* obj = it.GetValue();
                    if (LOG>=1) cout << "name " << name->GetValue() << " " << obj->scPDFObjectTypeLabel(obj->GetType())<< endl;
                    // page_section = parser.QueryDictionaryObject(dObj,name->GetValue()); // TODO: not sure why this is turned off
                }
                /// THis is the first pass of the parser where we populate the character tables
                /// TODO: collect document dimensions, (maybe all the bounding boxes for text)
                dictObject.accept(*vt);

            }
            /// Top level is a Stream
            else{
                PDFnode<PDFStreamInput> inStream((PDFStreamInput*)page_section.GetPtr());
                inStream.accept(*vt);
            }
        }
    };

    // Parse page object
    RefCountPtr<PDFDictionary> page(parser->ParsePage(page_number));
    if (LOG >=1) cout << "parsing Resources..." <<endl;
    Impl::parseSection(*parser, page, "Resources", vt);

    TextVisitor * vtext = new TextVisitor(*parser);

    // TODO: factor out a test
    // cout << "map_bfchars:"  << endl;
    // for ( auto& p : vt->map_bfchars()){
    //       cout << (unsigned int)p.first << " " << p.second.c_str() << endl;
    // }

    // TODO: map of custom codes to ascii characters
    vtext->lookup_bfchars = vt->lookup_bfchars;

    if (LOG >=1) cout << "parsing Contents..." <<endl;
    Impl::parseSection(*parser, page, "Contents", vtext);

    return std::move(vtext->m_text_data);
}
