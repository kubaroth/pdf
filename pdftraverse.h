/// Decorator of PDFObject and other subclasses
/// The purpose is to extend the PDFWriter classes with accept method
///  which will be executed during each visit

#pragma once

#include <string>

//#include <PDFWriter/PDFObject.h>
//#include <PDFWriter/PDFLiteralString.h>
//#include <PDFWriter/PDFDictionary.h>

// Option 1

// Template version would save on redefining each class
// but how do we trigger accept on node creation?
// In this case we need to only specialize each class which has a different
// parameters in the constructor
template <typename T>
class PDFnode : public T {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate"<<std::endl;}
    PDFnode(const T&& inValue) : T(inValue){ accept();} // this constructo needs to match each type
};

// partial specilization for T*
template <typename T>
class PDFnode<T*> : public T {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate ptr"<<std::endl;}
    PDFnode( T * inValue) : T(*inValue){ accept();} // this constructo needs to match each type
};

template <>
class PDFnode<PDFArray*> : public PDFArray {
public:
    void accept() {std::cout << "accept from PDFnodeTemplate ptr - array"<<std::endl;}
    PDFnode( PDFArray * inValue){
        std::cout  << inValue->GetLength() << std::endl;
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
// TODO: check other corner casses
// PDFIndirectObjectReference
// PDFStreamInput
