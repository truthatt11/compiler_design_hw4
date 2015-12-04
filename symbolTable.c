#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// This file is for reference only, you are not required to follow the implementation. //

int HASH(char * str) {
	int idx=0;
	while (*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}
	return (idx & (HASH_TABLE_SIZE-1));
}

SymbolTable symbolTable;

SymbolTableEntry* newSymbolTableEntry(int nestingLevel)
{
    SymbolTableEntry* symbolTableEntry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
    symbolTableEntry->nextInHashChain = NULL;
    symbolTableEntry->prevInHashChain = NULL;
    symbolTableEntry->nextInSameLevel = NULL;
    symbolTableEntry->sameNameInOuterLevel = NULL;
    symbolTableEntry->attribute = NULL;
    symbolTableEntry->name = NULL;
    symbolTableEntry->nestingLevel = nestingLevel;
    return symbolTableEntry;
}

void removeFromHashTrain(int hashIndex, SymbolTableEntry* entry)
{
}

void enterIntoHashTrain(int hashIndex, SymbolTableEntry* entry)
{
}

void initializeSymbolTable()
{
    int i;
    for(i=0; i<HASH_TABLE_SIZE; i++)
        symbolTable.hashTable[i] = NULL;
    symbolTable.currentLevel = 0;
    symbolTable.scopeDisplayElementCount = 0;
    symbolTable.scopeDisplay = (SymbolTableEntry**)malloc(sizeof(SymbolTableEntry*)*128);
}

void symbolTableEnd()
{
}

SymbolTableEntry* retrieveSymbol(char* symbolName)
{
    int slot = HASH(symbolName);
    SymbolTableEntry* now = symbolTable.hashTable[slot];

    if(now != NULL)
        while(strcmp(now->name, symbolName) != 0)
            now = now->nextInHashChain;
    return now;
}

SymbolTableEntry* enterSymbol(char* symbolName, SymbolAttribute* attribute)
{
    int slot = HASH(symbolName);
    int level = symbolTable.currentLevel;
    SymbolTableEntry* prev = retrieveSymbol(symbolName);
    SymbolTableEntry* now = symbolTable.hashTable[slot];

    if(prev != NULL)
        if(prev->nestingLevel == level) { /* ERROR: Duplicate definition */ /* TODO*/ }

    /* Maintain Hash Table */
    symbolTable.hashTable[slot] = newSymbolTableEntry(level);
    symbolTable.hashTable[slot]->nextInHashChain = now;
    now->prevInHashChain = symbolTable.hashTable[slot];
    now = symbolTable.hashTable[slot];
    now->attribute = attribute;

    /* Maintain ScopeDisplay */
    now->nextInSameLevel = symbolTable.scopeDisplay[level];
    symbolTable.scopeDisplay[level] = now;

    /* Maintain Same name variable */
    if(prev != NULL)
        now->sameNameInOuterLevel = prev;

    return now;
}

//remove the symbol from the current scope
void removeSymbol(char* symbolName)
{
    int level = symbolTable.currentLevel;
    int scope_first = 0;
    SymbolTableEntry* temp = NULL;
    SymbolTableEntry* scope = symbolTable.scopeDisplay[level];
    SymbolTableEntry* scope_prev = NULL;

    if(scope != NULL) {
        /* scope display */
        if(strcmp(scope->name, symbolName) == 0) {
            symbolTable.scopeDisplay[level] = scope->nextInSameLevel;
            scope_first = 1;
        }
        if(!scope_first) {
            while(strcmp(scope->name, symbolName) != 0) {
                scope_prev = scope;
                scope = scope->nextInSameLevel;
            }
            scope_prev->nextInSameLevel = scope->nextInSameLevel;
        }
        /* hash chain */
        temp = scope->prevInHashChain;
        if(temp != NULL)
            temp->nextInHashChain = scope->nextInHashChain;
        scope->nextInHashChain->prevInHashChain = temp;

        free(scope);
    }
    return;
}

int declaredLocally(char* symbolName)
{
    SymbolTableEntry* now = retrieveSymbol(symbolName);

    if(now->nestingLevel == symbolTable.currentLevel) return 1;
    else return 0;
}

void openScope()
{
    symbolTable.currentLevel = symbolTable.scopeDisplayElementCount;
    symbolTable.scopeDisplay[symbolTable.currentLevel] = NULL;
    symbolTable.scopeDisplayElementCount++;
}

void closeScope()
{
    int currentLevel = symbolTable.currentLevel;
    SymbolTableEntry* now = symbolTable.scopeDisplay[currentLevel];
    while(now != NULL) {
        SymbolTableEntry* temp = now->nextInSameLevel;
        removeSymbol(now->name);
        now = temp;
    }
    symbolTable.scopeDisplay[currentLevel] = NULL;
    symbolTable.currentLevel--;
    symbolTable.scopeDisplayElementCount--;
}
