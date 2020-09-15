#pragma once

#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFBoolean.h>
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFNull.h>
#include <PDFWriter/PDFName.h>
#include <PDFWriter/PDFInteger.h>
#include <PDFWriter/PDFReal.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFStreamInput.h>
#include <PDFWriter/PDFSymbol.h>

template <typename T> class PDFnode;
template <typename T> class PDFnode<T*>;


class NodeVisitor{
public:
    virtual void visit(PDFnode<PDFBoolean> *) = 0;
    virtual void visit(PDFnode<PDFLiteralString> *) = 0;
    virtual void visit(PDFnode<PDFHexString> *) = 0;
    virtual void visit(PDFnode<PDFNull> *) = 0;
    virtual void visit(PDFnode<PDFName> *) = 0;
    virtual void visit(PDFnode<PDFInteger> *) = 0;
    virtual void visit(PDFnode<PDFReal> *) = 0;
    virtual void visit(PDFnode<PDFArray> *) = 0;
    virtual void visit(PDFnode<PDFDictionary> *) = 0;
    virtual void visit(PDFnode<PDFIndirectObjectReference> *) = 0;
    virtual void visit(PDFnode<PDFStreamInput> *) = 0;
    virtual void visit(PDFnode<PDFSymbol> *) = 0;
};

class TestVistor:  public NodeVisitor{
public:
    void visit(PDFnode<PDFBoolean> * node) override;
    void visit(PDFnode<PDFLiteralString> * node) override;
    void visit(PDFnode<PDFHexString> * node) override;
    void visit(PDFnode<PDFNull> * node) override;
    void visit(PDFnode<PDFName> * node) override;
    void visit(PDFnode<PDFInteger> * node) override;
    void visit(PDFnode<PDFReal> * node) override;
    void visit(PDFnode<PDFArray> * node) override;
    void visit(PDFnode<PDFDictionary> * node) override;
    void visit(PDFnode<PDFIndirectObjectReference> * node) override;
    void visit(PDFnode<PDFStreamInput> * node) override;
    void visit(PDFnode<PDFSymbol> * node) override;

};



//class PDFnode : public T {
//public:
//    void accept(NodeVisitor & visitor) {std::cout << "accept from PDFnodeTemplate"<<std::endl;}
//    PDFnode(const T&& inValue) : T(inValue){} // this constructo needs to match each type
//};

//////////////////
void TestVistor::visit(PDFnode<PDFBoolean> * node) {
    std::cout << "++++TestVistor - PDFBoolean" << std::endl;
}
void TestVistor::visit(PDFnode<PDFLiteralString> * node) {
    std::cout << "++++TestVistor - PDFLiteralString" << std::endl;
}
void TestVistor::visit(PDFnode<PDFHexString> * node) {
    std::cout << "++++TestVistor - PDFHexString" << std::endl;
}
void TestVistor::visit(PDFnode<PDFNull> * node) {
    std::cout << "++++TestVistor - PDFNull" << std::endl;
}
void TestVistor::visit(PDFnode<PDFName> * node) {
    std::cout << "++++TestVistor - PDFName" << std::endl;
}
void TestVistor::visit(PDFnode<PDFInteger> * node) {
    std::cout << "++++TestVistor - PDFInteger" << std::endl;
}
void TestVistor::visit(PDFnode<PDFReal> * node) {
    std::cout << "++++TestVistor - PDFReal" << std::endl;
}
void TestVistor::visit(PDFnode<PDFArray> * node) {
    std::cout << "++++TestVistor - PDFArray" << std::endl;
}
void TestVistor::visit(PDFnode<PDFDictionary> * node) {
    std::cout << "++++TestVistor - PDFDictionary" << std::endl;
}
void TestVistor::visit(PDFnode<PDFIndirectObjectReference> * node) {
    std::cout << "++++TestVistor - PDFIndirectObjectReference" << std::endl;
}
void TestVistor::visit(PDFnode<PDFStreamInput> * node) {
    std::cout << "++++TestVistor - PDFStreamInput" << std::endl;
}
void TestVistor::visit(PDFnode<PDFSymbol> * node) {
    std::cout << "++++TestVistor - PDFSymbol" << std::endl;
}
