# pragma once
//(setq-default shell-file-name "/bin/bash")

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
// basic
#include <PDFWriter/PDFWriter.h>
#include <PDFWriter/PDFPage.h>
#include <PDFWriter/PageContentContext.h>
#include <PDFWriter/PDFFormXObject.h>
#include <PDFWriter/ResourcesDictionary.h>

// shapes
#include <PDFWriter/PDFFormXObject.h>
#include <PDFWriter/XObjectContentContext.h>


// pdfwriter - PARSING example
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

#include <set>
#include <cassert>

static std::set<ObjectIDType>pageIds;  // TODO: temp structure to remove duplicates of indirectRefs objects

using namespace std;
using namespace PDFHummus;

///////////////////  Parsing a page


struct  SymbolLookup{
    string bfchar_start = "beginbfchar"; //openoffice, google docs, inkspace use that.
    string bfchar_end = "endbfchar";
    // TODO: current we are extracting symbol from the object named as Differences,
    // but this may change in the future - make it more configurable - perhaps
    // with a new edge case soon, this will require some changes.
    // Currnetly using dsohowto.pdf as a test case.
    string resource_differences = "Differences";
    bool record = false;
    bool use_buffer_char = false; // for now assume we can use one or the other
    bool use_differences = false;
    // PDFArray * array;
    vector<string> differences_table;
    vector<string> bfchars;

    void add_table(PDFArray * array){
        use_differences = true;

        // symbol table is repeated multiple times and can be regenerated (for now)
        differences_table.clear();

        SingleValueContainerIterator<PDFObjectVector> it = array->GetIterator();
        while (it.MoveNext()) {
            PDFObject* obj = it.GetItem();
            if (obj->GetType() == PDFObject::ePDFObjectName){
                differences_table.push_back(((PDFName*)obj)->GetValue());
            }
            else if (obj->GetType() == PDFObject::ePDFObjectInteger){
                differences_table.push_back(std::to_string(((PDFInteger*)obj)->GetValue()));
            }
            else{
                assert(("this should not happend we need to add more type handling here", obj==nullptr));
            }
        }
    }
    void add_bfchars(string _s){
        use_buffer_char = true;
        bfchars.push_back(_s);
    }

    void print_bfchars(){
        for (auto it=bfchars.begin(); it!=bfchars.end(); it=it+2){ // store only values
            auto _key = *it;
            auto _value = *(it+1);
            cout << _key << " -- "<< _value <<endl;
        }
    }
};

// TODO: should be singleton
static SymbolLookup g_sl;


// Forward declaration
void parsePDFDictionary(PDFParser &parser, PDFDictionary *obj, int depth);
void parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *obj, int depth);
void parseObjectArray(PDFParser &parser, PDFArray *object, int depth);

void parseObjectName(PDFParser &parser, PDFName *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [name]: " << obj->GetValue() <<endl;
}

void parseObjectInteger(PDFParser &parser, PDFInteger *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [integer]: " << obj->GetValue() <<endl;
}

void parseObjectLiteralStr(PDFParser &parser, PDFLiteralString *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [literalStr]: " << obj->GetValue() <<endl;
}

void parseObjectBoolean(PDFParser &parser, PDFBoolean *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [bool]: " << obj->GetValue() <<endl;
}

void parseObjectHexString(PDFParser &parser, PDFHexString *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [hex]: " << obj->GetValue() <<endl;
}

void parseObjectNull(PDFParser &parser, PDFNull *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [NULL] " <<endl;
}

void parseObjectReal(PDFParser &parser, PDFReal *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [real]: " << obj->GetValue() <<endl;
}

void parseObjectStream(PDFParser &parser, PDFStreamInput *object, int depth){
    depth++;
    PDFDictionary* obj1 = object->QueryStreamDictionary();
    cout << std::string(depth, '.') << "streamDictionary " << endl;
    PDFObjectCastPtr<PDFDictionary> aDictionary(obj1);
    parsePDFDictionary(parser, aDictionary.GetPtr(), depth);

    cout << "content type: " << object->scPDFObjectTypeLabel(object->GetType()) << endl;

    PDFStreamInput* inStream = (PDFStreamInput*)object;//contents.GetPtr();

    int index = 0;

    PDFObjectParser* pp = parser.StartReadingObjectsFromStream(inStream);
    PDFObject *obj;
    while (obj=pp->ParseNewObject()){
        index++;
        // cout << "parsing stream " << obj <<endl;
        if (obj->GetType() == PDFObject::ePDFObjectSymbol){
            cout << obj->scPDFObjectTypeLabel(obj->GetType()) << " " << ((PDFSymbol*)obj)->GetValue() << endl;

            string symbolVal = ((PDFSymbol*)obj)->GetValue();
            // cout << "symbolVal: " << symbolVal <<endl;

            if (symbolVal.compare(g_sl.bfchar_start) == 0){
                g_sl.record = true;
            }
            else if (symbolVal.compare(g_sl.bfchar_end) == 0){
                g_sl.record = false;
            }


        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            PDFArray *arr = ((PDFArray*)obj);
            int arrlen = arr->GetLength();
            std::cout << "arrLen: " << arrlen<<std::endl;
            if (arrlen == 0)
                continue;
            else{
                SingleValueContainerIterator<PDFObjectVector> it = arr->GetIterator();
                PDFObject* obj1 = it.GetItem();
                // do {  // NOTE: with do while form first token is repeated twice.
                while (it.MoveNext()){
                    obj1 = it.GetItem();
                    // Majority of cases is to handle Integer and LiteralString
                    if (obj1->GetType() == PDFObject::ePDFObjectInteger){
                        cout << "arr : "<<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : "  << ((PDFInteger*)obj1)->GetValue() <<endl;
                    }
                    else if (obj1->GetType() == PDFObject::ePDFObjectLiteralString){
                        cout << "arr : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << ((PDFLiteralString*)obj1)->GetValue() <<endl;
                    }
                    else if (obj1->GetType() == PDFObject::ePDFObjectHexString){
                        cout << "arr : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << (((PDFHexString*)obj1)->GetValue()).c_str() <<" +++  " << pp->DecodeHexString(((PDFHexString*)obj1)->GetValue()) << endl;
                        auto hs = (PDFHexString*)obj1;
                        std::string aaa = hs->GetValue();
                        cout << *aaa.c_str() << endl;
                        for( auto a : aaa)
                            cout << (int)a << " ";
                        cout <<endl;
                    }
                    else if (obj1->GetType() == PDFObject::ePDFObjectSymbol){
                        cout << "Symbol: " << obj1->scPDFObjectTypeLabel(obj1->GetType()) << ((PDFSymbol*)obj1)->GetValue() <<  endl;
                    }
                    else {
                        cout << "arr other: " << obj1->scPDFObjectTypeLabel(obj1->GetType()) <<endl;
                    }
                }
            }
        }

        // else if (obj->GetType() == PDFObject::ePDFObjectInteger){
        //     cout << "arr : "<<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : "  << ((PDFInteger*)obj)->GetValue() <<endl;
        // }

        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            cout << "hex : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFHexString*)obj)->GetValue()  << "  +++  " << pp->DecodeHexString(((PDFHexString*)obj)->GetValue()) << endl;
            auto hs = (PDFHexString*)obj;
            std::string aaa = hs->GetValue();

            for( auto a : aaa)
                cout << int(a) << " ";
            cout <<endl;

            // store consecutive values as key-values
            if (g_sl.record) {
                g_sl.add_bfchars(aaa);
            }
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            auto name = ((PDFName*)obj)->GetValue();
            cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << name <<endl;
            if (name.compare("CMapType") == 0){
                cout << "++++CMapType" <<endl;
            }
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            cout << "litString : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFLiteralString*)obj)->GetValue() <<endl;
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectSymbol){
            cout << "Symbol: " << obj1->scPDFObjectTypeLabel(obj->GetType()) << ((PDFSymbol*)obj)->GetValue() <<  endl;
        }
        else{
            cout << "other " << obj->scPDFObjectTypeLabel(obj->GetType()) << " : " <<  endl;
        }
     }
}

void parseObjectSymbol(PDFParser &parser, PDFSymbol *obj, int depth){
    depth++;
    // equivalent to UNKNOWN
    cout << std::string(depth, ' ') << " [symbol]: " << "UNKNOWN" <<endl;
}

void parseObjectArray(PDFParser &parser, PDFArray *object, int depth){
    depth++;
    SingleValueContainerIterator<PDFObjectVector> it = object->GetIterator();
    int length = object->GetLength();
    int start = 0;
    do {
        PDFObject* obj = it.GetItem();

        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            //cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            //cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            //cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            parseObjectHexString(parser, (PDFHexString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            //cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            parseObjectNull(parser, (PDFNull*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            //cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            parseObjectName(parser, (PDFName*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            //cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            parseObjectInteger(parser, (PDFInteger*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            //cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            parseObjectReal(parser, (PDFReal*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            //cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            parseObjectArray(parser, (PDFArray*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        // TODO:this looks like create infinite recrsion
        else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            // cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
            parsePDFIndirectObjectReference(parser, (PDFIndirectObjectReference*) obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            //cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            parseObjectStream(parser, (PDFStreamInput*)obj, depth);
        }
        else {
            cout << std::string(depth, '.') << "array ELSE " << obj->GetType() << endl; //IndirectRef
        }

        start++;
    } while(it.MoveNext());

}

void parsePDFDictionary(PDFParser &parser, PDFDictionary *obj, int depth=0){
    depth++;

    auto it = obj->GetIterator();

    while(it.MoveNext()) {
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();
        cout << "name " << name->GetValue() << endl;

        if (obj->GetType() == PDFObject::ePDFObjectName){
            cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFName*)obj)->GetValue() <<endl;
        }
    }

    it = obj->GetIterator();

    do {  // TODO: this skips the first one
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();

        cout << "(parsePDFDictionary) - name " << name->GetValue() << endl;
        if (name->GetValue().compare("Parent") == 0) return;  // avoid cycles
        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            parseObjectHexString(parser, (PDFHexString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            // parseObjectNull(parser, (PDFNull*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            parseObjectName(parser, (PDFName*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            parseObjectInteger(parser, (PDFInteger*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            parseObjectReal(parser, (PDFReal*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            if (name->GetValue().compare(g_sl.resource_differences) == 0) {
                g_sl.add_table((PDFArray*)obj);
            }
            parseObjectArray(parser, (PDFArray*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
            parsePDFIndirectObjectReference(parser, (PDFIndirectObjectReference*) obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            parseObjectStream(parser, (PDFStreamInput*)obj, depth);
        }
        else {
            cout << std::string(depth, '.') << "UNKNOWN" <<endl;
        }
    } while(it.MoveNext());
}

void parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *object, int depth=0){
    depth++;

    PDFObject* obj = parser.ParseNewObject(object->mObjectID); // TODO: convert to RefCountPtr<PDFObject>

    if (obj->GetType() == PDFObject::ePDFObjectBoolean){
        //cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
        parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
        //cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
        parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectHexString){
        //cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
        parseObjectHexString(parser, (PDFHexString*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectNull){
        //cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
        parseObjectNull(parser, (PDFNull*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectName){
        // cout << std::string(depth, '.') << "ePDFObjectName" << endl;
        parseObjectName(parser, (PDFName*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectInteger){
        //cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
        parseObjectInteger(parser, (PDFInteger*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectReal){
        //cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
        parseObjectReal(parser, (PDFReal*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectArray){
        //cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
        parseObjectArray(parser, (PDFArray*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
        //cout << std::string(depth, '.') << "ePDFObjectDictionary" << endl;
        parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectStream){
        // cout << std::string(depth, '.') << "ePDFObjectStream" << endl; // parser.GetPageObjectID(obj)
        parseObjectStream(parser, (PDFStreamInput*)obj, depth);
    }
    else {
        cout << std::string(depth, '.') << "UNKNOWN" << endl;
    }
}

void parse_page(string document_path, int page_number){
    
    cout << "Extracting text from: " <<  document_path << " page: "<< page_number << endl;

    PDFParser parser;
    InputFile pdfFile;

    EStatusCode status = pdfFile.OpenFile(document_path);
    if(status != eSuccess) {
        return ;
    }

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    if(status != eSuccess) {
        return;
    }

    struct Impl {
        /// Print top level keys of the page
        static void  topLevelKeys(RefCountPtr<PDFDictionary> page,  RefCountPtr<PDFObject> page_section) {
            auto it = page.GetPtr()->GetIterator();
            while(it.MoveNext()) {
                PDFName* name = it.GetKey();
                PDFObject* obj = it.GetValue();
                cout << "name " << name->GetValue() << endl;
                if (obj->GetType() == PDFObject::ePDFObjectName){
                    cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFName*)obj)->GetValue() <<endl;
                }
            }
            cout << "page section type: " << page_section->scPDFObjectTypeLabel(page_section->GetType()) << endl;

        }
        /// page section "Resources" , "Contents"
        static void parseSection(PDFParser& parser, RefCountPtr<PDFDictionary> page, std::string section_string="Resource"){
            RefCountPtr<PDFObject> page_section(parser.QueryDictionaryObject(page.GetPtr(), section_string));

            topLevelKeys(page, page_section);

            /// top level is a Dictionary
            if (page_section->GetType() == PDFObject::ePDFObjectDictionary){
                PDFDictionary* dObj = (PDFDictionary*)page_section.GetPtr();
                auto it = dObj->GetIterator();
                while(it.MoveNext()) {
                    PDFName* name = it.GetKey();
                    PDFObject* obj = it.GetValue();
                    cout << "name " << name->GetValue() << " " << obj->scPDFObjectTypeLabel(obj->GetType())<< endl;
                    // page_section = parser.QueryDictionaryObject(dObj,name->GetValue());
                }
                parsePDFDictionary(parser, dObj, 0);
            }
            /// Top level is a Stream
            else{
                PDFStreamInput* inStream = (PDFStreamInput*)page_section.GetPtr();
                parseObjectStream(parser, inStream, 0);
            }
        // If not there is another method to store and reference symbols
            assert ((g_sl.use_buffer_char == true) || (g_sl.use_differences == true));
        }
    };

    // Parse page object
    RefCountPtr<PDFDictionary> page(parser.ParsePage(page_number));
    Impl::parseSection(parser, page, "Resources");
    cout << "================================" <<endl;
    Impl::parseSection(parser, page, "Contents");

}
