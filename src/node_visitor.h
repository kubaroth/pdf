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
#include <PDFWriter/PDFParser.h>

template <typename T> class PDFnode;
template <typename T> class PDFnode<T*>;

class NodeVisitor{
public:
    virtual ~NodeVisitor(){}
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
