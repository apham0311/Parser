#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

instruction *code;
//current index
int cIndex;
//symbol table
symbol *table;
//table size
int tIndex;

int tokenCounter;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

void program(lexeme *list);
void block(lexeme *list, int level);
void const_declaration(lexeme *list, int level);
int var_declaration(lexeme *list, int level);
void procedure_declaration(lexeme *list, int level);
void statement(lexeme *list, int level);
void condition(lexeme *list, int level);
void expression(lexeme *list, int level);
void term(lexeme *list, int level);
void factor(lexeme *list, int level);

int multipleDeclarationCheck(lexeme token, int level);
int findSymbol(lexeme token, int kind);
void mark(int level);



instruction *parse(lexeme *list, int printTable, int printCode)
{
    code = malloc(MAX_CODE_LENGTH*sizeof(instruction));
    table = malloc(MAX_SYMBOL_COUNT*sizeof(symbol));
    //begin parsing
    program(list);
    //only prints if -s directive is present
    if(printTable)
        printsymboltable();
    //only prints if -a directive is present
    if(printCode)
        printassemblycode();
    code[cIndex].opcode = -1;
    return code;
}


void emit(int opname, int level, int mvalue)
{
    code[cIndex].opcode = opname;
    code[cIndex].l = level;
    code[cIndex].m = mvalue;
    cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
    table[tIndex].kind = k;
    strcpy(table[tIndex].name, n);
    table[tIndex].val = v;
    table[tIndex].level = l;
    table[tIndex].addr = a;
    table[tIndex].mark = m;
    tIndex++;
}


void printparseerror(int err_code){
    switch (err_code)
    {
        case 1:
            printf("Parser Error: Program must be closed by a period\n");
            break;
        case 2:
            printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
            break;
        case 3:
            printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
            break;
        case 4:
            printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
            break;
        case 5:
            printf("Parser Error: Variables must be assigned using :=\n");
            break;
        case 6:
            printf("Parser Error: Only variables may be assigned to or read\n");
            break;
        case 7:
            printf("Parser Error: call must be followed by a procedure identifier\n");
            break;
        case 8:
            printf("Parser Error: if must be followed by then\n");
            break;
        case 9:
            printf("Parser Error: while must be followed by do\n");
            break;
        case 10:
            printf("Parser Error: Relational operator missing from condition\n");
            break;
        case 11:
            printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
            break;
        case 12:
            printf("Parser Error: ( must be followed by )\n");
            break;
        case 13:
            printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
            break;
        case 14:
            printf("Parser Error: Symbol declarations should close with a semicolon\n");
            break;
        case 15:
            printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
            break;
        case 16:
            printf("Parser Error: begin must be followed by end\n");
            break;
        case 17:
            printf("Parser Error: Bad arithmetic\n");
            break;
        case 18:
            printf("Parser Error: Confliciting symbol declarations\n");
            break;
        case 19:
            printf("Parser Error: Undeclared identifier\n");
            break;
        default:
            printf("Implementation Error: unrecognized error code\n");
            break;
    }
    printsymboltable();
    printassemblycode();
    free(code);
    free(table);
    //ends program upon error
    exit(0);
}

void printsymboltable()
{
    int i;
    printf("Symbol Table:\n");
    printf("Kind | Name        | Value | Level | Address | Mark\n");
    printf("---------------------------------------------------\n");
    for (i = 0; i < tIndex; i++)
        printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

    free(table);
    table = NULL;
}

void printassemblycode()
{
    int i;
    printf("Line\tOP Code\tOP Name\tL\tM\n");
    for (i = 0; i < cIndex; i++)
    {
        printf("%d\t", i);
        printf("%d\t", code[i].opcode);
        switch (code[i].opcode)
        {
            case 1:
                printf("LIT\t");
                break;
            case 2:
                switch (code[i].m)
                {
                    case 0:
                        printf("RTN\t");
                        break;
                    case 1:
                        printf("NEG\t");
                        break;
                    case 2:
                        printf("ADD\t");
                        break;
                    case 3:
                        printf("SUB\t");
                        break;
                    case 4:
                        printf("MUL\t");
                        break;
                    case 5:
                        printf("DIV\t");
                        break;
                    case 6:
                        printf("ODD\t");
                        break;
                    case 7:
                        printf("MOD\t");
                        break;
                    case 8:
                        printf("EQL\t");
                        break;
                    case 9:
                        printf("NEQ\t");
                        break;
                    case 10:
                        printf("LSS\t");
                        break;
                    case 11:
                        printf("LEQ\t");
                        break;
                    case 12:
                        printf("GTR\t");
                        break;
                    case 13:
                        printf("GEQ\t");
                        break;
                    default:
                        printf("err\t");
                        break;
                }
                break;
            case 3:
                printf("LOD\t");
                break;
            case 4:
                printf("STO\t");
                break;
            case 5:
                printf("CAL\t");
                break;
            case 6:
                printf("INC\t");
                break;
            case 7:
                printf("JMP\t");
                break;
            case 8:
                printf("JPC\t");
                break;
            case 9:
                switch (code[i].m)
                {
                    case 1:
                        printf("WRT\t");
                        break;
                    case 2:
                        printf("RED\t");
                        break;
                    case 3:
                        printf("HAL\t");
                        break;
                    default:
                        printf("err\t");
                        break;
                }
                break;
            default:
                printf("err\t");
                break;
        }
        printf("%d\t%d\n", code[i].l, code[i].m);
    }
    if (table != NULL)
        free(table);
}

void program(lexeme *list){
    int level = 0;
    cIndex = 0;
    tIndex = 0;
    tokenCounter = 0;
    emit(7, level, 0); //JMP
    addToSymbolTable(3, "main", 0, level, 0, 0);
    level = -1;
    //begins reading
    block(list, level);
    //checks for period at end of program
    if (list[tokenCounter].type != periodsym){
        printparseerror(1);
    }
    emit(9, 0, 3); //exit program instruction

    //fixes CAL instructions
    for(int i = 0; i < cIndex; i++){
        if(code[i].opcode == 5){
            code[i].m = table[code[i].m].addr;
        }
    }
    //fixes JMP m at start of code
    code[0].m = table[0].addr;
}

void block(lexeme *list, int level){
    level++;
    int procedure_idx = tIndex - 1;

    const_declaration(list, level);
    int x = var_declaration(list, level);
    procedure_declaration(list, level);
    table[procedure_idx].addr = cIndex*3;
    if(level == 0){
        emit(6, 0, x); //INC
    }
    else{
        emit(6, 0, x + 3);//INC
    }
    statement(list, level);
    mark(level);
    level--;
}

void const_declaration(lexeme *list, int level){
    if(list[tokenCounter].type == constsym){
        do{
            tokenCounter++;
            if(list[tokenCounter].type != identsym){
                printparseerror(2);
            }
            int symidx = multipleDeclarationCheck(list[tokenCounter], level);
            if(symidx != -1){
                printparseerror(18);
            }

            char *identName = list[tokenCounter].name;
            tokenCounter++;

            if(list[tokenCounter].type != assignsym){
                printparseerror(2);
            }
            tokenCounter++;

            if(list[tokenCounter].type != numbersym){
                printparseerror(2);
            }

            addToSymbolTable(1, identName, list[tokenCounter].value, level, 0, 0);
            tokenCounter++;
        } while(list[tokenCounter].type == commasym);

        if(list[tokenCounter].type != semicolonsym){
            if(list[tokenCounter].type == identsym){
                printparseerror(13);
            }
            else{
                printparseerror(14);
            }
        }
        tokenCounter++;
    }
}

int var_declaration(lexeme *list, int level){
    int numVars = 0;

    if(list[tokenCounter].type == varsym){
        do{
            numVars++;
            tokenCounter++;
            if(list[tokenCounter].type != identsym){
                printparseerror(3);
            }
            int symidx = multipleDeclarationCheck(list[tokenCounter], level);
            if(symidx != -1){
                printparseerror(18);
            }
            if(level == 0){
                addToSymbolTable(2, list[tokenCounter].name, 0, level, numVars - 1, 0);
            }
            else{
                addToSymbolTable(2, list[tokenCounter].name, 0, level, numVars + 2, 0);
            }
            tokenCounter++;
        } while(list[tokenCounter].type == commasym);

        if(list[tokenCounter].type != semicolonsym){
            if(list[tokenCounter].type == identsym){
                printparseerror(13);
            }
            else{
                printparseerror(14);
            }
        }
        tokenCounter++;
    }
    return numVars;
}

void procedure_declaration(lexeme *list, int level){
    while(list[tokenCounter].type == procsym){
        tokenCounter++;
        if(list[tokenCounter].type != identsym){
            printparseerror(4);
        }
        int symidx = multipleDeclarationCheck(list[tokenCounter], level);
        if(symidx != -1){
            printparseerror(18);
        }
        addToSymbolTable(3, list[tokenCounter].name, 0, level, 0, 0);
        tokenCounter++;

        if(list[tokenCounter].type != semicolonsym){
            printparseerror(4);
        }
        tokenCounter++;
        block(list, level);
        if(list[tokenCounter].type != semicolonsym){
            printparseerror(14);
        }
        tokenCounter++;
        emit(2, level, 0); //RTN
    }
}
void statement(lexeme *list, int level)
{
    if(list[tokenCounter].type == identsym)
    {
        int symIdx = findSymbol(list[tokenCounter], 2);
        if(symIdx == -1)
        {
            if(findSymbol(list[tokenCounter], 1) != findSymbol(list[tokenCounter], 3))
                printparseerror(6);
            else
                printparseerror(19);
        }
        tokenCounter++;
        if(list[tokenCounter].type != assignsym)
            printparseerror(5);
        tokenCounter++;
        expression(list, level);
        emit(4, level - table[symIdx].level, table[symIdx].addr); //STO
        return;
    }
    if(list[tokenCounter].type == beginsym)
    {
        do {
            tokenCounter++;
            statement(list, level);
        } while(list[tokenCounter].type == semicolonsym);
        if(list[tokenCounter].type != endsym)
            if(list[tokenCounter].type == identsym || list[tokenCounter].type == beginsym || list[tokenCounter].type == ifsym || list[tokenCounter].type == whilesym || list[tokenCounter].type == readsym || list[tokenCounter].type == writesym || list[tokenCounter].type == callsym)
                printparseerror(15);
            else
                printparseerror(16);
        tokenCounter++;
        return;
    }
    if(list[tokenCounter].type == ifsym)
    {
        tokenCounter++;
        condition(list, level);
        int jpcIdx = cIndex;
        emit(8, level, 0); //JPC
        if(list[tokenCounter].type != thensym)
            printparseerror(8);
        tokenCounter++;
        statement(list, level);
        if(list[tokenCounter].type == elsesym)
        {
            int jmpIdx = cIndex;
            emit(7, level, 0); //JMP
            code[jpcIdx].m = cIndex * 3;
            tokenCounter++;
            statement(list, level);
            code[jmpIdx].m = cIndex * 3;
        }
        else
            code[jpcIdx].m = cIndex * 3;
        return;
    }
    if(list[tokenCounter].type == whilesym)
    {
        tokenCounter++;
        int loopIdx = cIndex;
        condition(list, level);
        if(list[tokenCounter].type != dosym)
            printparseerror(9);
        tokenCounter++;
        int jpcIdx = cIndex;
        emit(8, level, 0); //JPC
        statement(list, level);
        emit(7, level, loopIdx * 3); //JMP
        code[jpcIdx].m = cIndex * 3;
        return;
    }
    if(list[tokenCounter].type == readsym)
    {
        tokenCounter++;
        if (list[tokenCounter].type != identsym)
            printparseerror(6);
        int symIdx = findSymbol(list[tokenCounter], 2);
        if(symIdx == -1)
        {
            if(findSymbol(list[tokenCounter], 1) != findSymbol(list[tokenCounter], 3))
                printparseerror(6);
            else
                printparseerror(19);
        }
        tokenCounter++;
        emit(9, level, 2); //SYS code for input
        emit(4, level - table[symIdx].level, table[symIdx].addr); //STO
        return;
    }
    if(list[tokenCounter].type == writesym)
    {
        tokenCounter++;
        expression(list, level);
        emit(9, level, 1); //SYS code for print
        return;
    }
    if(list[tokenCounter].type == callsym)
    {
        tokenCounter++;
        int symIdx = findSymbol(list[tokenCounter], 3);
        if(symIdx == -1)
            if(findSymbol(list[tokenCounter], 1) != findSymbol(list[tokenCounter], 2))
                printparseerror(7);
            else
                printparseerror(19);
        tokenCounter++;
        emit(5, level - table[symIdx].level, symIdx/*table[symIdx].addr*/); //CAL
        return;
    }
}
void condition(lexeme *list, int level)
{
    if(list[tokenCounter].type == oddsym)
    {
        tokenCounter++;
        expression(list, level);
        emit(2, level, 6); //ODD
    }
    else
    {
        expression(list, level);
        if(list[tokenCounter].type == eqlsym)
        {
            tokenCounter++;
            expression(list, level);
            emit(2, level, 8); //EQL
        }
        else if(list[tokenCounter].type == neqsym)
        {
            tokenCounter++;
            expression(list, level);
            emit(2, level, 9); //NEQ
        }
        else if(list[tokenCounter].type == lsssym)
        {
            tokenCounter++;
            expression(list, level);
            emit(2, level, 10); //LSS
        }
        else if(list[tokenCounter].type == leqsym)
        {
            tokenCounter++;
            expression(list, level);
            emit(2, level, 11); //LEQ
        }
        else if(list[tokenCounter].type == gtrsym)
        {
            tokenCounter++;
            expression(list, level);
            emit(2, level, 12); //GTR
        }
        else if(list[tokenCounter].type == geqsym)
        {
            tokenCounter++;
            expression(list, level);
            emit(2, level, 13); //GEQ
        }
        else
            printparseerror(10);
    }
}
void expression(lexeme *list, int level)
{
    if (list[tokenCounter].type == subsym)
    {
        tokenCounter++;
        term(list, level);
        emit(2, level, 1); //NEG

        while (list[tokenCounter].type == addsym || list[tokenCounter].type == subsym)
        {
            if (list[tokenCounter].type == addsym)
            {
                tokenCounter++;
                term(list, level);
                emit(2, level, 2); //ADD
            }
            else
            {
                tokenCounter++;
                term(list, level);
                emit(2, level, 3); //SUB
            }
        }
    }
    else
    {
        if (list[tokenCounter].type == addsym)
        {
            tokenCounter++;
        }
        term(list, level);

        while (list[tokenCounter].type == addsym || list[tokenCounter].type == subsym)
        {
            if (list[tokenCounter].type == addsym)
            {
                tokenCounter++;
                term(list, level);
                emit(2, level, 2); //ADD
            }
            else {
                tokenCounter++;
                term(list, level);
                emit(2, level, 3); //SUB
            }
        }
    }
    if (list[tokenCounter].type == addsym || list[tokenCounter].type == subsym || list[tokenCounter].type == multsym || list[tokenCounter].type == divsym || list[tokenCounter].type == modsym || list[tokenCounter].type == lparensym || list[tokenCounter].type == oddsym)
    {
        printparseerror(17);
    }
    return;
}


void term(lexeme *list, int level)
{
    factor(list, level);
    while (list[tokenCounter].type == multsym || list[tokenCounter].type == divsym || list[tokenCounter].type == modsym)
    {
        if (list[tokenCounter].type == multsym)
        {
            tokenCounter++;
            factor(list, level);
            emit(2, level, 4); //MUL
        }
        else if (list[tokenCounter].type == divsym)
        {
            tokenCounter++;
            factor(list, level);
            emit(2, level, 5); //DIV
        }
        else
        {
            tokenCounter++;
            factor(list, level);
            emit(2, level, 7); //MOD
        }
    }
    return;
}

void factor(lexeme *list, int level)
{
    if (list[tokenCounter].type == identsym)
    {
        int symIdx_var = findSymbol(list[tokenCounter], 2);
        int symIdx_const = findSymbol(list[tokenCounter], 1);

        if (symIdx_var == -1 && symIdx_const == -1)
        {
            if (findSymbol(list[tokenCounter], 3) != -1) {
                printparseerror(11);
            }
            else {
                printparseerror(19);
            }
        }
        //no variable found
        if (symIdx_var == -1) {
            emit(1, level, table[symIdx_const].val); //LIT
        }
            //no constant found or variable's level is greater than constant's level
        else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level) {
            emit(3, level - table[symIdx_var].level, table[symIdx_var].addr); //LOD
        }
            //constant found and constant's level is greater than variable's level
        else {
            emit(1, level, table[symIdx_const].val); //LIT
        }
        tokenCounter++;
    }

    else if (list[tokenCounter].type == numbersym)
    {
        emit(1, level, list[tokenCounter].value); //LIT
        tokenCounter++;
    }

    else if (list[tokenCounter].type == lparensym)
    {
        tokenCounter++;
        expression(list, level);
        if (list[tokenCounter].type != rparensym)
        {
            printparseerror(12);
        }
        tokenCounter++;
    }
    else {
        printparseerror(11);
    }
    return;
}


int multipleDeclarationCheck(lexeme token, int level){
    //goes through entire table
    for(int i = 0; i < tIndex; i++){
        //if name and level match, and token is unmarked, returns index
        if(token.name == table[i].name){
            if(table[i].mark == 0){
                if(table[i].level == level){
                    return i;
                }
            }
        }
    }
    return -1;
}
int findSymbol(lexeme token, int kind)
{
    int maxLevel = -1;
    int index = -1;
    //searches entire table
    for(int i = 0; i < tIndex; i++){
        if(table[i].mark == 0)
            //if name and kind match
            if(strcmp(table[i].name, token.name) == 0)
                if(table[i].kind == kind)
                    //if level is greater than previously found levels
                    if(table[i].level > maxLevel)
                    {
                        maxLevel = table[i].level;
                        index = i;
                    }
    }
    //return found symbol
    return index;
}
void mark(int level)
{
    //searches table from highest index to zero
    for(int i = tIndex; i >= 0; i--){
        //if unmarked
        if(table[i].mark == 0)
        {
            //if level is same as given level, mark
            if(table[i].level == level)
                table[i].mark = 1;
                //if level is less than given level exit function
            else if(table[i].level < level)
                return;
            //if level is greater than gien level do not mark
        }
    }
}
