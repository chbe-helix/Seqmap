//
//  main.cpp
//  sraFilter
//
//  Created by Yun Zhang on 7/26/19.
//  Copyright © 2019 Yun Zhang. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <cstring>
#include <algorithm>


using namespace std;

void jumpToLine(ifstream &file, int &jumpTo, int &currentLine)
{
    if (file.is_open())
    {
        while (file.good() & (currentLine < jumpTo))
        {
            file.ignore(numeric_limits<streamsize>::max(),'\n');
            currentLine++;
        }
    }
}

vector<string> splitString(string &inputString, string &delimiter)
{
    vector<string> outputVector;
    size_t startPosition = 0;
    size_t endPosition = 0;
    string info;
    
    while ((endPosition = inputString.find(delimiter, startPosition)) != string::npos) {
        info = inputString.substr(startPosition, endPosition-startPosition);
        outputVector.push_back(info);
        startPosition = endPosition + delimiter.length();
    }
    outputVector.push_back(inputString.substr(startPosition));
    return outputVector;
}

vector<string> splitString(string &inputString, string delimiter)
{
    vector<string> outputVector;
    size_t startPosition = 0;
    size_t endPosition = 0;
    string info;
    
    while ((endPosition = inputString.find(delimiter, startPosition)) != string::npos) {
        info = inputString.substr(startPosition, endPosition-startPosition);
        outputVector.push_back(info);
        startPosition = endPosition + delimiter.length();
    }
    outputVector.push_back(inputString.substr(startPosition));
    return outputVector;
}

vector<int> makePrefixTable(string &pattern){
    size_t n = (int)pattern.length();
    vector<int> prefixTable(n, 0);
    int len = 0;
    int i = 1;
    
    while (i < n) {
        if (pattern[i] == pattern[len]){
            len++;
            prefixTable[i] = len;
            i++;
        }
        else {
            if (len > 0) {
                len = prefixTable[len - 1];
            }
            else{
                prefixTable[i] = len;
                i++;
            }
        }
    }
    
    prefixTable.insert(prefixTable.begin(), -1);
    prefixTable.pop_back();
    return prefixTable;
}


bool KMPInclude(string &text, string pattern){
    
    vector<int> prefixTable = makePrefixTable(pattern);
    
    if (text.length() < pattern.length())
        return false;
    
    int iText = 0;
    int iPattern = 0;
    while(iText < text.length()){
        
        if (text[iText] == pattern[iPattern]){
            if (iPattern == pattern.length() - 1){
                return true;
            }
            iText++;
            iPattern++;
        }
        else{
            iPattern = prefixTable[iPattern];
            if (iPattern == -1){
                iText++;
                iPattern++;
            }
        }
    }
    return false;
}


bool KMPIncludeInVector(vector<string> &inputPattern, string &text){
    
    for (int i = 0; i < inputPattern.size(); i++)
    {
        if (KMPInclude(text, inputPattern[i]))
            return true;
    }
    return false;
}

bool equalInVector(vector<string> &inputStrings, string &checkString){
    
    if (inputStrings.size() == 0)
        return false;
    
    auto checkPosition = find(inputStrings.begin(), inputStrings.end(), checkString);
    if (checkPosition != inputStrings.end())
    {
        inputStrings.erase(checkPosition);
        return true;
    }
    return false;
}


vector<pair<int, int> > speciesIndex(string &speciesString, string &indexFilename)
{
    vector<string> speciesVector;
    speciesVector = splitString(speciesString, ",");
    vector<string> speciesInclude;
    vector<string> speciesEqual;
    
    vector<pair<int, int> > indexes;
    
    for (int i = 0; i < speciesVector.size(); i++)
    {
        if (speciesVector[i].front() == '='){
            speciesEqual.push_back(speciesVector[i].substr(1, speciesVector[i].length()-1));
        }
        else{
            speciesInclude.push_back(speciesVector[i]);
        }
    }
    
    ifstream indexFile;
    indexFile.open(indexFilename, ios_base::in);
    
    if (indexFile.is_open())
    {
        string line;
        vector<string> lineVector;
        
        while (indexFile.good())
        {
            getline(indexFile, line);
            lineVector = splitString(line, "\t");

            if(equalInVector(speciesEqual, lineVector[0]))
            {
                indexes.push_back(make_pair(stoi(lineVector[1]), stoi(lineVector[2])));
            }
            else if(KMPIncludeInVector(speciesInclude, lineVector[0]))
            {
                indexes.push_back(make_pair(stoi(lineVector[1]), stoi(lineVector[2])));
            }
        }
    }
    
    indexFile.close();
    return indexes;
}

void writeString(ofstream &outFile, string &line)
{
    if (outFile.is_open())
    {
        outFile << line << "\n";
    }
}

void writeVector(ofstream &outFile, vector<string> &inputVector)
{
    if (outFile.is_open())
    {
        for (int i = 0; i < inputVector.size(); i++)
        {
            outFile << inputVector[i] << "\t";
        }
        outFile << "\n";
    }
}


void getColNames(ifstream &inFile, vector<string> &header)
{
    if (inFile.is_open())
    {
        string headerRow;
        getline(inFile, headerRow);

        header = splitString(headerRow, "\t");
        
    }
}


class filterType
{
public:
    virtual bool judge(string &inputString)
    {
        return false;
    }
};

/*
string joinVector(vector<string> argumentVector)
{
    string outputString = "";
    for (int i = 0; i < argumentVector.size(); i++)
        outputString += argumentVector[i];
        
    return outputString;
}
*/

struct valueArgument
{
    long int criterias [3];
    // critieria[0] is >, critiera[2] is <, critiera[3] is =
    
    valueArgument(string &argument)
    {
        if (argument.substr(0, 2) == ">=")
        {
            criterias[0] = stol(argument.substr(2));
            criterias[1] = numeric_limits<long int>::min();
            criterias[2] = stol(argument.substr(2));
        }
        
        else if (argument.substr(0, 2) == "<=")
        {
            criterias[0] = numeric_limits<long int>::max();
            criterias[1] = stol(argument.substr(2));
            criterias[2] = stol(argument.substr(2));
        }
        
        else if (argument.substr(0, 1) == ">")
        {
            criterias[0] = stol(argument.substr(1));
            criterias[1] = numeric_limits<long int>::min();
            criterias[2] = -1;
        }
        
        else if (argument.substr(0, 1) == "<")
        {
            criterias[0] = numeric_limits<long int>::max();
            criterias[1] = stol(argument.substr(1));
            criterias[2] = -1;
        }
        
        else
        {
            criterias[0] = numeric_limits<long int>::max();
            criterias[1] = numeric_limits<long int>::min();
            if (argument.substr(0, 1) == "=")
                criterias[2] = stol(argument.substr(1));
            else
                criterias[2] = stol(argument);
        }
    }
    
    bool checkArgument(long int &inputValue)
    {
        if (inputValue > criterias[0])
            return true;
        if (inputValue < criterias[1])
            return true;
        if (inputValue == criterias[2])
            return true;
        return false;
    }
    
    void printCriterias()
    {
        for (int i = 0; i < 3; i++)
        {
            cout << criterias[i] << " ";
        }
    }
};

class valueArguments: public filterType
{
public:
    
    string model;
    int nArgument;
    valueArgument **arguments;
    
    valueArguments(string inputArguments)
    {
        if (KMPInclude(inputArguments, "OR"))
        {
            model = "OR";
            vector<string> argumentVector = splitString(inputArguments, "OR");
            
            arguments = new valueArgument*[argumentVector.size()];
            //cout << 1 << endl;
            for (int i = 0; i < argumentVector.size(); i++)
            {
                //valueArgument newArgument(argumentVector[i]);
                arguments[i] = new valueArgument(argumentVector[i]);
                nArgument = i;
            }
        }
        
        else if (KMPInclude(inputArguments, "AND"))
        {
            //cout << 2 << endl;
            model = "AND";
            vector<string> argumentVector = splitString(inputArguments, "AND");
            arguments = new valueArgument*[argumentVector.size()];

            for (int i = 0; i < argumentVector.size(); i++)
            {
                arguments[i] = new valueArgument(argumentVector[i]);
                nArgument = i;
                //cout << i << endl;
                //arguments[i]->printCriterias();
            }
        }
        else
        {
            //cout << 3 << endl;
            model = "OR";
            arguments = new valueArgument*[1];

            arguments[0] = new valueArgument(inputArguments);
            nArgument = 0;
            //arguments[0]->printCriterias();
        }
    }
    
    ~valueArguments() {
        for(int i = 0; i < nArgument + 1; i++) {
            delete arguments[i];
        }
        
        delete[] arguments;
    }
    
    virtual bool judge(string &stringInLine)
    {
        if ((stringInLine == "-") | (stringInLine.length() == 0))
            return false;
        
        long int valueInLine = stol(stringInLine);
        
        if (model == "OR")
        {
            for (int i = 0; i <= nArgument; i++)
            {
                //arguments[i].printCriterias();
                if (arguments[i]->checkArgument(valueInLine))
                    return true;
            }
            return false;
        }
        
        else
        {
            for (int i = 0; i <= nArgument; i++)
            {
                //arguments[i]->printCriterias();
                if (!(arguments[i]->checkArgument(valueInLine)))
                    return false;
            }
            return true;
        }
    }
};

class stringArguments: public filterType
{
public:
    
    vector<string> arguments;
    
    stringArguments(string inputArguments)
    {
        if (KMPInclude(inputArguments, ","))
            arguments = splitString(inputArguments, ",");
        else
            arguments.push_back(inputArguments);
    }
    
    virtual bool judge(string &stringInLine)
    {
        for (int i = 0; i < arguments.size(); i++)
        {
            if (stringInLine == arguments[i])
                return true;
        }
        return false;
    }
};


class pass: public filterType
{
public:
    virtual bool judge(string &inputString)
    {
        return true;
    }
};


class publicFilter: public filterType
{
public:
    virtual bool judge (string &inputString)
    {
        if (inputString == "public")
            return true;
        return false;
    }
};


void findIndexIn(string str, vector<string> &vect, int &index)
{
    auto it = find(vect.begin(), vect.end(), str);
    index = (int) distance(vect.begin(), it);
}

void printHelpMessage()
{
    string outputText =
    "This is the filter for SRA database.\n\n"
    "-i\t Mandatory argument. The directory of unfiltered SRA database. Should be tsv file.\n\n"
    "-o\t Mandatory argument. The directory of output filted SRA database.\n\n"
    "-index\t Mandatory argument. The directory of unfiltered SRA database index. Should be tsv file.\n\n"
    "-run\t Optional argument. The SRA run accession number. If you enter more than one run accession numbers, "
        "please seperate them by comma. For example: -run ERR925021,SRR9611265\n\n"
    "-sample\t Optional argument. The SRA sample accession number. If you enter more than one sample accession " "numbers, please seperate them by comma. For example: -sample SRS2530621,SRS3932940\n\n"
    "-experiment\t Optional argument. The SRA experiment accession number. If you enter more than one "
        "sample accession numbers, please seperate them by comma. "
        "For example: -experiment SRX3204478,SRX3204539\n\n"
    "-study\t Optional argument. The SRA study accession number. If you enter more than one study accession "
        "numbers, please seperate them by comma. For example: -study SRP212249,SRP212314\n\n"
    "-species\t Optional argument. If you enter more than one species, please seperate them by comma. "
        "please use '_' to seperate the word for each species, and it is case sensitive. "
        "If you with of filter the species name exactly equal to input, please add '-' in front of "
        "the species name. If no equal symbol in front of the species name, the program will output any "
        "species include the species name you entered."
        "for example, enter -sp =Homo_sapiens,Passerella_iliaca to get all SRA sepeies is exactly equal to "
        "Homo_sapiens or any species name include Passerella_iliaca.\n\n"
    "-spots\t Optional argument. You can choose to filter the number of spots by =, >, >=, <, <=. "
        "if you have more than one condition, please join them by AND or OR. "
        "Please use single quotation for your argument, for example: -spots '>100000AND<=10000000000' "
        "Please do NOT put AND and OR in same argument.\n\n"
    "-bases\t Optional argument. You can choose to filter the number of bases by =, >, >=, <, <=. "
        "if you have more than one condition, please join them by AND or OR. "
        "Please use single quotation for your argument, for example: -bases '>100000OR<=80000' "
        "Please do NOT put AND and OR in same argument. \n\n"
    "-all\t Optional argument. Output both controlled access and public accession. Output will only "
        "contain public accession by defualt.\n\n"
    "-seq\t Optional argument. Please choose Sequencing stategy from: "
        "AMPLICON,ATAC-seq,Bisulfite-Seq,CLONE,CLONEEND,CTS,ChIA-PET,ChIP,"
        "ChIP-Seq,DNase-Hypersensitivity,EST,FAIRE-seq,FINISHING,FL-cDNA,Hi-C,"
        "MBD-Seq,MNase-Seq,MRE-Seq,MeDIP-Seq,OTHER,POOLCLONE,RAD-Seq,RIP-Seq,"
        "RNA-Seq,SELEX,Synthetic-Long-Read,Targeted-Capture,"
        "Tethered Chromatin Conformation Capture,Tn-Seq,VALIDATION,"
        "WCS,WGA,WGS,WXS,miRNA-Seq,ncRNA-Seq,other."
        "Please seperate them by comma and do NOT put space in between. It is CASE-sensitive! "
        "For example: -seq ChIP-Seq,WGA,WGS\n\n"
    "-layout\t Optional argument. You can choose whether you want single end or paired end data. "
        "Please enter -layout single for single end data only, "
        "-layout paired for paired end data only. If there is no input for this argument, "
        "output data will include both single or paired end data\n";
    
    cout << outputText << endl;
}


void argumentParser (int &argc, const char *argv[], vector<string> &colNames, string &indexFileName,
                     string &outFileName, string &speciesString, filterType *filters[]){
    
    //filterType *filters [24] = {new pass};
    int index;
    findIndexIn("proj_accession_Visibility", colNames, index);
    filters[index] = new publicFilter;
    
    int i = 1;
    while (i < argc)
    {
        int index;
        
        if (strcmp(argv[i], "-index") == 0){
            indexFileName = argv[i+1];
        }
        if (strcmp(argv[i], "-o") == 0){
            outFileName = argv[i+1];
        }
        else if (strcmp(argv[i], "-species") == 0){
            speciesString = argv[i+1];
        }
        else if (strcmp(argv[i], "-run") == 0){
            findIndexIn("Run", colNames, index);
            filters[index] = new stringArguments(argv[i+1]);
        }
        else if (strcmp(argv[i], "-sample") == 0){
            findIndexIn("Sample", colNames, index);
            filters[index] = new stringArguments(argv[i+1]);
        }
        else if (strcmp(argv[i], "-study") == 0){
            findIndexIn("Study", colNames, index);
            filters[index] = new stringArguments(argv[i+1]);
        }
        else if (strcmp(argv[i], "-experiment") == 0){
            findIndexIn("Experiment", colNames, index);
            filters[index] = new stringArguments(argv[i+1]);
        }
        else if (strcmp(argv[i], "-all") == 0){
            findIndexIn("proj_accession_Visibility", colNames, index);
            filters[index] = new pass;
        }
        else if (strcmp(argv[i], "-layout") == 0){
            findIndexIn("LibraryLayout", colNames, index);
            string inputArgument;
            inputArgument = argv[i+1];
            transform(inputArgument.begin(), inputArgument.end(), inputArgument.begin(), ::toupper);
            filters[index] = new stringArguments(inputArgument);
        }
        else if (strcmp(argv[i], "-seq") == 0){
            findIndexIn("LibraryStrategy", colNames, index);
            filters[index] = new stringArguments(argv[i+1]);
        }
        else if (strcmp(argv[i], "-date") == 0){
            findIndexIn("proj_accession_Published", colNames, index);
            filters[index] = new valueArguments(argv[i+1]);
        }
        else if (strcmp(argv[i], "-spots") == 0){
            findIndexIn("Spots", colNames, index);
            filters[index] = new valueArguments(argv[i+1]);
        }
        else if (strcmp(argv[i], "-bases") == 0){
            findIndexIn("Bases", colNames, index);
            filters[index] = new valueArguments(argv[i+1]);
        }
        
        i++;
    }
}

void helpMessage(int &argc, const char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "--help") == 0)|(strcmp(argv[i], "-h") == 0)){
            printHelpMessage();
            exit(0);
        }
    }
}

string findinFileName (int &argc, const char *argv[])
{
    string inFileName;
    
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            inFileName = argv[i+1];
            return inFileName;
        }
    }
    return NULL;
}

bool filterString(string &inputString, string delimiter, filterType *filters[])
{
    vector<string> outputVector;
    size_t startPosition = 0;
    size_t endPosition = 0;
    string info;
    int i = -1;
    
    while ((endPosition = inputString.find(delimiter, startPosition)) != string::npos) {
        info = inputString.substr(startPosition, endPosition-startPosition);
        i++;
        if (!(filters[i]->judge(info)))
            return false;
        startPosition = endPosition + delimiter.length();
    }
    
    i++;
    info = inputString.substr(startPosition);
    if (!(filters[i]->judge(info)))
        return false;
    
    return true;
}


void readFilterSaveFile(ifstream &inFile, ofstream &outFile, vector<pair<int,int> > &indexes,
                        int &currentLine, filterType *filters[])
{
    if (indexes.size() == 0)
    {
        if (inFile.is_open())
        {
            string line;
            while (inFile.good()) {
                getline(inFile, line);
                currentLine ++;
                
                if (filterString(line, "\t", filters))
                    writeString(outFile, line);
            }
        }
    }
    
    else
    {
        for (int i = 0; i < indexes.size(); i++)
        {
            int startLine = indexes[i].first;
            int endLine = indexes[i].second;
            
            jumpToLine(inFile, startLine, currentLine);
            
            if (inFile.is_open())
            {
                string line;
                
                while ((inFile.good()) & (currentLine <= endLine)) {
                    
                    getline(inFile, line);
                    currentLine ++;
                    
                    if (filterString(line, "\t", filters))
                        writeString(outFile, line);
                }
            }
        }
    }
    
    inFile.close();
    outFile.close();
}


int main(int argc, const char * argv[]) {
    
    helpMessage(argc, argv);
    
    ifstream inFile;
    string inFileName = findinFileName(argc, argv);
    inFile.open(inFileName, ios_base::in);

    vector<string> colNames;
    getColNames(inFile, colNames);
    
    int currentLine = 1;
    
    ofstream outFile;
    string outFileName;
    string speciesString;
    string indexFileName;
    filterType *filters [24];
    for (int i = 0; i < 24; i++)
        filters[i] = new pass;

    argumentParser(argc, argv, colNames, indexFileName, outFileName, speciesString, filters);
    
    vector<pair<int,int> > indexes;
    indexes = speciesIndex(speciesString, indexFileName);
    
    outFile.open(outFileName, ios_base::out);
    writeVector(outFile, colNames);
    
    readFilterSaveFile(inFile, outFile, indexes, currentLine, filters);

    for (int i = 0; i < 24; i++)
        delete filters[i];
    return 0;
}
