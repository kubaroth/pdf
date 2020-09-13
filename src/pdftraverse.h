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

#include "resources_visitor.h"
#include "text_visitor.h"

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
    // construct from another object
    PDFnode(const T& inValue, NodeVisitor * visitor = nullptr) : T((inValue)){ /* transfer ownership */
        //if (visitor) accept(*visitor);

    }
    // construc from pointer
    PDFnode(const T* inValue, NodeVisitor * visitor = nullptr) : T(*inValue){ /* transfer ownership */
        //if (visitor) accept(*visitor);
    }
    PDFnode (PDFnode const&) = delete;
    PDFnode& operator=(PDFnode const&) = delete;
};


/// NOTE: !!!!
///  this is still required as: PDFnode<PDFArray> obj_adapt((PDFArray*)obj);  // crash and subsequent

template <>
class PDFnode<PDFArray> : public PDFArray {
public:
    void accept(NodeVisitor & visitor) {
        std::cout << "accept from PDFnodeTemplate ptr - array"<<std::endl;
        visitor.visit(this);
    }
    PDFnode( PDFArray * inValue, NodeVisitor * visitor = nullptr){
        auto it = inValue->GetIterator();
        auto i = 0;
        for(it.MoveNext(); !it.IsFinished(); it.MoveNext()){
            // std::cout  << i << " " << it.GetItem()->GetType() << std::endl;
            AppendObject(std::move(it.GetItem()));  //TODO: is move the right thing here?
            i++;
        }
        // TODO: should we cleanup input array at this point?
        //if (visitor) accept(*visitor);
    }
};

template <>
class PDFnode<PDFDictionary> : public PDFDictionary {
public:
    void accept(NodeVisitor & visitor) {
        std::cout << "accept from PDFnodeTemplate ptr - dictionary"<<std::endl;
        visitor.visit(this);
    }
    PDFnode( PDFDictionary * inValue, NodeVisitor * visitor = nullptr){
        auto it = inValue->GetIterator();
        for(it.MoveNext(); !it.IsFinished(); it.MoveNext()){
            auto key = it.GetKey();
            auto value = it.GetValue();
            Insert(key, value);
        }
        // TODO: should we cleanup input array at this point?
        //if (visitor) accept(*visitor);
    }
};

template <>
class PDFnode<PDFIndirectObjectReference> : public PDFIndirectObjectReference {
public:
    void accept(NodeVisitor & visitor) {
        std::cout << "accept from PDFnodeTemplate ptr - indirectRef"<<std::endl;
        visitor.visit(this);
    }
    PDFnode(PDFIndirectObjectReference * inValue, NodeVisitor * visitor = nullptr) :
        PDFIndirectObjectReference(inValue->mObjectID,
                                   inValue->mVersion) {
        //if (visitor) accept(*visitor);
    }
};

template <>
class PDFnode<PDFStreamInput> : public PDFStreamInput {
public:
    void accept(NodeVisitor & visitor) {
        std::cout << "accept from PDFnodeTemplate ptr - streamInput"<<std::endl;
        visitor.visit(this);
    }
    PDFnode(PDFStreamInput * inValue, NodeVisitor * visitor = nullptr) :
        PDFStreamInput(inValue->QueryStreamDictionary(),
                       inValue->GetStreamContentStart()) {
        //if (visitor) accept(*visitor);
    }
};
