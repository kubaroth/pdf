#include "visitor_test.h"

// NOTE: to break circular dependency import inside definition
#include "pdftraverse.h"


void TestVisitor::visit(PDFnode<PDFBoolean> * node) {
    std::cout << "++++TestVistor - PDFBoolean" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFLiteralString> * node) {
    std::cout << "++++TestVistor - PDFLiteralString" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFHexString> * node) {
    std::cout << "++++TestVistor - PDFHexString" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFNull> * node) {
    std::cout << "++++TestVistor - PDFNull" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFName> * node) {
    std::cout << "++++TestVistor - PDFName" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFInteger> * node) {
    std::cout << "++++TestVistor - PDFInteger" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFReal> * node) {
    std::cout << "++++TestVistor - PDFReal" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFArray> * node) {
    std::cout << "++++TestVistor - PDFArray" << std::endl;
}

void TestVisitor::visit(PDFnode<PDFDictionary> * node) {
    using namespace  std;
    std::cout << "++++TestVistor - PDFDictionary" << std::endl;

    auto it = node->GetIterator();

    while(it.MoveNext()) {
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();
        cout << "name " << name->GetValue() << endl;

        if (obj->GetType() == PDFObject::ePDFObjectName){
            cout << "ObjectName : " <<  obj->scPDFObjectTypeLabel(obj->GetType()) << " : " << ((PDFName*)obj)->GetValue() <<endl;
        }
    }
}

void TestVisitor::visit(PDFnode<PDFIndirectObjectReference> * node) {
    std::cout << "++++TestVistor - PDFIndirectObjectReference" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFStreamInput> * node) {
    std::cout << "++++TestVistor - PDFStreamInput" << std::endl;
}
void TestVisitor::visit(PDFnode<PDFSymbol> * node) {
    std::cout << "++++TestVistor - PDFSymbol" << std::endl;
}
