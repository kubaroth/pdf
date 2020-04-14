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
#include <memory>
#include <queue>

static std::set<ObjectIDType>pageIds;  // TODO: temp structure to remove duplicates of indirectRefs objects

using namespace std;
using namespace PDFHummus;

#define LOG 2 // 0: no output, 1:more 2:more 3:all (currently disabled)

///////////////////  Parsing a page
struct TextData {
    std::string text = "";

};


struct  SymbolLookup{

    const string bfchar_start = "beginbfchar"; // using resouce dictionary lookup
    const string bfchar_end = "endbfchar";
    int bfchars_index = 0;  //used take modulo of the value and  0: set as key, or value otherwise
    // TODO: current we are extracting symbol from the object named as Differences,
    // but this may change in the future - make it more configurable - perhaps
    // with a new edge case soon, this will require some changes.

    // TODO: another mode Td <-> Tj to lookup but add to char + 25 magic number
    // (Looks like this is number of letters in A-Z)
    /*
      if hexstring is between symbols Tf <-> TJ - use lookup
      if Td <-> Tj - dont lookup use offet
      else not a hexString but some special characters (\002) may be

    */


    // Currnetly using dsohowto.pdf as a test case.
    const string resource_differences = "Differences";

    
   
    bool record = false;  // Mark enable when traversing the 'use_differences' section
    // For now let's assume we can use one or the other
    bool use_buffer_char = false;  // Option 1: strings stored in a stream + some symbol lookup
    bool use_differences = false;  // Option 2: (hex strings) stored in dictionary

    queue<PDFObject*> objectStack;
    
    vector<string> differences_table; // thiess is a symbol table
    vector<string> bfchars;   // Option 2 text data

    map<char, char> map_bfchars;   // Option 2 text data
    tuple<char, char, int > symbol_pair; // key, value, index - helper to store key values while extracting hexstrings from stream

    // text data from a stream converted to text
    // The data to be pulated is moved from externally initilized variable
    unique_ptr<TextData> text_data;

    // required assignment operator to handle const members
    SymbolLookup () {
        bfchars_index = 0;
    }

    // required assignment operator to handle const members
    SymbolLookup& operator=(const SymbolLookup& other) {
        bfchars_index = 0;
        return *this;
    }
    
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
        use_buffer_char = true;  // mark the mode pdf data is stored
        // TODO: we want to store key:value map instead of list
        // keep track of modulo to set key or value
        bfchars.push_back(_s);
        if (bfchars_index%2 == 0){
            //assert (_s.size() == 1);  // TODO: assumption this is a 'char'  // \001
            char key_char = _s[0];
            symbol_pair = {key_char, 'a' , bfchars_index};
        }
        else {
            // only add if there are consecutive indices
            if ( std::get<2>(symbol_pair) + 1  == bfchars_index){
                // assert (_s.size() <= 2);    // TODO: the size is 2   "\000B"


                //pair<char,char> new_pair = {std::get<0>(symbol_pair), _s[1]};
                // map_bfchars.insert(new_pair); // insertion is skipped on subsequent keys
                // if (_s.size() == 1)
                //     map_bfchars[std::get<0>(symbol_pair)] = _s[0];
                if (_s.size() == 2)
                    map_bfchars[std::get<0>(symbol_pair)] = _s[1];

                
            }

        }

        bfchars_index++;
    }

    void print_bfchars() const {
        int index = 0;
        for (auto it=bfchars.begin(); it!=bfchars.end(); it=it+2){ // store only values
            auto _key = *it;
            auto _value = *(it+1);
            if (LOG>=1) cout << "index:" << index  << " key:" << _key << " value:  "<< _value <<endl;
            index++;
        }
    }
};

// using module visbility (static) as a shorthand
// to avoid passing this around
static SymbolLookup g_symLookup;


// Forward declaration
void parsePDFDictionary(PDFParser &parser, PDFDictionary *obj, int depth);
void parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *obj, int depth);
void parseObjectArray(PDFParser &parser, PDFArray *object, int depth);

void parseObjectName(PDFParser &parser, PDFName *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [name]: " << obj->GetValue() <<endl;

}

void parseObjectInteger(PDFParser &parser, PDFInteger *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [integer]: " << obj->GetValue() <<endl;

}

void parseObjectLiteralStr(PDFParser &parser, PDFLiteralString *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [literalStr]: " << obj->GetValue() <<endl;
}

void parseObjectBoolean(PDFParser &parser, PDFBoolean *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [bool]: " << obj->GetValue() <<endl;
}

void parseObjectHexString(PDFParser &parser, PDFHexString *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [hex]: " << obj->GetValue() <<endl;
}

void parseObjectNull(PDFParser &parser, PDFNull *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [NULL] " <<endl;
}

void parseObjectReal(PDFParser &parser, PDFReal *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [real]: " << obj->GetValue() <<endl;
}

void parseObjectStream(PDFParser &parser, PDFStreamInput *object, int depth){
    depth++;
    PDFDictionary* obj1 = object->QueryStreamDictionary();
    if (LOG>=2) cout << std::string(depth, '.') << "streamDictionary " << endl;
    PDFObjectCastPtr<PDFDictionary> aDictionary(obj1);
    parsePDFDictionary(parser, aDictionary.GetPtr(), depth);
    if (LOG>=2) cout << "content type: " << object->scPDFObjectTypeLabel(object->GetType()) << endl;
    PDFStreamInput* inStream = (PDFStreamInput*)object;

    PDFObjectParser* pp = parser.StartReadingObjectsFromStream(inStream);
    PDFObject *obj;

    // Start parsing objects from a stream
    cout << "streamStart" << "-------" << endl;
    while (obj=pp->ParseNewObject()){

        if (obj->GetType() == PDFObject::ePDFObjectSymbol){
            if (LOG>=2) cout << obj->scPDFObjectTypeLabel(obj->GetType()) << " " << ((PDFSymbol*)obj)->GetValue() << endl;
            string symbolVal = ((PDFSymbol*)obj)->GetValue();
            // cout << "symbolVal: " << symbolVal <<endl;

            // Option 1 if the bfchar token is encountered
            if (g_symLookup.bfchar_start.find(symbolVal) != string::npos){
                g_symLookup.record = true;
            }
            else if (g_symLookup.bfchar_end.find(symbolVal) != string::npos){
                g_symLookup.record = false;
            }
        }
        // Option 1 (using stream)
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            PDFArray *arr = ((PDFArray*)obj);
            int arrlen = arr->GetLength();
            if (arrlen == 0)
                continue;
            else{
                SingleValueContainerIterator<PDFObjectVector> it = arr->GetIterator();
                PDFObject* obj1 = it.GetItem();
                // do {  // NOTE: with do while form first token is repeated twice.
                while (it.MoveNext()){
                    obj1 = it.GetItem();
                    // These are majority of cases to handle Integer and LiteralString
                    // used in the text stream
                    // int: is offset positioning 0.001 mm (check pdf reference)
                    // Literal string: is the (word) inside parenthesis
                    // TODO: store data (keep acumulate for now)
                    if (obj1->GetType() == PDFObject::ePDFObjectInteger){
                        if (LOG>=2) cout << "arr (int) : "<<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : "  << ((PDFInteger*)obj1)->GetValue() <<endl;
                    }
                    else if (obj1->GetType() == PDFObject::ePDFObjectLiteralString){
                        if (LOG>=2) cout << "arr (str) : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << ((PDFLiteralString*)obj1)->GetValue() <<endl;
                    }
                    // Option 2 : hexstrings with table lookup only (bginfchar/endbfchar)
                    else if (obj1->GetType() == PDFObject::ePDFObjectHexString){
                        if (LOG>=2) cout << "arr (hex) : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << (((PDFHexString*)obj1)->GetValue()).c_str() <<" +++  " << pp->DecodeHexString(((PDFHexString*)obj1)->GetValue()) << endl;
                        auto hs = (PDFHexString*)obj1;
                        std::string lookup_key_seq = hs->GetValue();
                        // iterate and lookup up each key
                        string text = "";  // TODO keep accumulate for now
                        for (auto &_char : lookup_key_seq){
                            text += g_symLookup.map_bfchars[_char];
                        }
                        g_symLookup.text_data->text += text;
                    }
                    else if (obj1->GetType() == PDFObject::ePDFObjectSymbol){
                        if (LOG>=2) cout << "Symbol: " << obj1->scPDFObjectTypeLabel(obj1->GetType()) << ((PDFSymbol*)obj1)->GetValue() <<  endl;
                    }
                    else {
                        if (LOG>=2) cout << "arr other: " << obj1->scPDFObjectTypeLabel(obj1->GetType()) <<endl;
                    }
                }
            }
        }

        // else if (obj->GetType() == PDFObject::ePDFObjectInteger){
        //     cout << "arr : "<<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : "  << ((PDFInteger*)obj)->GetValue() <<endl;
        // }


        // Option 2 : parse HexStrings
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            if (LOG>=2) cout << "hex : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFHexString*)obj)->GetValue()  << "  +++  " << pp->DecodeHexString(((PDFHexString*)obj)->GetValue()) << endl;
            auto hs = (PDFHexString*)obj;
            // store consecutive values as key-values
            // The mapping will be somthing along this lines "\001" -> "\000B" is is unicode?
            if (g_symLookup.record ) { // || g_symLookup.prevObject
                string value = hs->GetValue();
                g_symLookup.add_bfchars(value);
            }
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            auto name = ((PDFName*)obj)->GetValue();
            if (LOG>=1) cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << name <<endl;
            if (name.compare("CMapType") == 0){
                if (LOG>=1)cout << "++++CMapType" <<endl;
            }
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            if (LOG>=2) cout << "litString : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFLiteralString*)obj)->GetValue() <<endl;
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // if (LOG>=3) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectSymbol){
            if (LOG>=2) cout << "Symbol: " << obj1->scPDFObjectTypeLabel(obj->GetType()) << ((PDFSymbol*)obj)->GetValue() <<  endl;
        }
        else{
            if (LOG>=2) cout << "other " << obj->scPDFObjectTypeLabel(obj->GetType()) << " : " <<  endl;
        }
    }
    cout << "streamEnd" << "--------" << endl;
}

void parseObjectSymbol(PDFParser &parser, PDFSymbol *obj, int depth){
    depth++;
    // equivalent to UNKNOWN
    if (LOG>=2) cout << std::string(depth, ' ') << " [symbol]: " << "UNKNOWN" <<endl;
}

void parseObjectArray(PDFParser &parser, PDFArray *object, int depth){
    depth++;
    SingleValueContainerIterator<PDFObjectVector> it = object->GetIterator();
    int length = object->GetLength();
    int start = 0;
    do {
        PDFObject* obj = it.GetItem();

        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            parseObjectHexString(parser, (PDFHexString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            parseObjectNull(parser, (PDFNull*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            parseObjectName(parser, (PDFName*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            parseObjectInteger(parser, (PDFInteger*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            parseObjectReal(parser, (PDFReal*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            parseObjectArray(parser, (PDFArray*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // if (LOG>=3) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        // TODO:this looks like create infinite recrsion
        else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            // if (LOG>=3) cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
            parsePDFIndirectObjectReference(parser, (PDFIndirectObjectReference*) obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            parseObjectStream(parser, (PDFStreamInput*)obj, depth);
        }
        else {
            if (LOG>=3) cout << std::string(depth, '.') << "array ELSE " << obj->GetType() << endl; //IndirectRef
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
        if (LOG>=1) cout << "name " << name->GetValue() << endl;

        if (obj->GetType() == PDFObject::ePDFObjectName){
            if (LOG>=2) cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFName*)obj)->GetValue() <<endl;
        }
    }

    it = obj->GetIterator();

    do {  // TODO: this skips the first one
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();

        if (LOG>=1) cout << "(parsePDFDictionary) - name " << name->GetValue() << endl;
        if (name->GetValue().compare("Parent") == 0) return;  // avoid cycles
        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            parseObjectHexString(parser, (PDFHexString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            // parseObjectNull(parser, (PDFNull*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            parseObjectName(parser, (PDFName*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            parseObjectInteger(parser, (PDFInteger*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            parseObjectReal(parser, (PDFReal*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            // Option 2 (symbol lookup table)
            if (name->GetValue().compare(g_symLookup.resource_differences) == 0) {
                g_symLookup.add_table((PDFArray*)obj);
            }
            parseObjectArray(parser, (PDFArray*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            if (LOG>=2) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            if (LOG>=2) cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
            parsePDFIndirectObjectReference(parser, (PDFIndirectObjectReference*) obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            parseObjectStream(parser, (PDFStreamInput*)obj, depth);
        }
        else {
            if (LOG>=2) cout << std::string(depth, '.') << "UNKNOWN" <<endl;
        }
    } while(it.MoveNext());
}

void parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *object, int depth=0){
    depth++;

    PDFObject* obj = parser.ParseNewObject(object->mObjectID); // TODO: convert to RefCountPtr<PDFObject>

    if (obj->GetType() == PDFObject::ePDFObjectBoolean){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
        parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
        parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectHexString){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
        parseObjectHexString(parser, (PDFHexString*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectNull){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
        parseObjectNull(parser, (PDFNull*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectName){
        // if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectName" << endl;
        parseObjectName(parser, (PDFName*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectInteger){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
        parseObjectInteger(parser, (PDFInteger*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectReal){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
        parseObjectReal(parser, (PDFReal*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectArray){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
        parseObjectArray(parser, (PDFArray*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectDictionary" << endl;
        parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectStream){
        // if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectStream" << endl; // parser.GetPageObjectID(obj)
        parseObjectStream(parser, (PDFStreamInput*)obj, depth);
    }
    else {
        if (LOG>=3) cout << std::string(depth, '.') << "UNKNOWN" << endl;
    }
}

unique_ptr<TextData> parse_page(string document_path, int page_number){

    cout << "Extracting text from: " <<  document_path << " page: "<< page_number << endl;

    PDFParser parser;
    InputFile pdfFile;

    // auto symLookup = make_shared<SymbolLookup>();

    // g_symLookup(); // reinitialize on each parse page invocation;
    
    unique_ptr<TextData> textData( new TextData() );
    g_symLookup = SymbolLookup();  
    g_symLookup.text_data = std::move(textData);

    EStatusCode status = pdfFile.OpenFile(document_path);
    if(status != eSuccess) {
        cout << "No Document Found..." << endl;
        return nullptr;
    }

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    if(status != eSuccess) {
        return nullptr;
    }

    struct Impl {
        /// Print top level keys of the page
        static void  topLevelKeys(RefCountPtr<PDFDictionary> page,  RefCountPtr<PDFObject> page_section) {
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
                    if (LOG>=1) cout << "name " << name->GetValue() << " " << obj->scPDFObjectTypeLabel(obj->GetType())<< endl;
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
            // assert ((g_symLookup.use_buffer_char == true) || (g_symLookup.use_differences == true));
        }
    };

    // Parse page object
    RefCountPtr<PDFDictionary> page(parser.ParsePage(page_number));
    cout << "parsing Resource..." <<endl;
    Impl::parseSection(parser, page, "Resources");
    cout << "parsing Contents..." <<endl;
    Impl::parseSection(parser, page, "Contents");

    return std::move(g_symLookup.text_data);
}
