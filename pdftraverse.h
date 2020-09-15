/// Decorator of PDFObject and other subclasses
/// The purpose is to extend the PDFWriter classes with accept method
///  which will be executed during each visit

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

// Option 1

// Template version would save on redefining each class
// but how do we trigger accept on node creation?
// In this case we need to only specialize each class which has a different
// parameters in the constructor
template <typename T>
class PDFnode : public T {
public:
    void accept(NodeVisitor & visitor) {
        std::cout << "accept from PDFnodeTemplate"<<std::endl;
        visitor.visit(this);
    }
    PDFnode(const T&& inValue) : T(inValue){} // this constructo needs to match each type
};

// partial specilization for T*
template <typename T>
class PDFnode<T*> : public T {
public:
    void accept(NodeVisitor & visitor) {
        std::cout << "accept from PDFnodeTemplate ptr"<<std::endl;
        visitor.visit(this);
    }
    PDFnode( T * inValue) : T(*inValue){} // this constructo needs to match each type
};




template <>
class PDFnode<PDFArray*> : public PDFArray {
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
class PDFnode<PDFDictionary*> : public PDFDictionary {
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
class PDFnode<PDFIndirectObjectReference*> : public PDFIndirectObjectReference {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate ptr - indirectRef"<<std::endl;}
    PDFnode(PDFIndirectObjectReference * inValue) : PDFIndirectObjectReference(inValue->mObjectID, inValue->mVersion) {
        accept();
    }
};

/// No need for PDFnode<PDFStreamInput*> specialization

//template <>
//class PDFnode<PDFStreamInput*> {
//public:
//    void accept() {std::cout << "accept from PDFnodeTemplate ptr - stream"<<std::endl;}
//    PDFnode(PDFStreamInput * inValue) {
////    PDFStreamInput(nullptr ,inValue->GetStreamContentStart()) {
////        PDFStreamInput(inValue->QueryStreamDictionary() ,inValue->GetStreamContentStart()) {
////        PDFDictionary * aaa = new PDFDictionary();
////        this->mDictionary = aaa;
//        accept();
//    }

////private:
////    PDFDictionary* mDictionary;
//};

// TODO: check other corner casses
// PDFStreamInput


