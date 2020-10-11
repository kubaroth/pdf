#include "visitor_test.h"

// NOTE: to break circular dependency import inside definition
#include "pdftraverse.h"

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
