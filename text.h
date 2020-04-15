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

    // Currnetly using dsohowto.pdf as a test case.
    const string resource_differences = "Differences";

    
   
    bool record = false;  // Mark enable when traversing the 'use_differences' section
    // For now let's assume we can use one or the other
    bool use_buffer_char = false;  // Option 1: strings stored in a stream + some symbol lookup
    bool use_differences = false;  // Option 2: (hex strings) stored in dictionary

    queue<PDFObject*> objectStack;
    
    vector<string> differences_table; // thiess is a symbol table
    vector<string> bfchars;   // Option 2 text data

    map<char, string> map_bfchars;   // Option 2 text data
    tuple<char, string, int > symbol_pair; // key, value, index - helper to store key values while extracting hexstrings from stream

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
    /// if the bfchar symbol is encounterd add (index, character) pair to the lookup table
    void add_bfchars(string _s){
        use_buffer_char = true;  // mark the mode pdf data is stored
        // TODO: we want to store key:value map instead of list
        // keep track of modulo to set key or value
        bfchars.push_back(_s);

        
        if (bfchars_index%2 == 0){
            char key_char;
            if (_s.size() == 1)
                key_char= _s[0]; // NOTE: here key_string is single char
            else
                key_char= _s[1]; 
            symbol_pair = {key_char, "a" , bfchars_index};
        }
        else {
            // only add if there are consecutive indices
            if ( std::get<2>(symbol_pair) + 1  == bfchars_index){

                // handling ligatures ff, fi, fl (TODO: there are more...)
                if ((_s[0] == '\373') && (_s[1] == '\000')) {
                    // todo handle keys as \000\002 instead \002
                    map_bfchars[std::get<0>(symbol_pair)] = "fi"; // ff -> can't use 'ff' as we use char
                }
                else if ((_s[0] == '\373') && (_s[1] == '\001')) {
                    map_bfchars[std::get<0>(symbol_pair)] = "fi"; // fi
                }
                else if ((_s[0] == '\373') && (_s[1] == '\002')) {
                    map_bfchars[std::get<0>(symbol_pair)] = "fl"; // fl
                }
                else {
                    string char_string(1, _s[1]);
                    map_bfchars[std::get<0>(symbol_pair)] = char_string;
                }
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
    pair<string,string> symbol_next = {"",""};
    while (obj=pp->ParseNewObject()){
        // Handle Symbol objects
        if (obj->GetType() == PDFObject::ePDFObjectSymbol){
            if (LOG>=2) cout << obj->scPDFObjectTypeLabel(obj->GetType()) << " " << ((PDFSymbol*)obj)->GetValue() << endl;
            string symbolVal = ((PDFSymbol*)obj)->GetValue();

            // Record the current symbol and alter the logic of the
            // next encounterd HexString in the Stream
            
            // Store the currnet one and previous symbol
            symbol_next.first = symbol_next.second;
            symbol_next.second = symbolVal;

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
                    if (obj1->GetType() == PDFObject::ePDFObjectInteger){
                        if (LOG>=2) cout << "arr (int) : "<<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : "  << ((PDFInteger*)obj1)->GetValue() <<endl;
                    }
                    // inkspace
                    else if (obj1->GetType() == PDFObject::ePDFObjectLiteralString){
                        if (LOG>=2) cout << "arr (str) : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << ((PDFLiteralString*)obj1)->GetValue() <<endl;
                        if (symbol_next.second.compare("Tf") == 0){
                            string text = ((PDFLiteralString*)obj1)->GetValue();
                            g_symLookup.text_data->text += text;
                        }
                    }
                    // Option 2 : hexstrings with table lookup only (bginfchar/endbfchar)
                    else if (obj1->GetType() == PDFObject::ePDFObjectHexString){
                        if (LOG>=2) cout << "arr (hex) : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << (((PDFHexString*)obj1)->GetValue()).c_str() <<" +++  " << pp->DecodeHexString(((PDFHexString*)obj1)->GetValue()) << endl;
                        auto hs = (PDFHexString*)obj1;
                        std::string lookup_key_seq = hs->GetValue();

                        // openoffice
                        // iterate and lookup up each key
                        // TODO: needs better handling - to only execute for string blocks
                        string text = "";
                        for (auto &_char : lookup_key_seq){
                            if ( g_symLookup.map_bfchars.find(_char) != g_symLookup.map_bfchars.end()){
                                text += g_symLookup.map_bfchars[_char];
                            }
                        }
                        g_symLookup.text_data->text += text;
                    }
                    else if (obj1->GetType() == PDFObject::ePDFObjectSymbol){
                        if (LOG>=2) cout << "Symbol:: " << obj1->scPDFObjectTypeLabel(obj1->GetType()) << ((PDFSymbol*)obj1)->GetValue() <<  endl;
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
            string value = hs->GetValue();

            // Stream following "beginbfchar" symbol are key-value pairs "\001" -> "\000B"
            // Store then in the lookup table
            
            // if (g_symLookup.record ) {  // TODO: cleanup - this is no longer needed
            if (symbol_next.second.compare("beginbfchar") == 0){
                string value = hs->GetValue();
                if (value.size() <= 2) // some keys are "\001" but some "\000\003"
                    g_symLookup.add_bfchars(value);
            }

            else if ((symbol_next.second.compare("Td") == 0) ||
                     (symbol_next.second.compare("Tf") == 0))  // ligatures
                {
                string value = hs->GetValue();
                int a =1 ;

                // NOTE: Hanling new-line character
                if ((value[0] == '\000') && (value[1] == '\001')){
                    g_symLookup.text_data->text += "\n";
                    // continue;
                }

                if (value.size() == 2){  // so far seem like keys are always 2bytes
                    // We use second byte for lookup for now: extract second char:  \000\003
                    char key = value[1];
                    string string_value;

                    if ( g_symLookup.map_bfchars.find(key) != g_symLookup.map_bfchars.end()){
                        string_value = g_symLookup.map_bfchars[key];
                        // For (keys value) which which are not equal
                        // use that char without offet
                        if (string_value[1] != key) {
                            g_symLookup.text_data->text += string_value;
                        }
                        else {
                            key += 29;  // magic number, offset in ascii table
                            string key_string(1, key);
                            g_symLookup.text_data->text += key_string;
                        }
                 
                    }
                    // For remaining majority of keys not found in the lookup dictionary
                    else{
                        key += 29;  // magic number
                        string key_string(1,key);
                        g_symLookup.text_data->text += key_string;
                    }
                    
                }
                else{
                    string value = hs->GetValue();
                    cout << "otherhex: " << value << endl;
                }
                
            }

            cout << "symbol_prev: " << symbol_next.first << "  symbol curr: " << symbol_next.second <<endl;
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
            if (symbol_next.second.compare("Tf") == 0){
                string text = ((PDFLiteralString*)obj)->GetValue();
                g_symLookup.text_data->text += text;
            }

        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // if (LOG>=3) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectSymbol){
            if (LOG>=2) cout << "Symbol: " << obj->scPDFObjectTypeLabel(obj->GetType()) << ((PDFSymbol*)obj)->GetValue() <<  endl;
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
