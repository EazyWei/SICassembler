#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* COP3404 Project 2
* Simulates SIC assembler program pass two
* Accepts one command line argument
*
* @Author Jonathan Shih
* @Version 9/22/2020
*/

typedef struct Opcode { // defines symbol
    int address;
    int srcLine;
    char opcodeName[7];
    char symbolName[7];
    char operand[255];
    struct Opcode *next;
} Opcode;

int hashfun(char name[]){ // hash function
    int index;

    index = name[0] - 'A';
    
    return index;
}

typedef struct Node { // node for linked list
    struct Opcode *head;
    struct Opcode *tail;
    int count;
} Node;

typedef struct SymbolTable { // table of linked lists
    char symbolName[7];
    int address;
    struct SymbolTable *next;
} SymbolTable;

typedef struct OpcodeTable {
    char mnemonic[7];
    char objcode[3];

} OpcodeTable;

int checkForDupes(char *name, int address, SymbolTable *table) { // check for duplicate label names
    int hash = hashfun(name);
    SymbolTable *symbols;
    SymbolTable *prev;
    
    if(strcmp(table[hash].symbolName, "") == 0) { // base case
        strcpy(table[hash].symbolName, name);
        table[hash].next = NULL;
        return 0;
    }
    
    symbols = &table[hash];
    prev = symbols;

    do{
        if(strcmp(name, symbols->symbolName) == 0) { // if duplicate
            return 1;
        } 
        prev = symbols;
        symbols = symbols->next; // keep searching the list
    }
    while(symbols != NULL);

    SymbolTable *newSymbol = (SymbolTable*)malloc(sizeof(SymbolTable));
    strcpy(newSymbol->symbolName, name);
    newSymbol->address = address;
    newSymbol->next = NULL;
    prev->next = newSymbol; // if not dupe add new label name
    return 0;
}

int isValidSymbol(char *name, SymbolTable *table) { // check for duplicate label names
    int hash = hashfun(name);
    SymbolTable *symbols;
    SymbolTable *prev;
    
    if(strcmp(table[hash].symbolName, "") == 0) { // base case
        return 0;
    }
    
    symbols = &table[hash];

    do{
        if(strcmp(name, symbols->symbolName) == 0) { // if duplicate
            return 1;
        } 
        symbols = symbols->next; // keep searching the list
    }
    while(symbols != NULL);

    return 0;
}

struct Opcode *insertOpcode(struct Opcode *head, struct Opcode *newNode) { // insert symbol into linked list
    
    if(!head) {
        head = newNode;
        //printf("New Head Symbol added to Symbol Table: %s\n", head->name);
    }
    else {
        struct Opcode *prev = NULL;
        for(struct Opcode *current = head; current != NULL; current = current->next) {
            prev = current;
        }
        prev->next = newNode;
    }
    return head;
}

int isDirective(char *candidate) { // check if label has a directive name
    int result = 0;
    if(strcmp(candidate, "START") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "WORD") == 0) {
        result = 1;
    }    
    if(strcmp(candidate, "BYTE") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "RESB") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "RESW") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "RESR") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "EXPORTS") == 0) {
        result = 1;
    }
    return result;        
}

int isOpcode(char *candidate) { // check if label has a directive name
    int result = 0;
    if(strcmp(candidate, "STL") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "JSUB") == 0) {
        result = 1;
    }    
    if(strcmp(candidate, "LDA") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "COMP") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "JEQ") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "J") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "STA") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "LDL") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "RSUB") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "TD") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "JEQ") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "RD") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "STCH") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "TIX") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "JLT") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "STX") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "LDX") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "LDCH") == 0) {
        result = 1;
    }
    if(strcmp(candidate, "WD") ==0 ) {
        result = 1;
    }
    return result;        
}

char *stripWhiteSpace(char *token) { //strip spaces
    int len = strlen(token) - 1;
    for(int i=len;i>=0;i--){
        if(isspace(token[i])) {
            token[i] = '\0';
        }
        else {
            break;
        }
    }
    return token;
}

//int main(int argc, char *argv[]) { // start main
int main() {
    
    /*if(argc != 2) {
        printf("USAGE: %s <filename>\n", argv[0]);
        return 1;
    }*/

    FILE *inputFile = fopen("/Users/jonnyshih/Desktop/UNF-CS/SourceCode/COP3404P2/Clock.sic", "r"); // open input file

    /*if(!inputFile) {
        printf("ERROR: Could not open %s for reading\n", argv[1]);
        return 1;
    }*/

    int locctr = 0;
    char line[1024];
    char name[6+1];
    int srcLine = 0;
    SymbolTable table[26];
    Opcode *headOpcode = NULL;
    Opcode *currentOpcode = NULL;

    //Proj 2 variables
    OpcodeTable optable[2] = {{"LDX","04"},{"LDCH","50"}};
    int startadr = 0;
    int codeadr = 0;

    for(int i=0; i<26; i++){ // initialize symbol table
        strcpy(table[i].symbolName, "");
        table[i].next = NULL;
    }

    while(fgets(line, 1024, inputFile)) { // read from line
        char *token[3];
        ++srcLine;
        int length = strlen(line);
        
        if (length == 0 || line[0] == '#') { // continue if blank or comment
            continue;
        }

        int hasLabel = 0;

        if(line[0] >= 'A' && line[0] <= 'Z') { // check for label
            hasLabel = 1;
            //printf("--had a symbol definition\n");
        }

        if(hasLabel) { // if a label line
            struct Opcode *tempOpcode = malloc(sizeof(struct Opcode));

            token[0] = strtok(line, " \t\n");
    
            //printf("-first token is %s\n", token[0]);

            strcpy(tempOpcode->symbolName, token[0]); // pass str token to name
            //printf("---current symbol is %s\n", tempSymbol->name);

            int directive = isDirective(tempOpcode->symbolName);

            if(directive) { // if label matches directive name then error
                printf("ASSEMBLY ERROR: Label name %s is a reserved assembler directive on line %d\n", tempOpcode->symbolName, srcLine);
                exit(1);
            }

            int isDuplicate =  checkForDupes(tempOpcode->symbolName, tempOpcode->address, table); // check if duplicate label
            
            if(!isDuplicate) { // if label not duplicate then insert to list
                strcpy(tempOpcode->opcodeName, "");
                tempOpcode->next = NULL;
                tempOpcode->srcLine = srcLine;
                //tempSymbol->address = locctr;
                //printf("New Label added to Symbol Table: %s\n", tempSymbol->name);
            }
            else {
                printf("ASSEMBLY ERROR: Duplicate label name %s found on line %d.\n", tempOpcode->symbolName, srcLine);
                exit(1);
            }

            token[1] = strtok(NULL, " \t\n\r");
            
            //struct Symbol *tempOpcode = malloc(sizeof(struct Opcode));
            
            strcpy(tempOpcode->opcodeName, token[1]); // read opcode
            //printf("---current opcode is %s\n", tempOpcode->name);        

            token[2] = strtok(NULL, "\t\n\r"); // operand token
            
            if(token[2] != NULL) {
            strcpy(tempOpcode->operand, stripWhiteSpace(token[2])); // read operand
            }
            else{
                strcpy(tempOpcode->operand, ""); // else operand is blank
            }

            tempOpcode->next = NULL;
            if(headOpcode == NULL) {
                headOpcode = tempOpcode;
            }
            else {
                currentOpcode->next = tempOpcode;
            }
            currentOpcode = tempOpcode;
        }
        
        else{ // if not a label line
            /*if(currentSymbol == NULL) {
                printf("ERROR: No label before START.\n");
            }*/
            
            struct Opcode *tempOpcode = malloc(sizeof(struct Opcode));

            token[0] = strtok(line, " \t\n\r");
    
            //printf("-first token is %s\n", token[0]);

            strcpy(tempOpcode->opcodeName, token[0]); // read opcode
            strcpy(tempOpcode->symbolName, ""); // no label

            token[1] = strtok(NULL, "\t\n\r"); // operand
            
            if(token[1] != NULL) {
            strcpy(tempOpcode->operand, stripWhiteSpace(token[1])); // read operand
            }
            else{
                strcpy(tempOpcode->operand, "");
            }
            
            tempOpcode->next = NULL;
            if(headOpcode == NULL) {
                headOpcode = tempOpcode;
            }
            else {
                currentOpcode->next = tempOpcode;
            }
            currentOpcode = tempOpcode;
        }
    }
   
    Opcode *thisOpcode = headOpcode;
    
    #if 0
    do{ // for debugging
        printf("The label is: %s\n", thisSymbol->name);
        Symbol *thisOpcode = thisSymbol->opcode;
        thisSymbol = thisSymbol->next;
        if(thisOpcode != NULL) {
            do{
                printf("The opcode is: %s and operand is: %s\n", thisOpcode->name, thisOpcode->operand); 
                thisOpcode = thisOpcode->next;
            }
            while(thisOpcode != NULL);
        }

    }
    while (thisSymbol != NULL);
    #endif

   locctr = 0;
    
    if(thisOpcode != NULL) { // read start address
        if(strcmp(thisOpcode->opcodeName, "START") == 0) {
            startadr = (int)strtol(thisOpcode->operand, NULL, 16);
            //printf("---the value of int is: %d\n", value); // commented out Pass 1
            locctr = startadr;
            }
        else {
            printf("ERROR: Missing START instruction. Start address set to 0.\n"); // if not START give program a name
            startadr = 0;
        }
    }
    else {
            printf("ERROR: Missing START instruction. Start address set to 0.\n"); // if not START give program a name
            startadr = 0;
    }

    while(thisOpcode != NULL){ // set locctr based on opcode and operand
        thisOpcode->address = locctr;
        if(strcmp(thisOpcode->symbolName, "")!=0) {
        //printf("%-6s     %X\n", thisOpcode->symbolName, thisOpcode->address);
        }
        if(!isDirective(thisOpcode->opcodeName) && codeadr == 0) {
            codeadr = locctr;
        }
        if(strcmp(thisOpcode->opcodeName, "WORD") == 0) {
            int value = (int)strtol(thisOpcode->operand, NULL, 10);
            if(value > 32767) {
                printf("ASSEMBLY ERROR: Memory location address is out of bounds.\n");
                exit(1);
            }
            thisOpcode->address = locctr;
            locctr += 3;
        }
        else if(strcmp(thisOpcode->opcodeName, "RESW") == 0) {
            int value = (int)strtol(thisOpcode->operand, NULL, 10);
            thisOpcode->address = locctr;
            locctr += 3 * value;
            //printf("---the value of locctr is: %d\n", locctr);
        }
        else if(strcmp(thisOpcode->opcodeName, "RESB") == 0) {
            int value = (int)strtol(thisOpcode->operand, NULL, 10);
            thisOpcode->address = locctr;
            locctr += 1 * value;
            //printf("---the value of locctr is: %d\n", locctr);
        }
        else if(strcmp(thisOpcode->opcodeName, "BYTE") == 0) {
            char type = thisOpcode->operand[0];
            //printf("Operand: %s\n", thisOpcode->operand);
            thisOpcode->address = locctr;
            if(type == 'C') {
                strtok(thisOpcode->operand, "'");
                char token[255];
                strcpy(token, strtok(NULL, "'"));
                strcpy(thisOpcode->operand, token);
                //printf("Byte operand: %s\n", thisOpcode->operand);
                locctr += strlen(thisOpcode->operand);
            }
            else if(type == 'X') {
                strtok(thisOpcode->operand, "'");
                char token[20];
                strcpy(token, strtok(NULL, "'"));
                strcpy(thisOpcode->operand, token);
                locctr += strlen(thisOpcode->operand) / 2;
                int len = strlen(thisOpcode->operand);
                for(int i=0;i<len;i++){
                    if(!((thisOpcode->operand[i] >= '0' && thisOpcode->operand[i] <= '9') || 
                    (thisOpcode->operand[i] >= 'A' && thisOpcode->operand[i] <= 'F'))) {
                        printf("ASSEMBLY ERROR: Invalid Hex value: %s.\n", thisOpcode->operand);
                        exit(1);
                    }
                }
            }
        }
        else if(strcmp(thisOpcode->opcodeName, "TIX") == 0) {
            thisOpcode->address = locctr;
            if(!isValidSymbol(thisOpcode->operand, table)) {
                printf("ASSEMBLY ERROR: No matching label %s found.\n", thisOpcode->operand);
                exit(1);
            }
        }
        else if(strcmp(thisOpcode->opcodeName, "END") == 0) {
            thisOpcode->address = locctr;
            if(!isValidSymbol(thisOpcode->operand, table)) {
                printf("ASSEMBLY ERROR: No matching label found: %s.\n", thisOpcode->operand);
                exit(1);
            }
        }
        /*else if(strcmp(thisOpcode->name, "LDX") == 0) {
            if(!isValidSymbol(thisOpcode->operand, table)) {
                printf("ASSEMBLY ERROR: No matching label found: %s.\n", thisOpcode->operand);
                exit(1);
            }
        }*/
        else if (strcmp(thisOpcode->opcodeName, "START") != 0) {
            thisOpcode->address = locctr;
            locctr += 3;
        }
        if (locctr > 32767) {
            printf("ASSEMBLY ERROR: Location address has exceeded memory space.\n");
            exit(1);
        }
        thisOpcode = thisOpcode->next;
    }

    fclose(inputFile);

    //Project 2 begin

    /*char *stradr = NULL;
    thisSymbol = headSymbol;
    while(thisSymbol!=NULL){
        if(!strcmp(thisSymbol->opcode, "START")) {
            stradr = thisOpcode->operand;
            break;
        }
        thisSymbol = thisSymbol->next;
    }*/
    thisOpcode = headOpcode;

    while(thisOpcode != NULL){ // check for symbols that don't exist
        if(isOpcode(thisOpcode->opcodeName)) {
            char *symbol = strtok(thisOpcode->operand, ",");
            if(symbol != NULL) {
                if(strcmp(symbol, "") != 0) {
                    if(!isValidSymbol(symbol, table)) {
                        printf("ASSEMBLY ERROR: Label not found: %s.\n", symbol);
                        exit(1);
                    }
                }
            }
        }
        thisOpcode = thisOpcode->next;
    }
    
    int length = strlen(argv[1])+5;
    char *testfile = calloc(length, sizeof(char));
    strcpy(testfile, argv[1]);
    char *outputFilename = strcat(testfile, ".obj"); // get filename
    FILE *out = fopen(outputFilename, "w"); // output file

    //print to obj
    fprintf(out, "H%-6s%06X%06X\n", headOpcode->symbolName, startadr, (locctr - startadr));

    thisOpcode = headOpcode;

    int max = 0; // max opcodes per line

    //Opcode *lastOpcode = NULL;

    //locctr = thisOpcode->address;
    /*for(lastOpcode = NULL, thisOpcode = headOpcode;
        thisOpcode != NULL;
        lastOpcode = thisOpcode, thisOpcode = thisOpcode->next) {*/
    while(thisOpcode != NULL){ // print object code
        /*if(locctr != thisOpcode->address) {
            //assert(locctr==thisOpcode->address);
        }*/
        if(strcmp(thisOpcode->opcodeName, "BYTE") == 0) { // BYTE case (need to limit to 69)
            int len = strlen(thisOpcode->operand);
            int numlines = len/30;
            //if(len > 30) { // long byte
                for(int i=0; i<=numlines; i++) {
                    fprintf(out, "T%06X%02X", thisOpcode->address, ((int)strlen(thisOpcode->operand)) > 30 ? 30 : ((int)strlen(thisOpcode->operand))); 
                        for(int j = 0; (j<strlen((char*)&(thisOpcode->operand[i*30]))) && (j<30); j++) {
                        fprintf(out, "%2X", (int)thisOpcode->operand[i*30+j]);
                        }
                fprintf(out, "\n");
                }
            //}
            thisOpcode = thisOpcode->next;
        }
        else if(strcmp(thisOpcode->opcodeName, "WORD") == 0) { // WORD case
            int wordValue = (int)strtol(thisOpcode->operand, NULL, 10);
            fprintf(out, "T%06X03%06X", thisOpcode->address, wordValue);
            fprintf(out, "\n");
            thisOpcode = thisOpcode->next;
        }
        /*else if(strcmp(thisOpcode->opcodeName, "RESB") == 0) {
            thisOpcode = thisOpcode->next;
        }
        else if(strcmp(thisOpcode->opcodeName, "RESW") == 0) {
            thisOpcode = thisOpcode->next;
        }*/
        else if(isOpcode(thisOpcode->opcodeName)) { // OPCODE CASE
            int opCount = 0;
            int maxCount = 30;
            Opcode* currentOpcode = thisOpcode;
            while(currentOpcode != NULL) {
                if(isOpcode(currentOpcode->opcodeName)) {
                    opCount += 3;
                    currentOpcode = currentOpcode->next;
                }
                else {
                    break;
                }
            }
            fprintf(out, "T%06X", thisOpcode->address); // print symbol address
            fprintf(out, "%02X", opCount > 30 ? 30 : opCount); // print record length
            while(thisOpcode != NULL) {
                if(isOpcode(thisOpcode->opcodeName)) { 
                    if(strcmp(thisOpcode->opcodeName, "STL") == 0) {
                        fprintf(out, "14");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "JSUB") == 0) {
                        fprintf(out, "48");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }    
                    if(strcmp(thisOpcode->opcodeName, "LDA") == 0) {
                        fprintf(out, "00");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "COMP") == 0) {
                        fprintf(out, "28");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "J") == 0) {
                        fprintf(out, "3C");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "STA") == 0) {
                        fprintf(out, "OC");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "LDL") == 0) {
                        fprintf(out, "08");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "RSUB") == 0) { // does not contain operand
                        fprintf(out, "4C0000");
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                    }
                    if(strcmp(thisOpcode->opcodeName, "TD") == 0) {
                        fprintf(out, "E0");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "JEQ") == 0) {
                        fprintf(out, "30");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "RD") == 0) {
                        fprintf(out, "D8");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "STCH") == 0) { // X
                        fprintf(out, "54");
                        char *token = strtok(thisOpcode->operand, ",");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", (tempOpcode->address + 0x8000));
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "TIX") == 0) {
                        fprintf(out, "2C");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "JLT") == 0) {
                        fprintf(out, "38");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "STX") == 0) {
                        fprintf(out, "10");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "LDX") == 0) {
                        fprintf(out, "04");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "LDCH") == 0) { //X
                        fprintf(out, "50");
                        char *token = strtok(thisOpcode->operand, ",");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", (tempOpcode->address + 0x8000));
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(strcmp(thisOpcode->opcodeName, "WD") == 0) {
                        fprintf(out, "DC");
                        Opcode* tempOpcode = headOpcode;
                        max++;
                        opCount -= 3;
                        //locctr += 3;
                        do {
                            if(strcmp(thisOpcode->operand, tempOpcode->symbolName) == 0) {
                                fprintf(out, "%X", tempOpcode->address);
                                break;
                            }
                            tempOpcode = tempOpcode->next;
                        }
                        while(tempOpcode != NULL);
                    }
                    if(max>=10){
                        fprintf(out, "\n");
                        fprintf(out, "T%06X%02X", thisOpcode->address + 3, opCount > 30 ? 30 : opCount);
                        max=0;
                    }
                    thisOpcode = thisOpcode->next;
                }
                else {
                    fprintf(out, "\n"); 
                    break;
                    max=0;
                }
            }
        }
        else{
            thisOpcode = thisOpcode->next;
            max=0;
        }
    }
    fprintf(out, "E%06X\n", codeadr);

    fclose(out);
}