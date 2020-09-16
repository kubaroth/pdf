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
    PDFnode(const T& inValue, NodeVisitor * visitor = nullptr) : T((inValue)){ /* transfer ownership */
        if (visitor) accept(*visitor);

    }
    // construc from pointer
    PDFnode(const T* inValue, NodeVisitor * visitor = nullptr) : T(*inValue){ /* transfer ownership */
        if (visitor) accept(*visitor);
    }
    PDFnode (PDFnode const&) = delete;
    PDFnode& operator=(PDFnode const&) = delete;
};


/// NOTE: !!!!
///  this is still required as: PDFnode<PDFArray> obj_adapt((PDFArray*)obj);  // crash and subsequent

template <>
class PDFnode<PDFArray> : public PDFArray {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate ptr - array"<<std::endl;}
    PDFnode( PDFArray * inValue){
        auto it = inValue->GetIterator();
        auto i = 0;
        for(it.MoveNext(); !it.IsFinished(); it.MoveNext()){
            // std::cout  << i << " " << it.GetItem()->GetType() << std::endl;
            AppendObject(std::move(it.GetItem()));  //TODO: is move the right thing here?
            i++;
        }
        // TODO: should we cleanup input array at this point?
        accept();
    }
};

template <>
class PDFnode<PDFDictionary> : public PDFDictionary {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate ptr - dictionary"<<std::endl;}
    PDFnode( PDFDictionary * inValue){
        auto it = inValue->GetIterator();
        for(it.MoveNext(); !it.IsFinished(); it.MoveNext()){
            auto key = it.GetKey();
            auto value = it.GetValue();
            Insert(key, value);
        }
        // TODO: should we cleanup input array at this point?
        accept();
    }
};

template <>
class PDFnode<PDFIndirectObjectReference> : public PDFIndirectObjectReference {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate ptr - indirectRef"<<std::endl;}
    PDFnode(PDFIndirectObjectReference * inValue) : PDFIndirectObjectReference(inValue->mObjectID, inValue->mVersion) {
        accept();
    }
};

template <>
class PDFnode<PDFStreamInput> : public PDFStreamInput {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate ptr - steamRef"<<std::endl;}
    PDFnode(PDFStreamInput * inValue) : PDFStreamInput(inValue->QueryStreamDictionary(), inValue->GetStreamContentStart()) {
        accept();
    }
};
