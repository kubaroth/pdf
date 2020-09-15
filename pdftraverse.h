/// Adaptor of PDFObject and other subclasses
/// The purpose is to extend the PDFWriter classes with accept method
/// which will be executed during each visit

#pragma once

#include <string>

//#include <PDFWriter/PDFObject.h>
//#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFStreamInput.h>

#include <PDFWriter/PDFLiteralString.h>

#include "visitor_test.h"

// This is an adaptor which moves the type into this object
// either by value or pointer and implments accept() method
// which accepts visitor

// TODO: define second template parameter which defaults (c++17?) on Visitor type
//       This would be require if we want to pass Visitor as a second argument to ctor
template <typename T>
class PDFnode : public T {
public:
    void accept(NodeVisitor & visitor) {
        std::cout << "accept from PDFnodeTemplate"<<std::endl;
        visitor.visit(this);
    }
    // construc from another object
    PDFnode(const T& inValue, NodeVisitor * visitor = nullptr) : T(std::move(inValue)){ /* transfer ownership */
        if (visitor) accept(*visitor);

    }
    // construc from pointer
    PDFnode(const T* inValue, NodeVisitor * visitor = nullptr) : T(std::move(*inValue)){ /* transfer ownership */
        if (visitor) accept(*visitor);
    }
    PDFnode (PDFnode const&) = delete;
    PDFnode& operator=(PDFnode const&) = delete;
};


