#include "resources_visitor.h"

// NOTE: to break circular dependency import inside definition
#include "pdftraverse.h"

#include <PDFWriter/PDFObjectCast.h>

#define LOG 0

using namespace std;

void ResourcesVisitor::visit(PDFnode<PDFBoolean> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFBoolean" << std::endl;
}
void ResourcesVisitor::visit(PDFnode<PDFLiteralString> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFLiteralString" << std::endl;
}
void ResourcesVisitor::visit(PDFnode<PDFHexString> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFHexString" << std::endl;
}
void ResourcesVisitor::visit(PDFnode<PDFNull> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFNull" << std::endl;
}
void ResourcesVisitor::visit(PDFnode<PDFName> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFName" << std::endl;
}
void ResourcesVisitor::visit(PDFnode<PDFInteger> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFInteger" << std::endl;
}
void ResourcesVisitor::visit(PDFnode<PDFReal> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFReal" << std::endl;
}
void ResourcesVisitor::visit(PDFnode<PDFArray> * node) {
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

void ResourcesVisitor::visit(PDFnode<PDFDictionary> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFDictionary" << std::endl;

    auto it = node->GetIterator();

    while(it.MoveNext()) {
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();
        if (LOG>=1) cout << "name " << name->GetValue() << endl;

        if (obj->GetType() == PDFObject::ePDFObjectName){
            if (LOG>=1) cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFName*)obj)->GetValue() <<endl;
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
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            if (LOG>=2) cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            PDFnode<PDFLiteralString> obj_adapt((PDFLiteralString*)obj);
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

            // Option 2 (symbol lookup table)
            if (name->GetValue().compare(resource_differences) == 0) {
                add_table((PDFArray*)obj);
            }
            PDFnode<PDFArray> obj_adapt((PDFArray*)obj);
            visit(&obj_adapt);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            if (LOG>=2) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            PDFnode<PDFDictionary> obj_adapt((PDFDictionary*)obj);
            //parsePDFDictionary(parser, &obj_adapt, depth);
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

void ResourcesVisitor::visit(PDFnode<PDFIndirectObjectReference> * node) {
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
void ResourcesVisitor::visit(PDFnode<PDFStreamInput> * node) {
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
        // Option 1 (extracting text using stream - see TextVisitor)
        else if (obj->GetType() == PDFObject::ePDFObjectArray){

        }

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

            if (LOG>=2) cout << "symbol_prev: " << symbol_next.first << "  symbol curr: " << symbol_next.second <<endl;
        }

        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // if (LOG>=3) cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
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
void ResourcesVisitor::visit(PDFnode<PDFSymbol> * node) {
    if (LOG>=3) std::cout << "TestVistor - PDFSymbol" << std::endl;
}
