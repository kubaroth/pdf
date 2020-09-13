#include "text_visitor.h"

// NOTE: to break circular dependency import inside definition
#include "pdftraverse.h"

#include <PDFWriter/PDFObjectCast.h>

#define LOG 0

using namespace std;

void TextVisitor::visit(PDFnode<PDFBoolean> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFBoolean" << std::endl;
}
void TextVisitor::visit(PDFnode<PDFLiteralString> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFLiteralString" << std::endl;
}
void TextVisitor::visit(PDFnode<PDFHexString> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFHexString" << std::endl;
}
void TextVisitor::visit(PDFnode<PDFNull> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFNull" << std::endl;
}
void TextVisitor::visit(PDFnode<PDFName> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFName" << std::endl;
}
void TextVisitor::visit(PDFnode<PDFInteger> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFInteger" << std::endl;
}
void TextVisitor::visit(PDFnode<PDFReal> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFReal" << std::endl;
}
void TextVisitor::visit(PDFnode<PDFArray> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFArray" << std::endl;


    SingleValueContainerIterator<PDFObjectVector> it = node->GetIterator();
    int length = node->GetLength();
    int start = 0;
    do {
        PDFObject* obj = it.GetItem();

        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            PDFnode<PDFBoolean> obj_adapt((PDFBoolean*)obj);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            PDFnode<PDFLiteralString> obj_adapt((PDFLiteralString*)obj);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            PDFnode<PDFHexString> obj_adapt((PDFHexString*)obj);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            PDFnode<PDFNull> obj_adapt((PDFNull*)obj);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            PDFnode<PDFName> obj_adapt((PDFName*)obj);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            PDFnode<PDFInteger> obj_adapt((PDFInteger*)obj);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            PDFnode<PDFReal> obj_adapt((PDFReal*)obj);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            PDFnode<PDFArray> obj_adapt((PDFArray*)obj);
            visit(&obj_adapt);

        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // if (LOG>=3) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            PDFnode<PDFDictionary> obj_adapt((PDFDictionary*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            // if (LOG>=3) cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
            PDFnode<PDFIndirectObjectReference> obj_adapt((PDFIndirectObjectReference*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            PDFnode<PDFStreamInput> obj_adapt((PDFStreamInput*)obj);
            visit(&obj_adapt);
        }
        else {
            if (LOG>=3) cout << std::string(depth, '.') << "array ELSE " << obj->GetType() << endl; //IndirectRef
        }
        start++;
    } while(it.MoveNext());
}

void TextVisitor::visit(PDFnode<PDFDictionary> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFDictionary" << std::endl;

    auto it = node->GetIterator();

    while(it.MoveNext()) {
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();
        cout << "name " << name->GetValue() << endl;

        if (obj->GetType() == PDFObject::ePDFObjectName){
            cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFName*)obj)->GetValue() <<endl;
        }
    }
    it = node->GetIterator();

    do {
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();

        if (LOG>=1) cout << "(parsePDFDictionary) - name " << name->GetValue() << endl;
        if (name->GetValue().compare("Parent") == 0) return;  // avoid cycles
        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            PDFnode<PDFBoolean> obj_adapt((PDFBoolean*)obj);
            //parseObjectBoolean(parser, &obj_adapt, depth);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            PDFnode<PDFLiteralString> obj_adapt((PDFLiteralString*)obj);
            //parseObjectLiteralStr(parser, &obj_adapt, depth);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            PDFnode<PDFHexString> obj_adapt((PDFHexString*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            // no need to visit
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            PDFnode<PDFName> obj_adapt((PDFName*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            PDFnode<PDFInteger> obj_adapt((PDFInteger*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            PDFnode<PDFReal> obj_adapt((PDFReal*)obj);
            visit(&obj_adapt);
        }

        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            PDFnode<PDFArray> obj_adapt((PDFArray*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            if (LOG>=2) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            PDFnode<PDFDictionary> obj_adapt((PDFDictionary*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            if (LOG>=2) cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
            PDFnode<PDFIndirectObjectReference> obj_adapt((PDFIndirectObjectReference*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            PDFnode<PDFStreamInput> obj_adapt((PDFStreamInput*)obj);
            visit(&obj_adapt);
        }
        else {
            if (LOG>=2) cout << std::string(depth, '.') << "UNKNOWN" <<endl;
        }
    } while(it.MoveNext());
}

void TextVisitor::visit(PDFnode<PDFIndirectObjectReference> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFIndirectObjectReference" << std::endl;

    PDFObject* obj = parser.ParseNewObject(node->mObjectID); // TODO: convert to RefCountPtr<PDFObject>

    if (obj->GetType() == PDFObject::ePDFObjectBoolean){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
        PDFnode<PDFBoolean> obj_adapt((PDFBoolean*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
        PDFnode<PDFLiteralString> obj_adapt((PDFLiteralString*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectHexString){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
        PDFnode<PDFHexString> obj_adapt((PDFHexString*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectNull){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
        PDFnode<PDFNull> obj_adapt((PDFNull*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectName){
        // if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectName" << endl;
        PDFnode<PDFName> obj_adapt((PDFName*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectInteger){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
        PDFnode<PDFInteger> obj_adapt((PDFInteger*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectReal){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
        PDFnode<PDFReal> obj_adapt((PDFReal*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectArray){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
        PDFnode<PDFArray> obj_adapt((PDFArray*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
        //if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectDictionary" << endl;
        PDFnode<PDFDictionary> obj_adapt((PDFDictionary*)obj);
        visit(&obj_adapt);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectStream){
        // if (LOG>=3) cout << std::string(depth, '.') << "ePDFObjectStream" << endl; // parser.GetPageObjectID(obj)
        PDFnode<PDFStreamInput> obj_adapt((PDFStreamInput*)obj);
        visit(&obj_adapt);
    }
    else {
        if (LOG>=3) cout << std::string(depth, '.') << "UNKNOWN" << endl;
    }
}
void TextVisitor::visit(PDFnode<PDFStreamInput> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFStreamInput" << std::endl;

    PDFDictionary* obj1 = node->QueryStreamDictionary();
    if (LOG>=2) cout << std::string(depth, '.') << "streamDictionary " << endl;

    PDFnode<PDFDictionary> obj_adapt((PDFDictionary*)obj1);
    visit(&obj_adapt);


    if (LOG>=2) cout << "content type: " << node->scPDFObjectTypeLabel(node->GetType()) << endl;
    PDFStreamInput* inStream = (PDFStreamInput*)node;

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
                                if ( lookup_bfchars.find(_char) != lookup_bfchars.end()){
                                    new_text += lookup_bfchars[_char];
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
                            if ( lookup_bfchars.find(_char) != lookup_bfchars.end()){
                                text += lookup_bfchars[_char];
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

        // Option 2 : parse HexStrings
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            if (LOG>=2) cout << "hex : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFHexString*)obj)->GetValue()  << "  +++  " << pp->DecodeHexString(((PDFHexString*)obj)->GetValue()) << endl;
            auto hs = (PDFHexString*)obj;
            string value = hs->GetValue();

            if ((symbol_next.second.compare("Td") == 0) ||
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

                    if ( lookup_bfchars.find(key) != lookup_bfchars.end()){
                        string_value = lookup_bfchars[key];
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
                        if ( lookup_bfchars.find(_char) != lookup_bfchars.end()){
                            text += lookup_bfchars[_char];
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
                if (LOG>=1)cout << "CMapType" <<endl;
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
            //parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
            PDFnode<PDFDictionary> obj_adapt((PDFDictionary*)obj);
            visit(&obj_adapt);

        }
        else if (obj->GetType() == PDFObject::ePDFObjectSymbol){
            if (LOG>=2) cout << "Symbol: " << obj->scPDFObjectTypeLabel(obj->GetType()) << ((PDFSymbol*)obj)->GetValue() <<  endl;
        }
        else{
            if (LOG>=2) cout << "other " << obj->scPDFObjectTypeLabel(obj->GetType()) << " : " <<  endl;
        }
    }
}
void TextVisitor::visit(PDFnode<PDFSymbol> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFSymbol" << std::endl;
}
