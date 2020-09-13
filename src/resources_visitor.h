#pragma once

#include <iostream>
#include <memory>
#include <cassert>

#include "node_visitor.h"
#include "textdata.h"

// TODO: are we gonna store data inside visitor?
class ResourcesVisitor:  public NodeVisitor{
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

    std::map<char, std::string> lookup_bfchars; // bfchars

    // text data from a stream converted to text
    // The data to be pulated is moved from externally initilized variable
    std::unique_ptr<TextData> m_text_data;

    ~ResourcesVisitor(){}
    explicit ResourcesVisitor(PDFParser& parser) : parser(parser) {
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


    /// Extracting the (key,value) mapping from "Differences" encoding dictionary
    /// Specifies  complete character encoding for this font.
    void add_table(PDFArray * array){

        SingleValueContainerIterator<PDFObjectVector> it = array->GetIterator();
        char index = 0;  // keep track when the pair needs to be saved

        while (it.MoveNext()) {
            index++;
            PDFObject* obj = it.GetItem();
            if (obj->GetType() == PDFObject::ePDFObjectName){
                std::string name = ((PDFName*)obj)->GetValue();
                m_lookup_pair.second = name;
            }
            else if (obj->GetType() == PDFObject::ePDFObjectInteger){
                char key_index = ((PDFInteger*)obj)->GetValue();
                m_lookup_pair.first = key_index;
            }
            else{
                assert(("this should not happend we need to add more type handling here", obj==nullptr));
            }
            // update lookup table
            if (index % 2 == 0){
                lookup_bfchars[m_lookup_pair.first] = m_lookup_pair.second;
            }
        }
    }

    /// Extracting the (key,value) mapping if the bfchar symbol is encountered
    /// add (index, character) pair to the lookup table
    void add_bfchars(std::string _s){

        if (m_bfchars_index%2 == 0){
            char key_char;
            if (_s.size() == 1)
                key_char= _s[0]; // NOTE: here key_string is single char
            else
                key_char= _s[1];
            m_symbol_pair = {key_char, "a" , m_bfchars_index};
        }
        else {
            // only add if there are consecutive indices
            if ( std::get<2>(m_symbol_pair) + 1  == m_bfchars_index){

                // handling ligatures ff, fi, fl (TODO: there are more...)
                if ((_s[0] == '\373') && (_s[1] == '\000')) {
                    // todo handle keys as \000\002 instead \002
                    lookup_bfchars[std::get<0>(m_symbol_pair)] = "fi"; // ff -> can't use 'ff' as we use char
                }
                else if ((_s[0] == '\373') && (_s[1] == '\001')) {
                    lookup_bfchars[std::get<0>(m_symbol_pair)] = "fi"; // fi
                }
                else if ((_s[0] == '\373') && (_s[1] == '\002')) {
                    lookup_bfchars[std::get<0>(m_symbol_pair)] = "fl"; // fl
                }
                // handling regular characters
                else {
                    std::string char_string(1, _s[1]);
                    lookup_bfchars[std::get<0>(m_symbol_pair)] = char_string;
                }
            }
        }

        m_bfchars_index++;
    }
};




