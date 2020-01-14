//(setq-default shell-file-name "/bin/bash")

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
// basic
#include <PDFWriter/PDFWriter.h>
#include <PDFWriter/PDFPage.h>
#include <PDFWriter/PageContentContext.h>
#include <PDFWriter/PDFFormXObject.h>
#include <PDFWriter/ResourcesDictionary.h>

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

#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/IDocumentContextExtender.h>  // dictionary
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFBoolean.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFNull.h>
#include <PDFWriter/PDFSymbol.h>
#include <PDFWriter/PDFStreamInput.h>


// svg
//#include <QDebug>
//#include <QtSvg/QtSvg>
//#include <QtSvg/QGraphicsSvgItem>

// xml
// #include "tinyxml2.h"

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

//static const string scBasePath =  "/u/kr/SRC/hummuspdf_samples/BasicImageAndText/Materials";
static const string scSystemFontsPath = "/usr/share/fonts/gnu-free/";


void basic(){
    PDFWriter pdfWriter;
    EStatusCode status;
    
    cout << "Testing " <<endl;

  do
  {
    status = pdfWriter.StartPDF("./test.pdf",ePDFVersion13);
    if(status != eSuccess)
      break;

    // Create a new page
    PDFPage* pdfPage = new PDFPage();
    pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

    // Create an image object from image
    PDFFormXObject* image = pdfWriter.CreateFormXObjectFromJPGFile("/u/kr/SRC/hummuspdf_samples/BasicImageAndText/Materials/SanAntonioPass.JPG");
    if(!image)
    {
      status = eFailure;
      break;
    }

    // Create a content context for the page
    PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);
        
    // Place the image in the center of the page
    pageContentContext->q();  // Save the current graphics state on the graphics state stack
    pageContentContext->cm(0.4,0,0,0.4,57.5,241); // Modify the current transformation matrix
    pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(image->GetObjectID()));
    pageContentContext->Q();  // Restore the graphics state by removing the most recently saved state from the stack and making it the current state

    // Image object can be deleted right after i was used
    delete image;

    // Create a title text over the image
    PDFUsedFont* arialTTF = pdfWriter.GetFontForFile(scSystemFontsPath + "FreeMono.ttf");
    if(!arialTTF)
    {
      status = eFailure;
      break;
    }

    pageContentContext->BT();   // begin text object
    pageContentContext->k(0,0,0,1);  // Set the non-stroking color space
    pageContentContext->Tf(arialTTF,20); // Set the text font, T f , to font and the text font size
    /*
     * Set the text matrix a b c d e f
     * a b 0
     * c d 0
     * e f 1
     */
        pageContentContext->Tm(1,0,0,1,90,610);
    pageContentContext->Tj("San Antonio Pass, Cordillera Huayhuash, Peru"); // Show a text string.
    pageContentContext->ET(); // end text object

    // End content context, and write the page  !!!!!!!!!!!!!!!!!!!!!!!! required
    status = pdfWriter.EndPageContentContext(pageContentContext);
    if(status != eSuccess)
      break;

    status = pdfWriter.WritePageAndRelease(pdfPage);
    if(status != eSuccess)
      break;

    status = pdfWriter.EndPDF();
  }while(false);

  if(eSuccess == status)
    cout<<"Succeeded in creating PDF file\n";
  else
    cout<<"Failed in creating PDF file\n";
}

void shapes(){
    PDFWriter pdfWriter;
  EStatusCode status;

  do
  {
    // Initial Setup for file,page and page content
    status = pdfWriter.StartPDF("./test.pdf",ePDFVersion13);
    if(status != eSuccess)
      break;

    PDFPage* pdfPage = new PDFPage();
    pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));
    PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

    // Start adding content to page

    // Draw a Line, stroke
    pageContentContext->q();
    pageContentContext->w(2);
    pageContentContext->K(0,0,1,0);

    pageContentContext->m(10,500);
    pageContentContext->l(30,700);
    pageContentContext->s();

    pageContentContext->Q();

    // Draw a Polygon, stroke
    pageContentContext->q();
    pageContentContext->w(2);
    pageContentContext->K(0,1,0,0);

    pageContentContext->m(40,500);
    pageContentContext->l(60,700);
    pageContentContext->l(160,700);
    pageContentContext->l(140,500);
    pageContentContext->s();

    pageContentContext->Q();

    // Draw a Rectangle, fill
    pageContentContext->q();
    pageContentContext->k(0,1,0,0);

    pageContentContext->re(200,400,100,300);
    pageContentContext->f();

    pageContentContext->Q();

    // Pausing content context, to allow form definition
    pdfWriter.PausePageContentContext(pageContentContext);

    // Create a form with a triangle pattern, RGB colorspace, fill
    PDFFormXObject* formXObject = pdfWriter.StartFormXObject(PDFRectangle(0,0,400,400));
    XObjectContentContext* xobjectContent = formXObject->GetContentContext();

    xobjectContent->rg(1,0,0);
    xobjectContent->m(0,0);
    xobjectContent->l(200,400);
    xobjectContent->l(400,0);
    xobjectContent->f();

    ObjectIDType formObjectID = formXObject->GetObjectID();
    status = pdfWriter.EndFormXObjectAndRelease(formXObject);
    if(status != eSuccess)
      break;

    // Place the form in multiple locations in the page
    string formNameInPage = pdfPage->GetResourcesDictionary().AddFormXObjectMapping(formObjectID);

    pageContentContext->q();
    pageContentContext->cm(0.5,0,0,0.5,120,100);
    pageContentContext->Do(formNameInPage);
    pageContentContext->Q();

    pageContentContext->q();
    pageContentContext->cm(0.2,0,0,0.2,350,100);
    pageContentContext->Do(formNameInPage);
    pageContentContext->Q();

    // End adding content to page

    // end page content, write page object and finalize PDF
    status = pdfWriter.EndPageContentContext(pageContentContext);
    if(status != eSuccess)
      break;

    status = pdfWriter.WritePageAndRelease(pdfPage);
    if(status != eSuccess)
      break;

    status = pdfWriter.EndPDF();
  }while(false);

  if(eSuccess == status)
    cout<<"Succeeded in creating PDF file\n";
  else
    cout<<"Failed in creating PDF file\n";    
}


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

        break;  // We are interested with just the first page
    }
}

// Forward declaration
void parsePDFDictionary(PDFParser &parser, PDFDictionary *obj, int depth);
void parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *obj, int depth);
void parseObjectArray(PDFParser &parser, PDFArray *object, int depth);
    
void parseObjectName(PDFParser &parser, PDFName *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [name]: " << obj->GetValue() <<endl;
}

void parseObjectInteger(PDFParser &parser, PDFInteger *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [integer]: " << obj->GetValue() <<endl;
}

void parseObjectLiteralStr(PDFParser &parser, PDFLiteralString *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [literalStr]: " << obj->GetValue() <<endl;
}

void parseObjectBoolean(PDFParser &parser, PDFBoolean *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [bool]: " << obj->GetValue() <<endl;
}

void parseObjectHexString(PDFParser &parser, PDFHexString *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [hex]: " << obj->GetValue() <<endl;
}

void parseObjectNull(PDFParser &parser, PDFNull *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [NULL] " <<endl;
}

void parseObjectReal(PDFParser &parser, PDFReal *obj, int depth){
    depth++;
    cout << std::string(depth, ' ') << " [real]: " << obj->GetValue() <<endl;
}

void parseObjectStream(PDFParser &parser, PDFStreamInput *object, int depth){
    depth++;
    PDFDictionary* obj1 = object->QueryStreamDictionary();
    cout << std::string(depth, '.') << "streamDictionary " << endl;
    PDFObjectCastPtr<PDFDictionary> aDictionary(obj1);
    parsePDFDictionary(parser, aDictionary.GetPtr(), depth);


    MapIterator<PDFNameToPDFObjectMap> it = aDictionary->GetIterator();
    
    do {
        PDFObject *obj2 = it.GetValue();
        if (obj2->GetType() == PDFObject::ePDFObjectInteger){
            // cout << "printing" << ((PDFInteger*)obj2)->GetValue() << endl;

            int bufLen = ((PDFInteger*)obj2)->GetValue();
            
            IByteReader* streamReader = parser.CreateInputStreamReader(object);
            Byte buffer[bufLen];
            if(streamReader) {
                while(streamReader->NotEnded()) {
                    LongBufferSizeType readAmount = streamReader->Read(buffer, bufLen);
                    cout.write((const char*)buffer,readAmount);
                }
            }
            break;  // break after first Length
        }
    } while(it.MoveNext());
        


    /////////////////////////////////////

    // Byte buffer[100000];
    // IByteReader* r = parser.StartReadingFromStream(object);
    // while (r->NotEnded()){
    //     LongBufferSizeType readAmount = r->Read(buffer,100000);
    //     cout.write((const char*)buffer,readAmount);
    //     cout << '\n ';
    //     cout << "===============================\n";
    // }

    // exit(0);      //

    ////////////////////////////////////////

    // PDFObjectParser* p = parser.StartReadingObjectsFromStream(object);
    // PDFObject* obj;
    // while( obj=p->ParseNewObject()) {  // TODO: this skips the first one

    //     cout << "parsing " << obj->scPDFObjectTypeLabel(obj->GetType()) << " " <<endl;
        
    // }  
  
  
}

void parseObjectSymbol(PDFParser &parser, PDFSymbol *obj, int depth){
    depth++;
    // equivalent to UNKNOWN
    cout << std::string(depth, ' ') << " [symbol]: " << "UNKNOWN" <<endl;
}

void parseObjectArray(PDFParser &parser, PDFArray *object, int depth){
    depth++;
    SingleValueContainerIterator<PDFObjectVector> it = object->GetIterator();
    int length = object->GetLength();
    int start = 0;
    do {
        PDFObject* obj = it.GetItem();
        
        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            //cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            //cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            //cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            parseObjectHexString(parser, (PDFHexString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            //cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            parseObjectNull(parser, (PDFNull*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            //cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            parseObjectName(parser, (PDFName*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            //cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            parseObjectInteger(parser, (PDFInteger*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            //cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            parseObjectReal(parser, (PDFReal*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            //cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            parseObjectArray(parser, (PDFArray*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            // cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        // TODO:this looks like create infinite recrsion
        // else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
        //     cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
        //     parsePDFIndirectObjectReference(parser, (PDFIndirectObjectReference*) obj, depth);
        // }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            //cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            parseObjectStream(parser, (PDFStreamInput*)obj, depth);
        }
        else {
            //cout << std::string(depth, '.') << "array ELSE " << obj->GetType() << endl; //IndirectRef
        }

        start++;
    } while(it.MoveNext());

}

void parsePDFDictionary(PDFParser &parser, PDFDictionary *obj, int depth=0){
    depth++;
    auto it = obj->GetIterator();
  
    do {  // TODO: this skips the first one
        PDFName* name = it.GetKey();
        PDFObject* obj = it.GetValue();
        
        if (obj->GetType() == PDFObject::ePDFObjectBoolean){
            //cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
            parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
            //cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
            parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectHexString){
            //cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
            parseObjectHexString(parser, (PDFHexString*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectNull){
            //cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
            parseObjectNull(parser, (PDFNull*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectName){
            //cout << std::string(depth, '.') << "ePDFObjectName" << endl;
            parseObjectName(parser, (PDFName*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectInteger){
            //cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
            parseObjectInteger(parser, (PDFInteger*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectReal){
            //cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
            parseObjectReal(parser, (PDFReal*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectArray){
            //cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
            parseObjectArray(parser, (PDFArray*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
            //cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << endl;  // 8
            parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
            //cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << endl;
            parsePDFIndirectObjectReference(parser, (PDFIndirectObjectReference*) obj, depth);
        }
        else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
            //cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
            parseObjectStream(parser, (PDFStreamInput*)obj, depth);
        }
        else {
            //cout << std::string(depth, '.') << "UNKNOWN" <<endl;
        }
    } while(it.MoveNext());
    
}
void parsePDFIndirectObjectReference(PDFParser &parser, PDFIndirectObjectReference *object, int depth=0){
    depth++;

    PDFObject* obj = parser.ParseNewObject(object->mObjectID); // TODO: convert to RefCountPtr<PDFObject>

    if (obj->GetType() == PDFObject::ePDFObjectBoolean){
        //cout << std::string(depth, '.') << "ePDFObjectBoolean" <<endl;
        parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
        //cout << std::string(depth, '.') << "ePDFObjectLiteralString" << endl;
        parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectHexString){
        //cout << std::string(depth, '.') << "ePDFObjectHexString" << endl;
        parseObjectHexString(parser, (PDFHexString*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectNull){
        //cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
        parseObjectNull(parser, (PDFNull*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectName){
        // cout << std::string(depth, '.') << "ePDFObjectName" << endl;
        parseObjectName(parser, (PDFName*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectInteger){
        //cout << std::string(depth, '.') << "ePDFObjectInteger" << endl;
        parseObjectInteger(parser, (PDFInteger*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectReal){
        //cout << std::string(depth, '.') << "ePDFObjectReal" << endl;
        parseObjectReal(parser, (PDFReal*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectArray){
        //cout << std::string(depth, '.') << "ePDFObjectArray" << endl;
        parseObjectArray(parser, (PDFArray*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
        //cout << std::string(depth, '.') << "ePDFObjectDictionary" << endl;
        parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
    }
    else if (obj->GetType() == PDFObject::ePDFObjectStream){
        //cout << std::string(depth, '.') << "ePDFObjectStream" << endl;
        parseObjectStream(parser, (PDFStreamInput*)obj, depth);
    }
    else {
        //cout << std::string(depth, '.') << "UNKNOWN" << endl;
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

    parsePDFDictionary(parser, page.GetPtr());

}


void testPage2(PDFParser &parser, InputFile &pdfFile){
    cout << parser.GetPagesCount() <<endl;

    ObjectIDType id = parser.GetPageObjectID(1);
    cout << "page 1 " << id << endl;
    ObjectIDType id_end = parser.GetPageObjectID(2);
    cout << "page 2 " << id << endl;


    
    ObjectIDType index_id = id;
    while (index_id < 2){
        PDFObject* obj = parser.ParseNewObject(id);
        cout << obj->GetType() <<endl;
        // PDFObjectParser pp =  parser.GetObjectParser();

        parsePDFDictionary(parser, (PDFDictionary*)obj);
        index_id++;
    
    }

}

int testPage3(){
    string path = "/u/kr/Downloads/dsohowto2.pdf";

    // // overwrites pdf
    // PDFWriter pdfWriter;
    // pdfWriter.StartPDF(path, ePDFVersion13);  
    // PDFPage* pdfPage = new PDFPage();
    // PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

    
    
    }
    
int readin_original(){
    string path = "/u/kr/Downloads/dsohowto2.pdf";
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

    showPagesInfo(parser,pdfFile,status);

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

    // testPage(parser, pdfFile);
    testPage2(parser, pdfFile);

}

int main() {
//    basic();
//    shapes();

    // readin_original();
    readin();

    // testPage3();

}
