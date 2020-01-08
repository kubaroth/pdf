#include <iostream>
#include <string>


// shapes
#include <PDFWriter/PDFFormXObject.h>
#include <PDFWriter/XObjectContentContext.h>


// pdfwriter - PARSING example
#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/RefCountPtr.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFObjectCast.h>
#include <PDFWriter/PDFInteger.h>
#include <PDFWriter/PDFReal.h>
#include <PDFWriter/PDFName.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFStreamInput.h>
#include <PDFWriter/IByteReader.h>
#include <PDFWriter/EStatusCode.h>
#include <PDFWriter/PDFObject.h>  // ePDFObjectArray

// Page 134
// General graphics state w , J , j , M , d , ri , i , gs 4.7 156
// Special graphics state q , Q , cm 4.7 156
// Path construction m , l , c , v , y , h , re 4.9 163
// Path painting S , s , f , F , f* , B , B* , b , b* , n 4.10 167
// Clipping paths W , W* 4.11 172
// Text objects BT , ET 5.4 308
// Text state Tc , Tw , Tz , TL , Tf, Tr, Ts 5.2 302
// Text positioning Td , TD , Tm , T* 5.5 310
// Text showing Tj , TJ , ' , " 5.6 311
// Type 3 fonts d0 , d1 5.10 326
// Color CS , cs , SC , SCN , sc , scn , G , g , RG , rg , K , k 4.21 216
// Shading patterns sh 4.24 232
// Inline images BI , ID , EI 4.38 278
// XObjects Do 4.34 261
// Marked content MP , DP , BMC , BDC , EMC 9.8 584
// Compatibility BX , EX 3.20 95

using namespace std;
using namespace PDFHummus;


///////////////////  Parsing a page
 
void showPDFinfo(PDFParser& parser) {
    cout << "PDF Header level = " << parser.GetPDFLevel() << "\n";
    cout << "Number of objects in PDF = " << parser.GetObjectsCount() << "\n";
    cout << "Number of pages in PDF = " << parser.GetPagesCount() << "\n";
}

void showXObjectsPerPageInfo(PDFParser& parser,PDFObjectCastPtr<PDFDictionary> xobjects)
{
    RefCountPtr<PDFName> key;
    PDFObjectCastPtr<PDFIndirectObjectReference> value;
    MapIterator<PDFNameToPDFObjectMap> it = xobjects->GetIterator();
    while(it.MoveNext()) {
        key = it.GetKey();
        value = it.GetValue();

        cout << "XObject named " << key->GetValue().c_str() << " is object " << value->mObjectID << " of type ";

        PDFObjectCastPtr<PDFStreamInput> xobject(parser.ParseNewObject(value->mObjectID));
        PDFObjectCastPtr<PDFDictionary> xobjectDictionary(xobject->QueryStreamDictionary());
        PDFObjectCastPtr<PDFName> typeOfXObject = xobjectDictionary->QueryDirectObject("Subtype");

        cout << typeOfXObject->GetValue().c_str() << "\n";
    }
}

void checkXObjectRef(PDFParser& parser,RefCountPtr<PDFDictionary> page)
{
    PDFObjectCastPtr<PDFDictionary> resources(parser.QueryDictionaryObject(page.GetPtr(),"Resources"));
    if(!resources){
        wcout << "No XObject in this page\n";
        return;
    }

    PDFObjectCastPtr<PDFDictionary> xobjects(parser.QueryDictionaryObject(resources.GetPtr(),"XObject"));
    if(!xobjects) {
        wcout << "No XObject in this page\n";
        return;
    }

    cout << "Displaying XObjects information for this page:\n";
    showXObjectsPerPageInfo(parser,xobjects);
}

void showContentStream(PDFStreamInput* inStream,IByteReaderWithPosition* inPDFStream,PDFParser& inParser)
{
    IByteReader* streamReader = inParser.CreateInputStreamReader(inStream);
    Byte buffer[1000];
    if(streamReader) {
        inPDFStream->SetPosition(inStream->GetStreamContentStart());
        while(streamReader->NotEnded()) {
            LongBufferSizeType readAmount = streamReader->Read(buffer,1000);
            cout.write((const char*)buffer,readAmount);
        }
        cout << "\n";
    }
    else {
        cout << "Unable to read content stream\n";
    }

    delete streamReader;
}

void showPageContent(PDFParser& parser, RefCountPtr<PDFObject> contents, InputFile& pdfFile)
{
    if(contents->GetType() == PDFObject::ePDFObjectArray) {
        PDFObjectCastPtr<PDFIndirectObjectReference> streamReferences;
        SingleValueContainerIterator<PDFObjectVector> itContents = ((PDFArray*)contents.GetPtr())->GetIterator();
        // array of streams
        while(itContents.MoveNext()) {
            streamReferences = itContents.GetItem();
            PDFObjectCastPtr<PDFStreamInput> stream = parser.ParseNewObject(streamReferences->mObjectID);
            showContentStream(stream.GetPtr(),pdfFile.GetInputStream(),parser);
        }
    }
    else {
        // stream
        showContentStream((PDFStreamInput*)contents.GetPtr(),pdfFile.GetInputStream(),parser);
    }
}


void showPagesInfo(PDFParser& parser, InputFile& pdfFile, EStatusCode status)
{
    for(unsigned long i=0; i < parser.GetPagesCount() && eSuccess == status; ++i) {
        cout << "Showing info for page " << i << ":\n";

        // Parse page object
        RefCountPtr<PDFDictionary> page(parser.ParsePage(i));

        // check XObject referenences
        checkXObjectRef(parser,page);

        // show page content
        RefCountPtr<PDFObject> contents(parser.QueryDictionaryObject(page.GetPtr(),"Contents"));
        if(!contents) {
            cout << "No contents for this page\n";
            continue;
        }

        // content may be array or single
        cout << "Showing content of page:\n";
        showPageContent(parser,contents,pdfFile);

        cout << "End page content\n";
    }
}




void parseDict(PDFParser &parser, PDFDictionary *obj){
    auto it = obj->GetIterator();
    while(it.MoveNext()) {
        PDFName* name = it.GetKey();
        cout << "__" << name->GetValue();
        PDFObject* obj = it.GetValue();
        cout << " __ " << obj->GetType()<<endl;

    }
}

void testPage(PDFParser &parser, InputFile &pdfFile){
    cout << parser.GetPagesCount() <<endl;
    RefCountPtr<PDFDictionary> page(parser.ParsePage(1));

    // check XObject referenences
    checkXObjectRef(parser,page);

    // show page content
    RefCountPtr<PDFObject> contents(parser.QueryDictionaryObject(page.GetPtr(),"Contents"));
    if(!contents) {
        cout << "No contents for this page\n";
    }
    // cout << "Showing content of page:\n";
    // showPageContent(parser,contents,pdfFile);
    // cout << "End page content\n";

    // if (contents->GetType() == PDFObject::ePDFObjectStream){
    //     (PDFStreamInput*)contents.GetPtr(),pdfFile.GetInputStream(),parser)
    // }

    // TODO - move into the recursive function
    auto it = page->GetIterator();
    while(it.MoveNext()) {
        PDFName* name = it.GetKey();
        // cout << name->GetValue() << endl;
        PDFObject* obj = it.GetValue();
        // cout<<obj->GetType()<<endl;

        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            cout << "ePDFObjectBoolean" <<endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            cout << "ePDFObjectLiteralString" << endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectHexString){
            cout << "ePDFObjectHexString" << endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectNull){
            cout << "ePDFObjectNull" << endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectName){
            cout << "ePDFObjectName" << endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectInteger){
            cout << "ePDFObjectInteger" << endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectReal){
            cout << "ePDFObjectReal" << endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectArray){
            cout << "ePDFObjectArray" << endl;
        }
        if (obj->GetType() == PDFObject::ePDFObjectDictionary){
          cout << "ePDFObjectDictionary" << endl;                   // 8
        }
        if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            cout << "ePDFObjectIndirectObjectReference" << endl;
            PDFIndirectObjectReference  *oo = (PDFIndirectObjectReference*)obj;
            
            PDFObject* ob = parser.ParseNewObject(oo->mObjectID);
            cout << "___" << ob->GetType() <<endl;
            if (ob->GetType() == PDFObject::ePDFObjectDictionary){
              parseDict(parser, (PDFDictionary*)ob);
            }
        }
        if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            cout << "ePDFObjectStream" << endl;
        }
        

    }

}


int readin(){
    string path = "../dsohowto.pdf";
    cout << path <<endl;

    PDFParser parser;
    InputFile pdfFile;

    EStatusCode status = pdfFile.OpenFile(path);
    if(status != eSuccess) {
        return status;
    }

    status = parser.StartPDFParsing(pdfFile.GetInputStream());
    if(status != eSuccess) {
        return status;
    }

    // showPDFinfo(parser); // Just wcout some info (no iteration)

    // showPagesInfo(parser,pdfFile,status);

    testPage(parser, pdfFile);

}

int main(){
    readin();
    return 0;
}
