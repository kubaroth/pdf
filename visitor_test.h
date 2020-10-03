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

// TODO: are we gonna store data inside visitor?
class TestVisitor:  public NodeVisitor{
public:
    void visit(PDFnode<PDFBoolean> * node) override{
        std::cout << "++++TestVistor - PDFBoolean" << std::endl;
    }
    void visit(PDFnode<PDFLiteralString> * node) override{
        std::cout << "++++TestVistor - PDFLiteralString" << std::endl;
    }
    void visit(PDFnode<PDFHexString> * node) override{
        std::cout << "++++TestVistor - PDFHexString" << std::endl;
    }
    void visit(PDFnode<PDFNull> * node) override{
        std::cout << "++++TestVistor - PDFNull" << std::endl;
    }
    void visit(PDFnode<PDFName> * node) override{
        std::cout << "++++TestVistor - PDFName" << std::endl;
    }
    void visit(PDFnode<PDFInteger> * node) override{
        std::cout << "++++TestVistor - PDFInteger" << std::endl;
    }
    void visit(PDFnode<PDFReal> * node) override{
        std::cout << "++++TestVistor - PDFReal" << std::endl;
    }
    void visit(PDFnode<PDFArray> * node) override{
        std::cout << "++++TestVistor - PDFArray" << std::endl;
    }
    void visit(PDFnode<PDFDictionary> * node) override{
        std::cout << "++++TestVistor - PDFDictionary" << std::endl;
    }
    void visit(PDFnode<PDFIndirectObjectReference> * node) override{
        std::cout << "++++TestVistor - PDFIndirectObjectReference" << std::endl;
    }
    void visit(PDFnode<PDFStreamInput> * node) override{
        std::cout << "++++TestVistor - PDFStreamInput" << std::endl;
    }
    void visit(PDFnode<PDFSymbol> * node) override{
        std::cout << "++++TestVistor - PDFSymbol" << std::endl;
    }

};




