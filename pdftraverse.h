#pragma once

#include <string>

//#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFLiteralString.h>

// Option 1

// Template version would save on redefining each class
// but how do we trigger accept on node creation?
// In this case we need to only specialize each class which has a different
// parameters in the constructor
template <typename T>
class PDFnode : public T {
//    T * data_;
public:
    void accept() {std::cout << "accept from PDFnodeTemplate"<<std::endl;}
    PDFnode(const T&& inValue) : T(inValue){ accept();} // this constructo needs to match each type
};

// partial specilization for T*
template <typename T>
class PDFnode<T*> : public T {
//    T * data_;
public:
    void accept() {std::cout << "accept from PDFnodeTemplate"<<std::endl;}
    PDFnode( T * inValue) : T(*inValue){ accept();} // this constructo needs to match each type
};

// Option 2

// Here we would need to subclass and redefine _each_ PDFWriter* class
class PDFLiteralStringNode : public PDFLiteralString{
public:
    PDFLiteralStringNode(const std::string& inValue) : PDFLiteralString(inValue){
        accept();
    }

    void accept() {std::cout << "accept from PDFLiteralStringNode"<<std::endl;}

};
