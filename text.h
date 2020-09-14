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

#include "pdftraverse.h"

using namespace std;
using namespace PDFHummus;

// 0: no output
// 1: main messages
// 2: useful debug
// 3: all (currently disabled, as these execution paths are not used)
#define LOG 1

/// Store extracted text (location/alignment/bbox in the future)
struct TextData {
    std::string text = "";
    TextData() : text(""){};
};


class SymbolLookup{

public:
    // required assignment operator to handle const members
    SymbolLookup () {
        m_bfchars_index = 0;
    }

    // required assignment operator to handle const members
    SymbolLookup& operator=(const SymbolLookup& other) {
        m_bfchars_index = 0;
        return *this;
    }
    
    // text data from a stream converted to text
    // The data to be pulated is moved from externally initilized variable
    unique_ptr<TextData> m_text_data;

private:
    int m_bfchars_index = 0;  //used take modulo of the value and  0: set as key, or value otherwise

    // Currnetly using dsohowto.pdf as a test case.
    const string resource_differences = "Differences";

    map<char, string> m_map_bfchars;
    tuple<char, string, int > m_symbol_pair; // key, value, index - helper to store key values while extracting hexstrings from stream

    // For ObjectLiteralString part of ObjArray words can be broken at the end of the line
    // having "-" at the start/end of the string.
    // Each part being a separate StringLiteral object.
    // While adding " " character we need to take into accont "-" and skip adding the " "
    pair<string, string> m_prev_word = {"", ""};  // original unprocessed text to search for dashes
    pair<char, string> m_lookup_pair; // The same purpose as 'm_symbol_pair' TODO: merge them together
    bool m_add_space = false;  // if previous Object is Integer for Array and current is LiteralString enable

    /// Extracting the (key,value) mapping from "Differences" encoding dictionary
    /// Specifies  complete character encoding for this font.
    void add_table(PDFArray * array);

    /// Extracting the (key,value) mapping if the bfchar symbol is encountered
    /// add (index, character) pair to the lookup table
    void add_bfchars(string _s);

    /// PDF Traversal
public:
    /// Currently used by parse_page function
    void parsePDFDictionary(PDFParser &parser, PDFDictionary *obj, int depth);
private:
    void parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *obj, int depth);
    void parseObjectArray(PDFParser &parser, PDFArray *object, int depth);

    void parseObjectName(PDFParser &parser, PDFName *obj, int depth);
    void parseObjectInteger(PDFParser &parser, PDFInteger *obj, int depth);
    void parseObjectLiteralStr(PDFParser &parser, PDFLiteralString *obj, int depth);
    void parseObjectBoolean(PDFParser &parser, PDFBoolean *obj, int depth);
    void parseObjectReal(PDFParser &parser, PDFReal *obj, int depth);
    void parseObjectHexString(PDFParser &parser, PDFHexString *obj, int depth);
    void parseObjectNull(PDFParser &parser, PDFNull *obj, int depth);
public:
    /// Most of the text extraction is handled by Stream Object
    void parseObjectStream(PDFParser &parser, PDFStreamInput *object, int depth);

private:
    void parseObjectSymbol(PDFParser &parser, PDFSymbol *obj, int depth);
};

inline void SymbolLookup::add_table(PDFArray * array){

    SingleValueContainerIterator<PDFObjectVector> it = array->GetIterator();
    char index = 0;  // keep track when the pair needs to be saved

    while (it.MoveNext()) {
        index++;
        PDFObject* obj = it.GetItem();
        if (obj->GetType() == PDFObject::ePDFObjectName){
            string name = ((PDFName*)obj)->GetValue();
            m_lookup_pair.second = name;
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            char key_index = ((PDFInteger*)obj)->GetValue();
            m_lookup_pair.first = key_index;
        }
        else{
            assert(("this should not happend we need to add more type handling here", obj==nullptr));
        }
        // update lookup table
        if (index % 2 == 0){
            m_map_bfchars[m_lookup_pair.first] = m_lookup_pair.second;
        }
    }
}

inline void SymbolLookup::add_bfchars(string _s){


    if (m_bfchars_index%2 == 0){
        char key_char;
        if (_s.size() == 1)
            key_char= _s[0]; // NOTE: here key_string is single char
        else
            key_char= _s[1];
        m_symbol_pair = {key_char, "a" , m_bfchars_index};
    }
    else {
        // only add if there are consecutive indices
        if ( std::get<2>(m_symbol_pair) + 1  == m_bfchars_index){

            // handling ligatures ff, fi, fl (TODO: there are more...)
            if ((_s[0] == '\373') && (_s[1] == '\000')) {
                // todo handle keys as \000\002 instead \002
                m_map_bfchars[std::get<0>(m_symbol_pair)] = "fi"; // ff -> can't use 'ff' as we use char
            }
            else if ((_s[0] == '\373') && (_s[1] == '\001')) {
                m_map_bfchars[std::get<0>(m_symbol_pair)] = "fi"; // fi
            }
            else if ((_s[0] == '\373') && (_s[1] == '\002')) {
                m_map_bfchars[std::get<0>(m_symbol_pair)] = "fl"; // fl
            }
            // handling regular characters
            else {
                string char_string(1, _s[1]);
                m_map_bfchars[std::get<0>(m_symbol_pair)] = char_string;
            }
        }
    }

    m_bfchars_index++;
}

inline void SymbolLookup::parseObjectName(PDFParser &parser, PDFName *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [name]: " << obj->GetValue() <<endl;

}

inline void SymbolLookup::parseObjectInteger(PDFParser &parser, PDFInteger *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [integer]: " << obj->GetValue() <<endl;

}

inline void SymbolLookup::parseObjectLiteralStr(PDFParser &parser, PDFLiteralString *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [literalStr]: " << obj->GetValue() <<endl;
}

inline void SymbolLookup::parseObjectBoolean(PDFParser &parser, PDFBoolean *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [bool]: " << obj->GetValue() <<endl;
}

inline void SymbolLookup::parseObjectHexString(PDFParser &parser, PDFHexString *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [hex]: " << obj->GetValue() <<endl;
}

inline void SymbolLookup::parseObjectNull(PDFParser &parser, PDFNull *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [NULL] " <<endl;
}

inline void SymbolLookup::parseObjectReal(PDFParser &parser, PDFReal *obj, int depth){
    depth++;
    if (LOG>=2) cout << std::string(depth, ' ') << " [real]: " << obj->GetValue() <<endl;
}

inline void SymbolLookup::parseObjectStream(PDFParser &parser, PDFStreamInput *object, int depth){
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

    // Keep track of the currnet one and previous symbol
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
                // TODO: very minor - but skip if the last word in previous array had "-" see: m_prev_word
                m_add_space = true; // Assures the first word in array will have space inserted at the front
                while (it.MoveNext()){
                    obj1 = it.GetItem();
                    // These are majority of cases to handle Integer and LiteralString
                    // used in the text stream
                    // int: is offset positioning 0.001 mm (check pdf reference)
                    // Literal string: is the (word) inside parenthesis
                    if (obj1->GetType() == PDFObject::ePDFObjectInteger){
                        if (LOG>=2) cout << "arr (int) : "<<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : "  << ((PDFInteger*)obj1)->GetValue() <<endl;
                        int value = ((PDFInteger*)obj1)->GetValue();
                        if (std::abs(value) > 50){  // Magic - offset less then 50 are considered as single word
                            m_add_space = true;
                        }
                    }
                    // LookupOption 2 - inkspace
                    else if (obj1->GetType() == PDFObject::ePDFObjectLiteralString){
                        if (LOG>=2) cout << "arr (str) : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << ((PDFLiteralString*)obj1)->GetValue() <<endl;
                        if ((symbol_next.second.compare("Tf") == 0) ||
                            (symbol_next.second.compare("Td") == 0)) {  // dsohowto.pdf
                            string text = ((PDFLiteralString*)obj1)->GetValue();

                            // lookup value in the table, and update text variable
                            string new_text = "";
                            for (auto &_char : text){
                                if ( m_map_bfchars.find(_char) != m_map_bfchars.end()){
                                    new_text += m_map_bfchars[_char];
                                }
                                else {
                                    new_text += _char;
                                }
                            }

                            // update m_prev_word pair
                            m_prev_word.first = m_prev_word.second;
                            m_prev_word.second = new_text;

                            // skip the work we will combine in (next iteration)
                            auto found = m_prev_word.second.find('-');
                            if (found == m_prev_word.second.size()-1) {
                                continue;
                            }

                            // (Next iteration)
                            // merge previoius word wit the current
                            found = m_prev_word.first.find('-');
                            string first_word = m_prev_word.first;
                            if (found == first_word.size()-1) {
                                string first = first_word.substr(0,first_word.size()-1);
                                new_text = first + m_prev_word.second;

                                if (LOG>=2) cout << "combined word_pair " << m_prev_word.first<< ":";
                                cout << m_prev_word.second;
                                cout << " " << new_text << endl;
                            }

                            // Option 2 - introduce extra space between each LiteralStringObject (where required)
                            if (m_add_space){
                                new_text = " " + new_text;
                            }
                            // skip adding space
                            else { }

                            // Finally update the global text
                            m_text_data->text += new_text;
                            m_add_space = false; // reset to false

                        }
                    }
                    // Option 2 : hexstrings with table lookup only (bginfchar/endbfchar)
                    else if (obj1->GetType() == PDFObject::ePDFObjectHexString){
                        if (LOG>=2) cout << "arr (hex) : " <<  obj1->scPDFObjectTypeLabel(obj1->GetType()) << " : " << (((PDFHexString*)obj1)->GetValue()).c_str() <<" +++  " << pp->DecodeHexString(((PDFHexString*)obj1)->GetValue()) << endl;
                        auto hs = (PDFHexString*)obj1;
                        std::string lookup_key_seq = hs->GetValue();

                        // LookupOption 1 - openoffice
                        // iterate and lookup up each key
                        // TODO: needs better handling - to only execute for string blocks
                        string text = "";
                        for (auto &_char : lookup_key_seq){
                            if ( m_map_bfchars.find(_char) != m_map_bfchars.end()){
                                text += m_map_bfchars[_char];
                            }
                        }

                        // remove replacement of double quote with 'quotedblright'
                        if (text.find("quote") != string::npos)
                            continue;

                        m_text_data->text += text;
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

            // Build Lookup-Table
            // Stream following "beginbfchar" symbol are key-value pairs "\001" -> "\000B" (char, string)

            if (symbol_next.second.compare("beginbfchar") == 0){
                string value = hs->GetValue();
                if (value.size() <= 2) // some keys are "\001" but some "\000\003"
                    add_bfchars(value);
            }

            else if ((symbol_next.second.compare("Td") == 0) ||
                     (symbol_next.second.compare("Tf") == 0))  // ligatures
                {
                string value = hs->GetValue();

                // NOTE: Handling new-line character
                if ((value[0] == '\000') && (value[1] == '\001')){
                    m_text_data->text += "\n";
                }

                // LookupOption(3) - googledocs + ligatures
                if (value.size() == 2){  // so far seem like keys are always 2bytes
                    // We use second byte for lookup for now: extract second char:  \000\003
                    char key = value[1];
                    string string_value;

                    if ( m_map_bfchars.find(key) != m_map_bfchars.end()){
                        string_value = m_map_bfchars[key];
                        // For (key, value) which which are not equal use that char without offet
                        // NOTE: This include ligatures lookup.
                        if (string_value[1] != key) {
                            m_text_data->text += string_value;
                        }
                        else {
                            key += 29;  // magic number, offset in ascii table
                            string key_string(1, key);
                            m_text_data->text += key_string;
                        }

                    }
                    // For remaining majority of keys (googledocs document) not found in the lookup dictionary
                    else{
                        key += 29;  // magic number
                        string key_string(1,key);
                        m_text_data->text += key_string;
                    }

                }
                else{
                    // LookupOption(6) Yet another way to lookup
                    string lookup_key_seq = hs->GetValue();
                    string text = "";
                    for (auto &_char : lookup_key_seq){
                        if ( m_map_bfchars.find(_char) != m_map_bfchars.end()){
                            text += m_map_bfchars[_char];
                        }
                    }
                    m_text_data->text += text;
                    if (LOG>=1) cout << "otherhex Tf: " << lookup_key_seq << " lookup values: " << text << endl;
                }

            }

            if (LOG>=2) cout << "symbol_prev: " << symbol_next.first << "  symbol curr: " << symbol_next.second <<endl;
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
                m_text_data->text += text;
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
}

inline void SymbolLookup::parseObjectSymbol(PDFParser &parser, PDFSymbol *obj, int depth){
    depth++;
    // equivalent to UNKNOWN
    if (LOG>=2) cout << std::string(depth, ' ') << " [symbol]: " << "UNKNOWN" <<endl;
}

inline void SymbolLookup::parseObjectArray(PDFParser &parser, PDFArray *object, int depth){
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
/// Entry Point 1
inline void SymbolLookup::parsePDFDictionary(PDFParser &parser, PDFDictionary *obj, int depth=0){
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
            if (name->GetValue().compare(resource_differences) == 0) {
                add_table((PDFArray*)obj);
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

inline void SymbolLookup::parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *object, int depth=0){
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

inline unique_ptr<TextData> parse_page(string document_path, int page_number){

    cout << "Extracting text from: " <<  document_path << " page: "<< page_number << endl;

    PDFParser parser;
    InputFile pdfFile;

    unique_ptr<TextData> textData( new TextData() );
    SymbolLookup g_symLookup;
    g_symLookup.m_text_data = std::move(textData);

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
        static void parseSection(PDFParser& parser, RefCountPtr<PDFDictionary> page, SymbolLookup& lookup, std::string section_string="Resource"){
            RefCountPtr<PDFObject> page_section(parser.QueryDictionaryObject(page.GetPtr(), section_string));

            topLevelKeys(page, page_section);

            /// top level is a Dictionary
            if (page_section->GetType() == PDFObject::ePDFObjectDictionary){
                // TODO: replace with PDFnode<PDFDictionary>
                PDFnode<PDFDictionary> aaa(std::move (*(PDFDictionary*)page_section.GetPtr()));
                PDFnode<PDFDictionary*> aaa_ptr(std::move((PDFDictionary*)page_section.GetPtr()));

                PDFDictionary* dictObject = (PDFDictionary*)page_section.GetPtr();
                auto it = dictObject->GetIterator();
                while(it.MoveNext()) {
                    PDFName* name = it.GetKey();
                    PDFObject* obj = it.GetValue();
                    if (LOG>=1) cout << "name " << name->GetValue() << " " << obj->scPDFObjectTypeLabel(obj->GetType())<< endl;
                    // page_section = parser.QueryDictionaryObject(dObj,name->GetValue()); // TODO: not sure why this is turned off
                }
                /// THis is the first pass of the parser where we populate the character tables
                /// TODO: collect document dimentions, (maybe all the bounding boxes for text)
                lookup.parsePDFDictionary(parser, dictObject, 0);
            }
            /// Top level is a Stream
            else{
                PDFStreamInput* inStream = (PDFStreamInput*)page_section.GetPtr();
                lookup.parseObjectStream(parser, inStream, 0);
            }
        }
    };

    // Parse page object
    RefCountPtr<PDFDictionary> page(parser.ParsePage(page_number));
    if (LOG >=1) cout << "parsing Resources..." <<endl;
    Impl::parseSection(parser, page, g_symLookup, "Resources");
    if (LOG >=1) cout << "parsing Contents..." <<endl;
    Impl::parseSection(parser, page, g_symLookup, "Contents");

    return std::move(g_symLookup.m_text_data);
}
