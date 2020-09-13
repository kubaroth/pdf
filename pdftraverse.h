#pragma once

#include <string>

//#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFLiteralString.h>


// Template version would save on redefining each class
// but how do we trigger accept on node creation?
// In this case we need to only specialize each class which has a different
// parameters in the constructor
template <typename T>
class PDFnode {
    T * data_;
public:
    void accept() {std::cout << "accept from PDFnodeTemplate"<<std::endl;}
    PDFnode(T * data) : data_(data){ accept();}

};

// Here we would need to subclass and redefine each PDFWriter class
class PDFLiteralStringNode : PDFLiteralString{
public:
    PDFLiteralStringNode(const std::string& inValue) : PDFLiteralString(inValue){
        accept();
    }

    void accept() {std::cout << "accept from PDFLiteralStringNode"<<std::endl;}

};
