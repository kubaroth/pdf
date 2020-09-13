#pragma once

#include <iostream>
#include <memory>
#include <cassert>

#include "node_visitor.h"
#include "textdata.h"

class TextVisitor : public NodeVisitor {
private:
    PDFParser& parser;
    int depth = 0;

    int m_bfchars_index = 0;
    const std::string resource_differences = "Differences";

    std::tuple<char, std::string, int > m_symbol_pair;

    // For ObjectLiteralString part of ObjArray words can be broken at the end of the line
    // having "-" at the start/end of the string.
    // Each part being a separate StringLiteral object.
    // While adding " " character we need to take into accont "-" and skip adding the " "
    std::pair<std::string, std::string> m_prev_word = {"", ""};  // original unprocessed text to search for dashes
    std::pair<char, std::string> m_lookup_pair; // The same purpose as 'm_symbol_pair' TODO: merge them together
    bool m_add_space = false;  // if previous Object is Integer for Array and current is LiteralString enable
    
public:
    TextVisitor();
    std::map<char, std::string> lookup_bfchars; // bfchars

    // text data from a stream converted to text
    // The data to be pulated is moved from externally initilized variable
    std::unique_ptr<TextData> m_text_data;

    ~TextVisitor(){}
    explicit TextVisitor(PDFParser& parser) : parser(parser) {
        m_text_data = std::move(std::unique_ptr<TextData>(new TextData));
    }
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
