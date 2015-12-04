#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "symbolTable.h"
// This file is for reference only, you are not required to follow the implementation. //
// You only need to check for errors stated in the hw4 assignment document. //
int g_anyErrorOccur = 0;

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2);
void processProgramNode(AST_NODE *programNode);
void processDeclarationNode(AST_NODE* declarationNode);
void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize);
void declareFunction(AST_NODE* returnTypeNode);
void processDeclDimList(AST_NODE* variableDeclDimList, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize);
void processTypeNode(AST_NODE* typeNode);
void processBlockNode(AST_NODE* blockNode);
void processStmtNode(AST_NODE* stmtNode);
void processGeneralNode(AST_NODE *node);
void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode);
void checkWhileStmt(AST_NODE* whileNode);
void checkForStmt(AST_NODE* forNode);
void checkAssignmentStmt(AST_NODE* assignmentNode);
void checkIfStmt(AST_NODE* ifNode);
void checkWriteFunction(AST_NODE* functionCallNode);
void checkFunctionCall(AST_NODE* functionCallNode);
void processExprRelatedNode(AST_NODE* exprRelatedNode);
void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter);
void checkReturnStmt(AST_NODE* returnNode);
void processExprNode(AST_NODE* exprNode);
void processVariableLValue(AST_NODE* idNode);
void processVariableRValue(AST_NODE* idNode);
void processConstValueNode(AST_NODE* constValueNode);
void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue);
void evaluateExprValue(AST_NODE* exprNode);


typedef enum ErrorMsgKind
{
    SYMBOL_IS_NOT_TYPE,
    SYMBOL_REDECLARE,
    SYMBOL_UNDECLARED,
    NOT_FUNCTION_NAME,
    TRY_TO_INIT_ARRAY,
    EXCESSIVE_ARRAY_DIM_DECLARATION,
    RETURN_ARRAY,
    VOID_VARIABLE,
    TYPEDEF_VOID_ARRAY,
    PARAMETER_TYPE_UNMATCH,
    TOO_FEW_ARGUMENTS,
    TOO_MANY_ARGUMENTS,
    RETURN_TYPE_UNMATCH,
    INCOMPATIBLE_ARRAY_DIMENSION,
    NOT_ASSIGNABLE,
    NOT_ARRAY,
    IS_TYPE_NOT_VARIABLE,
    IS_FUNCTION_NOT_VARIABLE,
    STRING_OPERATION,
    ARRAY_SIZE_NOT_INT,
    ARRAY_SIZE_NEGATIVE,
    ARRAY_SUBSCRIPT_NOT_INT,
    PASS_ARRAY_TO_SCALAR,
    PASS_SCALAR_TO_ARRAY
} ErrorMsgKind;

void printErrorMsgSpecial(AST_NODE* node1, char* name2, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node1->linenumber);
    /*
    switch(errorMsgKind)
    {
    default:
        printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        break;
    }
    */
}


void printErrorMsg(AST_NODE* node, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node->linenumber);
    /*
    switch(errorMsgKind)
    {
        printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        break;
    }
    */
}


void semanticAnalysis(AST_NODE *root)
{
    processProgramNode(root);
}


DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2)
{
    if(dataType1 == FLOAT_TYPE || dataType2 == FLOAT_TYPE) {
        return FLOAT_TYPE;
    } else {
        return INT_TYPE;
    }
}


void processProgramNode(AST_NODE *programNode)
{
    /* var decl, func decl */
    AST_NODE *now = programNode->child;
    while(now != NULL) {
        switch(now->nodeType) {
            AST_NODE *Decl_node = now->child;
            case DECLARATION_NODE:
                if(now->semantic_value.declSemanticValue.kind == FUNCTION_DECL) { /* function declaration */
                    declareFunction(Decl_node);
                }else { /* error */ }
                break;
            case VARIABLE_DECL_LIST_NODE:
                while(Decl_node != NULL) { processDeclarationNode(Decl_node); Decl_node = Decl_node->rightSibling; }
                break;
            default:
                /* parsing error */;
        }
        now = now->rightSibling;
    }
}

void processDeclarationNode(AST_NODE* declarationNode)
{
    AST_NODE* now = declarationNode->child;
    switch(declarationNode->semantic_value.declSemanticValue.kind) {
        case VARIABLE_DECL:
            declareIdList(now, VARIABLE_ATTRIBUTE, 0);
            break;
        case TYPE_DECL:
            processTypeNode(now);
            break;
        case FUNCTION_DECL:
            declareFunction(now);
            break;
        case FUNCTION_PARAMETER_DECL:
            declareIdList(now, VARIABLE_ATTRIBUTE, 1);
            break;
        default:
            /* error */
            break;
    }
}


void processTypeNode(AST_NODE* idNodeAsType)
{
    /* ADD RIGHTSIBLING AS A SYMBOL, ATTRIBUTED AS A TYPE */
    SymbolAttribute* attr;
    TypeDescriptor*  typedes;
    DATA_TYPE DataType;
    char* datatype = idNodeAsType->semantic_value.identifierSemanticValue.identifierName;
    AST_NODE* now = idNodeAsType->rightSibling;

    if(strcmp(datatype, "int")==0) DataType = INT_TYPE;
    else if(strcmp(datatype, "float")==0) DataType = FLOAT_TYPE;
    else if(strcmp(datatype, "void")==0) DataType = VOID_TYPE;
    
    while(now != NULL) {
        attr = malloc(sizeof(SymbolAttribute));
        typedes = malloc(sizeof(TypeDescriptor));
        attr->attributeKind = TYPE_ATTRIBUTE;
        attr->attr.typeDescriptor = typedes;

        if(now->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
            typedes->kind = SCALAR_TYPE_DESCRIPTOR;
            typedes->properties.dataType = DataType;
        }else if(now->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
            typedes->kind = ARRAY_TYPE_DESCRIPTOR;
            processDeclDimList(now, typedes, 0);
        }else { /* error */ }
        now->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(now->semantic_value.identifierSemanticValue.identifierName, attr);
        now = now->rightSibling;
    }
}


void declareIdList(AST_NODE* declarationNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize)
{
    SymbolAttribute* attr = malloc(sizeof(SymbolAttribute));
    char* name = declarationNode->semantic_value.identifierSemanticValue.identifierName;
    AST_NODE* now = declarationNode->rightSibling;
    DATA_TYPE datatype;
    if(strcmp(name, "int")==0) { datatype = INT_TYPE; }
    else if(strcmp(name, "float")==0) { datatype = FLOAT_TYPE; }
    attr->attributeKind = VARIABLE_ATTRIBUTE;
    
    while(now != NULL) {
        TypeDescriptor* type = malloc(sizeof(TypeDescriptor));
        type->properties.dataType = datatype;

        if(now->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
            type->kind = SCALAR_TYPE_DESCRIPTOR;
        }
        if(now->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
            type->kind = ARRAY_TYPE_DESCRIPTOR;
            type->properties.arrayProperties.elementType = datatype;
            processDeclDimList(now, type, 0);
        }
        if(now->semantic_value.identifierSemanticValue.kind == WITH_INIT_ID) {
            type->kind = SCALAR_TYPE_DESCRIPTOR;
        }
        
        attr->attr.typeDescriptor = type;
        now->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(now->semantic_value.identifierSemanticValue.identifierName, attr);
        now = now->rightSibling;
    }
}

void declareFunction(AST_NODE* declarationNode)
{
    SymbolAttribute* attr = malloc(sizeof(SymbolAttribute));
    FunctionSignature* func_sig = malloc(sizeof(FunctionSignature));
    /* para_count, para_list, return_type */

    attr->attributeKind = FUNCTION_SIGNATURE;
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode)
{
}

void checkWhileStmt(AST_NODE* whileNode)
{
}


void checkForStmt(AST_NODE* forNode)
{
}


void checkAssignmentStmt(AST_NODE* assignmentNode)
{
}


void checkIfStmt(AST_NODE* ifNode)
{
}

void checkWriteFunction(AST_NODE* functionCallNode)
{
}

void checkFunctionCall(AST_NODE* functionCallNode)
{
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter)
{
}


void processExprRelatedNode(AST_NODE* exprRelatedNode)
{
}

void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue)
{
}

void evaluateExprValue(AST_NODE* exprNode)
{
}


void processExprNode(AST_NODE* exprNode)
{
}


void processVariableLValue(AST_NODE* idNode)
{
}

void processVariableRValue(AST_NODE* idNode)
{
}


void processConstValueNode(AST_NODE* constValueNode)
{
}


void checkReturnStmt(AST_NODE* returnNode)
{
}


void processBlockNode(AST_NODE* blockNode)
{
}


void processStmtNode(AST_NODE* stmtNode)
{
}


void processGeneralNode(AST_NODE *node)
{
}

void processDeclDimList(AST_NODE* idNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize)
{
    int dim = 0;
    AST_NODE* dim_value = idNode->child;
    typeDescriptor->kind = ARRAY_TYPE_DESCRIPTOR;
    while(dim_value != NULL) {
        if(dim_value->nodeType == CONST_VALUE_NODE) {
            if(dim_value->semantic_value.const1->const_type == INTEGERC) {
                typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim] = dim_value->semantic_value.const1->const_u.intval;
            } else{
                printErrorMsg(idNode, INCOMPATIBLE_ARRAY_DIMENSION);
            }
        } else if(dim_value->nodeType == EXPR_NODE) {
            /*TODO*/ /* processExprNode*/
        }
        dim++;
    }
    typeDescriptor->properties.arrayProperties.dimension = dim;
}

