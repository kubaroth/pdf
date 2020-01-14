// #include "main_test.h"

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
#include <PDFWriter/IDocumentContextExtender.h>      // dictionary
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFBoolean.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFNull.h>
#include <PDFWriter/PDFSymbol.h>
#include <PDFWriter/PDFStreamInput.h>


// #include "PDFParserTest.h"
// #include "PDFParser.h"
// #include "InputFile.h"
// #include "PDFObject.h"
// #include "PDFDictionary.h"
// #include "PDFObjectCast.h"
// #include "PDFIndirectObjectReference.h"
// #include "PDFArray.h"
// #include "PDFDictionary.h"
// #include "PDFStreamInput.h"
// #include "OutputFile.h"
// #include "IByteWriterWithPosition.h"
// #include "PrimitiveObjectsWriter.h"

// #include <iostream>


#include <PDFWriter/ObjectsBasicTypes.h>


#include <set>

class PDFObject;
class PDFParser;
class IByteWriter;


using namespace std;
using namespace PDFHummus;

typedef set<ObjectIDType> ObjectIDTypeSet;


static const char* scIndirectStart = "Indirect object reference:\r\n";
static const char* scParsedAlready = "was parsed already\r\n";
static const char* scIteratingStreamDict = "Stream . iterating stream dictionary:\r\n";

static int mTabLevel;
static ObjectIDTypeSet mIteratedObjectIDs;

EStatusCode IterateObjectTypes(PDFObject* inObject,PDFParser& inParser,IByteWriter* inOutput)
{
    PrimitiveObjectsWriter primitivesWriter;

    primitivesWriter.SetStreamForWriting(inOutput);

    for(int i=0;i<mTabLevel;++i)
        inOutput->Write((const Byte*)"  ",2);

    if(inObject->GetType() == PDFObject::ePDFObjectIndirectObjectReference)
    {
        inOutput->Write((const Byte*)scIndirectStart,strlen(scIndirectStart));
        if(mIteratedObjectIDs.find(((PDFIndirectObjectReference*)inObject)->mObjectID) == mIteratedObjectIDs.end())
        {
            mIteratedObjectIDs.insert(((PDFIndirectObjectReference*)inObject)->mObjectID);
            RefCountPtr<PDFObject> pointedObject(inParser.ParseNewObject(((PDFIndirectObjectReference*)inObject)->mObjectID));
            if(!pointedObject)
            {
                cout<<"\nFailed to retreive object of ID ="<<((PDFIndirectObjectReference*)inObject)->mObjectID<<"\n";
                return PDFHummus::eFailure;
            }
            return IterateObjectTypes(pointedObject.GetPtr(),inParser,inOutput);
        }
        else
        {
            for(int i=0;i<mTabLevel;++i)
                inOutput->Write((const Byte*)"  ",2);
            inOutput->Write((const Byte*)scParsedAlready,strlen(scParsedAlready));
            return PDFHummus::eSuccess;
        }
        
    }
    else if(inObject->GetType() == PDFObject::ePDFObjectArray)
    {
        primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
        ++mTabLevel;
        PDFObjectCastPtr<PDFArray> anArray;
        anArray = inObject;  // do assignment here, otherwise it's considered constructor...which won't addref
        SingleValueContainerIterator<PDFObjectVector> it = anArray->GetIterator();
        EStatusCode status = PDFHummus::eSuccess;
        while(it.MoveNext() && PDFHummus::eSuccess == status)
            status = IterateObjectTypes(it.GetItem(),inParser,inOutput);
        --mTabLevel;
        return status;
    }
    else if(inObject->GetType() == PDFObject::ePDFObjectDictionary)
    {
        primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));
        ++mTabLevel;
        PDFObjectCastPtr<PDFDictionary> aDictionary;
        aDictionary = inObject; // do assignment here, otherwise it's considered constructor...which won't addref
        MapIterator<PDFNameToPDFObjectMap> it = aDictionary->GetIterator();

        EStatusCode status = PDFHummus::eSuccess;
        while(it.MoveNext() && PDFHummus::eSuccess == status)
        {
            status = IterateObjectTypes(it.GetKey(),inParser,inOutput);
            if(PDFHummus::eSuccess == status)
                status = IterateObjectTypes(it.GetValue(),inParser,inOutput);
        }
        --mTabLevel;
        return status;

    }
    else if(inObject->GetType() == PDFObject::ePDFObjectStream)
    {
        inOutput->Write((const Byte*)scIteratingStreamDict,strlen(scIteratingStreamDict));
        PDFObjectCastPtr<PDFDictionary> aDictionary(((PDFStreamInput*)inObject)->QueryStreamDictionary());


    // IByteReader* streamReader = inParser.CreateInputStreamReader((PDFStreamInput*)inObject);
    // Byte buffer[1000];

    // // instead of passing open file heree  - Test if the streamstart needs to be set (looks the same)
    // InputFile pdfFile;
    // EStatusCode status = pdfFile.OpenFile("/u/kr/Downloads/dsohowto2.pdf");
    // if(status != eSuccess) {
    //     cout << "problem opening file" <<endl;
    // }
    // IByteReaderWithPosition* inPDFStream = pdfFile.GetInputStream();
    
    // if(streamReader) {
    //     while(streamReader->NotEnded()) {
    //         LongBufferSizeType readAmount = streamReader->Read(buffer,1000);
    //         cout.write((const char*)buffer,readAmount);

    //         // to dsohowto2.txt
    //         // primitivesWriter.WriteKeyword(tTypeLabel(inObject->GetType()));
    //     }
    //     cout << "\n";
    // }    


    //////////////////////////
    

    MapIterator<PDFNameToPDFObjectMap> it = aDictionary->GetIterator();
    
    do {
        PDFObject *obj2 = it.GetValue();
        if (obj2->GetType() == PDFObject::ePDFObjectInteger){
            // cout << "printing" << ((PDFInteger*)obj2)->GetValue() << endl;

            int bufLen = ((PDFInteger*)obj2)->GetValue();
        
            IByteReader* streamReader = inParser.CreateInputStreamReader((PDFStreamInput*)inObject);
            Byte buffer[bufLen];
            if(streamReader) {
            while(streamReader->NotEnded()) {
                LongBufferSizeType readAmount = streamReader->Read(buffer, bufLen);
                cout.write((const char*)buffer,readAmount);
            }
            }
            break;
        }
    } while(it.MoveNext());
    

    /////////////////////////////////

    // PDFObjectParser* p = inParser.StartReadingObjectsFromStream((PDFStreamInput*)inObject);
    // PDFObject* obj;
    // while( obj=p->ParseNewObject()) {  // TODO: this skips the first one

    //     cout << "parsing " << obj->scPDFObjectTypeLabel(obj->GetType()) << " " <<endl;
    
    // }


  
     return IterateObjectTypes(aDictionary.GetPtr(),inParser,inOutput);

    }
    else
    {
    // to dsohowto2.txt
        primitivesWriter.WriteKeyword(PDFObject::scPDFObjectTypeLabel(inObject->GetType()));

    ////////////////////////////////////////////  to std::cout
    // PDFObject* obj = inObject;

    // int depth = 0;
    // if (obj->GetType() == PDFObject::ePDFObjectBoolean){
    //     cout << std::string(depth, '.') << "ePDFObjectBoolean " << ((PDFBoolean*)obj)->GetValue() <<endl;
    //     // parseObjectBoolean(parser, (PDFBoolean*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectLiteralString){
    //     cout << std::string(depth, '.') << "ePDFObjectLiteralString " << ((PDFLiteralString*)obj)->GetValue()<< endl;
    //     // parseObjectLiteralStr(parser, (PDFLiteralString*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectHexString){
    //     cout << std::string(depth, '.') << "ePDFObjectHexString " << ((PDFHexString*)obj)->GetValue()<< endl;
    //     // parseObjectHexString(parser, (PDFHexString*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectNull){
    //     cout << std::string(depth, '.') << "ePDFObjectNull" << endl;
    //     // parseObjectNull(parser, (PDFNull*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectName){
    //     cout << std::string(depth, '.') << "ePDFObjectName " << ((PDFName*)obj)->GetValue() << endl;
    //     // parseObjectName(parser, (PDFName*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectInteger){
    //     cout << std::string(depth, '.') << "ePDFObjectInteger " << ((PDFInteger*)obj)->GetValue() <<endl;
    //     // parseObjectInteger(parser, (PDFInteger*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectReal){
    //     cout << std::string(depth, '.') << "ePDFObjectReal " << ((PDFReal*)obj)->GetValue() << endl;
    //     // parseObjectReal(parser, (PDFReal*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectArray){
    //     cout << std::string(depth, '.') << "ePDFObjectArray" << "_______"<<endl;
    //     // parseObjectArray(parser, (PDFArray*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectDictionary){
    //     cout << std::string(depth, '.') << "(ePDFObjectDictionary)" << "_______" <<endl;      // 8
    //     // parsePDFDictionary(parser, (PDFDictionary*)obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference){
    //     cout << std::string(depth, '.') << "(ePDFObjectIndirectObjectReference)" << "_______" << endl;
    //     // parsePDFIndirectObjectReference(parser, (PDFIndirectObjectReference*) obj, depth);
    // }
    // else if (obj->GetType() == PDFObject::ePDFObjectStream){         // 10
    //     cout << std::string(depth, '.') << "ePDFObjectStream" << "_______" << endl;
    //     // parseObjectStream(parser, (PDFStreamInput*)obj, depth);
    // }
    // else {
    //     cout << std::string(depth, '.') << "UNKNOWN" << "_______" <<endl;
    // }


    
        return PDFHummus::eSuccess;
    }
    
}


int main(){
    EStatusCode status = PDFHummus::eSuccess;
    InputFile pdfFile;
    PDFParser parser;
    OutputFile outputFile;

    do
    {
        status = pdfFile.OpenFile("../dsohowto.pdf");
        if(status != PDFHummus::eSuccess)
        {
            cout<<"unable to open file for reading. should be in TestMaterials/XObjectContent.pdf\n";
            break;
        }

        status = parser.StartPDFParsing(pdfFile.GetInputStream());
        if(status != PDFHummus::eSuccess)
        {
            cout<<"unable to parse input file";
            break;
        }

        // now let's do something with what got parsed

        if(parser.GetPDFLevel() != 1.4)
        {
            cout<<"expecting level 1.4, got "<<parser.GetPDFLevel()<<"\n";
            status = PDFHummus::eFailure;
            break;
        }

        if(parser.GetPagesCount() != 47)
        {
            cout<<"expecting 2 pages, got "<<parser.GetPagesCount()<<"\n";
            status = PDFHummus::eFailure;
            break;
        }

        // now just iterate, and print the object types
        PDFObjectCastPtr<PDFDictionary> catalog(parser.QueryDictionaryObject(parser.GetTrailer(),"Root"));
        if(!catalog)
        {
            cout<<"Can't find catalog. fail\n";
            status = PDFHummus::eFailure;
            break;
        }

        mTabLevel = 0;
        status = outputFile.OpenFile("../dsohowto2.txt");

        status = IterateObjectTypes(catalog.GetPtr(),parser,outputFile.GetOutputStream());
        if(status != PDFHummus::eSuccess)
        {
            cout<<"Failed iterating object types\n";
            break;
        }

    }while(false);

}
