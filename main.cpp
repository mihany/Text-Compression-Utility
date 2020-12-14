/*
 *      Author : Mohamed Ibrahim Hany
*/
#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"
#define LI long int
#define UC unsigned char
const UC reference = 0x80;
using namespace std;

/*---------------------------------------------------Node-------------------------------------------------------------*/
/*------------------------------------------------Compressor----------------------------------------------------------*/
struct newNode{
    newNode *right{};
    newNode *left{};
    LI frequency{};
    UC symbol{};
    string usedBit;
};
bool nodesComparison(const newNode& tokenA,const newNode& tokenB){
    return (tokenA.frequency < tokenB.frequency);
}
/*-----------------------------------------------Decompressor---------------------------------------------------------*/
struct interpretation{
    interpretation *z;
    interpretation *o;
    UC symbol;
};
/*--------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------ProtoTyping---------------------------------------------------------*/
/*------------------------------------------------Compressor----------------------------------------------------------*/
void compressor(char* );

LI fileSizeCompressor(char*);
void fCounter(int, UC&, int, FILE*);
void fileSizeByte(LI, UC&,int, FILE*);
void fileNamerCompressor(char*, string*, UC&, int&, FILE*);
void symbolConverter(UC, UC&,int, FILE*);
void fileContentFiller(FILE*, LI, string*, UC&, int&, FILE*);
/*-----------------------------------------------Decompressor---------------------------------------------------------*/
void decompressor(char* );
bool ifFile(UC&,int&,FILE*);
LI fileSizeDecompressor(UC&,int,FILE*);
void fileNamerDecompressor(char*,int,UC&,int&,interpretation*,FILE*);
void fileInterpreter(char*,LI,UC&,int&,interpretation*,FILE*);
UC pattern8BitsProcess(UC&,int,FILE*);
void toString(UC&,int,int&,FILE*,interpretation*,UC);
void treeDestructor(interpretation*);
/*--------------------------------------------------------------------------------------------------------------------*/


int main(void) {

    string mode;
    char* inputFile;
    cout<<"Enter the Huffman mode (compress or decompress)"<<endl;
    cin>>mode;

    if(mode == "compress"){
        cout<<"Compression mode is activated :D"<<endl<<endl;   // "../testFiles/bible.txt"

        inputFile = "../aacccccdddeee";
        compressor(inputFile);

    }else if(mode == "decompress"){
        cout<<"Decompression mode is activated :D"<<endl<<endl;   // "../testFiles/bible.txt"

        inputFile = "../ahmed.huf";
        decompressor(inputFile);
    }

}

/*------------------------------------------------Compressor----------------------------------------------------------*/
void compressor(char* inputFile){

    string output;
    LI frequency[256] = { };
    LI fileInBits = 0;
    int totalUniqueSymbols = 0;
    FILE *pointerToTextFile;
    FILE *pointerToCompressedFile;

    pointerToTextFile = fopen( inputFile ,"rb");
    if(!pointerToTextFile){
        cout<< inputFile <<" is not FOUND"<<endl<<"Compressor will EXIT :P"<<endl;
        exit(1);
    }
    fclose(pointerToTextFile);

    output =  inputFile;
    output.replace(output.size()-4,4,".huf");

    UC *tokenPointer;
    UC token;
    tokenPointer = &token;

    LI finalFileSize=0;
    LI S;

    for(char *ch =  inputFile ;*ch; ++ch){
        ++frequency[(UC)(*ch)];                  //            cout<<number[(UC)(*c)]<<endl;
    }

    S = fileSizeCompressor( inputFile );
    finalFileSize += S;

    fileInBits += 56;

    pointerToTextFile = fopen( inputFile ,"rb");
    fread(tokenPointer, 1, 1, pointerToTextFile);

    for(LI f=0 ; f < S ; ++f){
        ++frequency[token];
        fread(tokenPointer, 1, 1, pointerToTextFile);
    }
    fclose(pointerToTextFile);

    for(LI *f = frequency ; f < frequency+256 ; ++f){
        if(*f){
            ++totalUniqueSymbols;
        }
    }
//    cout<<totalUniqueSymbols<<endl;

    newNode nodesHolder[ totalUniqueSymbols*2-1 ];
    newNode *N = nodesHolder;
    for(LI *k = frequency ; k< 256+frequency ; ++k){
        if(*k){
            N->right = nullptr;
            N->left = nullptr;
            N->frequency = *k;
            N->symbol = k-frequency;
            ++N;
        }

        cout<<N->frequency<<"\t"<<N->symbol<<endl;
    }

    sort(nodesHolder, nodesHolder + totalUniqueSymbols, nodesComparison);

    newNode *node1 = nodesHolder;
    newNode *node2 = nodesHolder + 1;
    newNode *currentNode = nodesHolder + totalUniqueSymbols;
    newNode *parentNode = nodesHolder + totalUniqueSymbols;
    newNode *childNode = nodesHolder + 2;

    for(int n=0 ; n < totalUniqueSymbols-1 ; ++n){
        currentNode -> frequency = node1 -> frequency + node2 -> frequency;
        currentNode -> left = node1;
        currentNode -> right = node2;
        node1 -> usedBit="1";
        node2 -> usedBit="0";
        ++currentNode;

        if(childNode >= nodesHolder + totalUniqueSymbols){
            node1 = parentNode;
            ++parentNode;
        }
        else{
            if(childNode->frequency < parentNode->frequency){
                node1 = childNode;
                ++childNode;
            }
            else{
                node1=parentNode;
                ++parentNode;
            }
        }

        if(childNode >= nodesHolder + totalUniqueSymbols){
            node2=parentNode;
            ++parentNode;
        }
        else if(parentNode>=currentNode){
            node2=childNode;
            ++childNode;
        }
        else{
            if(childNode->frequency < parentNode->frequency){
                node2=childNode;
                ++childNode;
            }
            else{
                node2=parentNode;
                ++parentNode;
            }
        }

    }


    for(N = nodesHolder + totalUniqueSymbols*2-2 ; N>nodesHolder-1 ; --N){
//        ((!pointerToTextFile) ? cout<< inputFile <<" is not found! "<<endl : cout<<"Compressor Starts"<<endl);
        if(N->left){
            N->left->usedBit = N->usedBit + N->left->usedBit;
        }
        if(N->right){
            N->right->usedBit = N->usedBit + N->right->usedBit;
        }
    }

    pointerToCompressedFile = fopen(&output[0], "wb");
    int latestBitCounter = 0;
    UC latestByte;

    fwrite(&totalUniqueSymbols, 1, 1, pointerToCompressedFile);
    fileInBits += 8;

    char *pointerToString;
    UC currentLength;
    UC currentSymbol;
    string arrayString[256];
    for( N = nodesHolder ; N < nodesHolder + totalUniqueSymbols ; ++N){
        cout<<"Symbol: "<< N-> symbol << N -> frequency << "\t" << N -> usedBit<<endl;
        arrayString[(N -> symbol)] = N -> usedBit;
        currentLength = N -> usedBit.length();
        currentSymbol = N -> symbol;

        symbolConverter(currentSymbol,latestByte,latestBitCounter,pointerToCompressedFile);
        symbolConverter(currentLength,latestByte,latestBitCounter,pointerToCompressedFile);


        fileInBits += currentLength + 16;

        pointerToString = &N->usedBit[0];
        while(*pointerToString){
            if(latestBitCounter == 8){
                fwrite(&latestByte,1,1,pointerToCompressedFile);
                latestBitCounter = 0;
            }
            if(*pointerToString == '1'){
                latestByte <<= 1;                   // Shift Operator for bit masking
                latestByte |= 1;                    // Or bit masking
                ++latestBitCounter;
            }else if(*pointerToString == '0'){
                latestByte <<= 1;                   // Shift Operator for bit masking
                ++latestBitCounter;
            }else{
                cout<<"An error has occurred"<<endl<<"Compression process aborted"<<endl;
                fclose(pointerToCompressedFile);
                remove(&output[0]);
                exit(1);
            }
            ++pointerToString;
        }

        fileInBits += currentLength*( N->frequency );
    }

//    cout<<"Total bits before: "<<fileInBits<<endl;
    if(fileInBits % 8){
        fileInBits = (fileInBits/8+1) * 8;
    }
//    cout<<"Total bits after: "<<fileInBits<<endl;

    cout << "Text file size is: \t\t" << finalFileSize << " bytes" << endl;
    cout << "Compressed file size is: \t" << fileInBits/8 << " bytes" << endl;
    cout << "The compression ratio will be: \t" << (((float)fileInBits/8)/finalFileSize)*100<< "%" << endl;         //The compression ratio
    cout << "The efficiency (η): \t\t" << 1.0-(((float)fileInBits/8)/finalFileSize) << endl;
    if(fileInBits/8 > finalFileSize){
        cout << endl << "THE SIZE OF COMPRESSED FILE IS GREATER THAN SOURCE FILE" << endl << endl;
    }

    fCounter(1,latestByte,latestBitCounter,pointerToCompressedFile);

    pointerToTextFile = fopen( inputFile ,"rb");
    fseek(pointerToTextFile, 0, SEEK_END);
    S = ftell(pointerToTextFile);
    rewind(pointerToTextFile);

    if(latestBitCounter == 8){
        fwrite(&latestByte, 1, 1, pointerToCompressedFile);
        latestBitCounter = 0;
    }
    latestByte <<= 1;                // Shift Operator for bit masking
    latestByte |= 1;                 // Or bit masking
    ++latestBitCounter;

    fileSizeByte(S,latestByte,latestBitCounter,pointerToCompressedFile);
    fileNamerCompressor( inputFile ,arrayString,latestByte,latestBitCounter,pointerToCompressedFile);
    fileContentFiller(pointerToTextFile,S,arrayString,latestByte,latestBitCounter,pointerToCompressedFile);
    fclose(pointerToTextFile);

    ((latestBitCounter == 8) ? fwrite(&latestByte, 1, 1, pointerToCompressedFile) : latestByte <<= 8 - latestBitCounter, fwrite(&latestByte, 1, 1, pointerToCompressedFile));

    fclose(pointerToCompressedFile);
    cout<<endl<<"Created compressed file: "<<output<<endl;
    cout<<"Compression is complete"<<endl;
}
/*------------------------------------------------Compressor----------------------------------------------------------*/

/*-----------------------------------------------Decompressor---------------------------------------------------------*/
void decompressor(char* inputFile){

    int frequency=0;
    FILE *pointerToCompressedFile;

    pointerToCompressedFile = fopen(inputFile,"rb");
    if(!pointerToCompressedFile){
        cout<<inputFile<<" does not exist"<<endl;
        exit(1);
    }

    fread(&frequency, 1, 1, pointerToCompressedFile);
    if(frequency == 0)
        frequency=256;
//    cout<<frequency<<endl;

    UC latestByte = 0;
    UC latestSymbol;
    int latestBitCounter = 0;
    int len;
    interpretation *origin = new interpretation;
    origin -> z = nullptr;
    origin ->o = nullptr;

    for(int i = 0; i < frequency ; ++i){
        latestSymbol = pattern8BitsProcess(latestByte, latestBitCounter, pointerToCompressedFile);
        len = pattern8BitsProcess(latestByte, latestBitCounter, pointerToCompressedFile);
        if(len == 0)
            len=256;
        toString(latestByte, len,latestBitCounter, pointerToCompressedFile, origin, latestSymbol);
    }

    pattern8BitsProcess(latestByte, latestBitCounter, pointerToCompressedFile);
    pattern8BitsProcess(latestByte, latestBitCounter, pointerToCompressedFile);

    if(ifFile(latestByte,latestBitCounter, pointerToCompressedFile)){
        LI size = fileSizeDecompressor(latestByte, latestBitCounter, pointerToCompressedFile);

        int lengthOfFileName = pattern8BitsProcess(latestByte, latestBitCounter, pointerToCompressedFile);
        char outputNewFile[lengthOfFileName+4];
        fileNamerDecompressor(outputNewFile, lengthOfFileName,latestByte,latestBitCounter, origin, pointerToCompressedFile);

        fileInterpreter(outputNewFile,size,latestByte,latestBitCounter,origin,pointerToCompressedFile);
    }

    fclose(pointerToCompressedFile);
    treeDestructor(origin);
    cout<<"The original text file is retrieved"<<inputFile<<endl;
}
/*-----------------------------------------------Decompressor---------------------------------------------------------*/

void symbolConverter(UC symbol,UC &latestByte,int latestBitCounter,FILE *filePointer){
    latestByte <<= 8-latestBitCounter;                              // Shift Operator for bit masking
    latestByte |= ( symbol >> latestBitCounter);                    // Or bit masking
    fwrite(&latestByte, 1, 1, filePointer);
    latestByte = symbol;
}

void fCounter(int counter,UC &latestByte,int latestBitCounter,FILE *pointerToCompressedFile){
    UC temp = counter%256;
    symbolConverter(temp,latestByte,latestBitCounter,pointerToCompressedFile);
    temp = counter/256;
    symbolConverter(temp,latestByte,latestBitCounter,pointerToCompressedFile);
}

void fileSizeByte(LI S,UC &latestByte,int latestBitCounter,FILE *pointerToCompressedFile){
    for(int i = 0 ; i < 8 ; ++i){
        symbolConverter(S%256,latestByte,latestBitCounter,pointerToCompressedFile);
        S /= 256;
    }
}

void fileNamerCompressor(char *name, string *arrayString, UC &latestByte, int &latestBitCounter, FILE *pointerToCompressedFile){
    symbolConverter(strlen(name),latestByte,latestBitCounter,pointerToCompressedFile);
    char *pointerToString;
    for(char *ch = name ; *ch ; ++ch){
        pointerToString = &arrayString[(UC)(*ch)][0];
        while(*pointerToString){
            if(latestBitCounter == 8){
                fwrite(&latestByte, 1, 1, pointerToCompressedFile);
                latestBitCounter = 0;
            }
            if(*pointerToString == '1'){
                latestByte <<= 1;             // Shift Operator for bit masking
                latestByte |= 1;              // Or bit masking
                ++latestBitCounter;
            }else if (*pointerToString == '0'){
                latestByte <<= 1;             // Shift Operator for bit masking
                ++latestBitCounter;
            }else{
                cout<<"ERROR in fileNamerCompressor Function"<<endl;
                exit(1);
            }
            ++pointerToString;
        }
    }
}

void fileContentFiller(FILE *pointerToTextFile, LI S, string *arrayString, UC &latestByte, int &latestBitCounter, FILE *pointerToCompressedFile){
    UC *tokenPointer;
    UC token;
    tokenPointer = &token;
    char *pointerToString;
    fread(tokenPointer,1,1,pointerToTextFile);
    for(LI f = 0; f<S ; ++f){
        pointerToString = & arrayString[token][0];
        while(*pointerToString){
            if(latestBitCounter == 8){
                fwrite(&latestByte, 1, 1, pointerToCompressedFile);
                latestBitCounter = 0;
            }
            if (*pointerToString == '1'){
                latestByte<<=1;             // Shift Operator for bit masking
                latestByte|=1;
                ++latestBitCounter;
            }else if (*pointerToString == '0'){
                latestByte<<=1;             // Shift Operator for bit masking
                ++latestBitCounter;
            }else{
                cout<<"ERROR in fileContentFiller Function"<<endl;
                exit(1);
            }
            ++pointerToString;
        }
        fread(tokenPointer, 1, 1, pointerToTextFile);
    }
}

LI fileSizeCompressor(char *filePath){
    LI s;
    FILE *filePointer = fopen(filePath,"rb");
    fseek(filePointer, 0, SEEK_END);        // fseek ( FILE * stream, long int offset, int origin ) > Pass SEEK_END as the last parameter to fseek to seek relative to the end of the file.
    s = ftell(filePointer);
    fclose(filePointer);
    return s;
}
/*------------------------------------------------Compressor----------------------------------------------------------*/

/*-----------------------------------------------Decompressor---------------------------------------------------------*/
void treeDestructor(interpretation *N){
    if(N->z)
        treeDestructor(N->z);
    if(N->o)
        treeDestructor(N->o);
    delete N;
}

void toString(UC &latestByte,int length,int &latestBitCounter,FILE *pointerToCompressedFile,interpretation *N,UC symbol){
    for(int i=0 ; i<length ; ++i){
        if(latestBitCounter == 0){
            fread(&latestByte, 1, 1, pointerToCompressedFile);
            latestBitCounter = 8;
        }
        switch(latestByte&reference){
            case 0:
                if(!(N->z)){
                    N->z = new interpretation;
                    N->z->z = nullptr;
                    N->z->o = nullptr;
                }
                N=N->z;
                break;
            case 128:
                if(!(N->o)){
                    N->o = new interpretation;
                    N->o->z = nullptr;
                    N->o->o = nullptr;
                }
                N = N->o;
                break;
        }
        latestByte <<= 1;
        --latestBitCounter;
    }
    N -> symbol = symbol;
}

UC pattern8BitsProcess(UC &latestByte,int latestBitCounter,FILE *filePointer){
    UC symbol;
    UC tokenByte;
    fread(&tokenByte, 1, 1, filePointer);
    symbol = latestByte | (tokenByte >> latestBitCounter);
    latestByte = tokenByte << (8-latestBitCounter);
    return symbol;
}

bool ifFile(UC &latestByte,int &latestBitCounter,FILE *pointerToCompressedFile){        //file returns True
    bool status;
    if(latestBitCounter == 0){
        fread(&latestByte, 1, 1, pointerToCompressedFile);
        latestBitCounter = 8;
    }
    status = latestByte & reference;
    latestByte <<= 1;
    --latestBitCounter;
//    cout<<status<<endl;
    return status;
}

LI fileSizeDecompressor(UC &latestByte,int latestBitCounter,FILE *pointerToCompressedFile){
    LI s=0;
    LI termM = 1;                   //for multiply
    for(int f = 0 ; f < 8 ; ++f){
        s += termM*(pattern8BitsProcess(latestByte, latestBitCounter, pointerToCompressedFile));
        termM *= 256;
    }
    return s;
}

void fileNamerDecompressor(char *outputNewFile,int lengthOfFileName,UC &latestByte,int &latestBitCounter,interpretation *origin,FILE *pointerToCompressedFile){
    interpretation *N;
    outputNewFile[lengthOfFileName] = 0;
    for(int l = 0; l < lengthOfFileName ; ++l){
        N = origin;
        while(N -> z || N -> o){
            if(latestBitCounter == 0){
                fread(&latestByte, 1, 1, pointerToCompressedFile);
                latestBitCounter = 8;
            }
            if(latestByte&reference){
                N = N -> o;
            }
            else{
                N = N-> z;
            }
            latestByte <<= 1;
            --latestBitCounter;
        }
        outputNewFile[l] = N -> symbol;
    }
}

void fileInterpreter(char *filePath,LI S,UC &latestByte,int &latestBitCounter,interpretation *origin,FILE *pointerToCompressedFile){
    interpretation *N;
    FILE *outputNewFilePointer = fopen(filePath,"wb");
    for(LI f=0 ; f<S ; ++f){
        N = origin;
        while(N->z || N->o){
            if(latestBitCounter == 0){
                fread(&latestByte, 1, 1, pointerToCompressedFile);
                latestBitCounter=8;
            }
            (latestByte&reference) ? N=N->o : N=N->z;
            latestByte <<= 1;
            --latestBitCounter;
        }
        fwrite(&(N -> symbol), 1, 1, outputNewFilePointer);
    }
    fclose(outputNewFilePointer);
}
/*-----------------------------------------------Decompressor---------------------------------------------------------*/
