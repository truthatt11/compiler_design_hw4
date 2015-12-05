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
    
    switch(errorMsgKind)
    {
        case SYMBOL_IS_NOT_TYPE:
            printf("Symbol is not type.\n");
            break;
        case SYMBOL_REDECLARE:
            printf("Symbol redeclare.\n");
            break;
        case SYMBOL_UNDECLARED:
            printf("Symbol undeclared.\n");
            break;
        case NOT_FUNCTION_NAME:
            printf("No function name.\n");
            break;
        case TRY_TO_INIT_ARRAY:
            printf("Try to init array.\n");
            break;
        case EXCESSIVE_ARRAY_DIM_DECLARATION:
            printf("Excessive array dim declaration.\n");
            break;
        case RETURN_ARRAY:
            printf("Return array.\n");
            break;
        case VOID_VARIABLE:
            printf("Void variable.\n");
            break;
        case TYPEDEF_VOID_ARRAY:
            printf("Typedef void array.\n");
            break;
        case PARAMETER_TYPE_UNMATCH:
            printf("Parameter type unmatch.\n");
            break;
        case TOO_FEW_ARGUMENTS:
            printf("Too few arguments.\n");
            break;
        case TOO_MANY_ARGUMENTS:
            printf("Too many arguments.\n");
            break;
        case RETURN_TYPE_UNMATCH:
            printf("Return type unmatch.\n");
            break;
        case INCOMPATIBLE_ARRAY_DIMENSION:
            printf("Incompatible array dimension.\n");
            break;
        case NOT_ASSIGNABLE:
            printf("Not assignable.\n");
            break;
        case NOT_ARRAY:
            printf("Not array.\n");
            break;
        case IS_TYPE_NOT_VARIABLE:
            printf("Is type not variable.\n");
            break;
        case IS_FUNCTION_NOT_VARIABLE:
            printf("Is function not variable.\n");
            break;
        case STRING_OPERATION:
            printf("String operation.\n");
            break;
        case ARRAY_SIZE_NOT_INT:
            printf("Array size not int.\n");
            break;
        case ARRAY_SIZE_NEGATIVE:
            printf("Array size negative.\n");
            break;
        case ARRAY_SUBSCRIPT_NOT_INT:
            printf("Array subscript not int.\n");
            break;
        case PASS_ARRAY_TO_SCALAR:
            printf("Pass array to scalar.\n");
            break;
        case PASS_SCALAR_TO_ARRAY:
            printf("Pass scalar ot array.\n");
            break;
        default:
            printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
            break;
    }
    
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
        AST_NODE *Decl_node = now->child;
        switch(now->nodeType) {
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

        if(now->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
            type->kind = SCALAR_TYPE_DESCRIPTOR;
            type->properties.dataType = datatype;
        }
        if(now->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
            type->kind = ARRAY_TYPE_DESCRIPTOR;
            type->properties.arrayProperties.elementType = datatype;
            processDeclDimList(now, type, ignoreArrayFirstDimSize);
        }
        if(now->semantic_value.identifierSemanticValue.kind == WITH_INIT_ID) {
            type->kind = SCALAR_TYPE_DESCRIPTOR;
            type->properties.dataType = datatype;
        }
        
        attr->attr.typeDescriptor = type;
        now->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(now->semantic_value.identifierSemanticValue.identifierName, attr);
        if(now->semantic_value.identifierSemanticValue.symbolTableEntry == NULL)
            printErrorMsg(now, SYMBOL_REDECLARE);
        now = now->rightSibling;
    }
}

void declareFunction(AST_NODE* declarationNode)
{
    SymbolAttribute* attr = malloc(sizeof(SymbolAttribute));
    FunctionSignature* func_sig = malloc(sizeof(FunctionSignature));
    int para_count = 0;
    AST_NODE *func_name = declarationNode->rightSibling;
    AST_NODE *para_list = func_name->rightSibling;
    AST_NODE *block_node = para_list->rightSibling;
    char* name = declarationNode->semantic_value.identifierSemanticValue.identifierName;
    DATA_TYPE datatype;

    openScope();

    if(strcmp(name, "int")==0) { datatype = INT_TYPE; }
    else if(strcmp(name, "float")==0) { datatype = FLOAT_TYPE; }
    attr->attributeKind = FUNCTION_SIGNATURE;
    attr->attr.functionSignature = func_sig;
    func_sig->returnType = datatype;
    func_sig->parameterList = NULL;
    
    /* Parameter parsing */
    if(para_list->nodeType == PARAM_LIST_NODE) {
        AST_NODE* para = para_list->child;
        Parameter* prev_para = NULL;
        while(para != NULL) {
            Parameter* param = malloc(sizeof(Parameter));
            processDeclarationNode(para);
            param->parameterName = para->child->rightSibling->semantic_value.identifierSemanticValue.identifierName;
            param->type = para->child->rightSibling->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;
            if(func_sig->parameterList == NULL)
                func_sig->parameterList = param;
            else
                prev_para->next = param;
            prev_para = param;
            para = para->rightSibling;
            para_count++;
        }
    }else { /* Parse Tree Error: no param? */ }
    func_sig->parametersCount = para_count;

    /* Block Node Parsing */
    processBlockNode(block_node);

    closeScope();
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode)
{
}

void checkWhileStmt(AST_NODE* whileNode)
{
    AST_NODE *cond, *block;
    cond = whileNode->child;
    block = cond->rightSibling;

    processGeneralNode(cond);
    openScope();
    processBlockNode(block);
    closeScope();
}


void checkForStmt(AST_NODE* forNode)
{
    AST_NODE *init, *cond, *incr, *block;
    init = forNode;
    cond = init->rightSibling;
    incr = cond->rightSibling;
    block = incr->rightSibling;

    AST_NODE* child = init->child;
    switch(init->nodeType) {
        case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
            while(child != NULL) {
                if(child->nodeType == STMT_NODE)
                    processStmtNode(child);
                child = child->rightSibling;
            }
            break;
        case NUL_NODE:
            break;
        default:
            break;
    }
    child = cond->child;
    switch(cond->nodeType) {
        case NONEMPTY_RELOP_EXPR_LIST_NODE:
            while(child != NULL) {
                processGeneralNode(child);
                child = child->rightSibling;
            }
            break;
        case NUL_NODE:
            break;
        default:
            break;
    }
    child = incr->child;
    switch(incr->nodeType) {
        case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
            while(child != NULL) {
                if(child->nodeType == STMT_NODE)
                    processStmtNode(child);
                child = child->rightSibling;
            }
            break;
        case NUL_NODE:
            break;
        default:
            break;
    }
    openScope();
    processBlockNode(block);
    closeScope();
}


void checkAssignmentStmt(AST_NODE* assignmentNode)
{
    AST_NODE *left, *right, *temp;
    DATA_TYPE ldata, rdata;
    left = assignmentNode;
    right = left->rightSibling;
    if(right->rightSibling != NULL) { /* Assignment has three arguments?! */}
    /* Check left identifier */
    processVariableLValue(left);
    if(left->nodeType == IDENTIFIER_NODE) {
        char* id_name = left->semantic_value.identifierSemanticValue.identifierName;
        SymbolTableEntry* entry = retrieveSymbol(id_name);
        SymbolAttribute* attr = entry->attribute;
        if(attr->attributeKind == VARIABLE_ATTRIBUTE) {
            if(attr->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR)
                ldata = attr->attr.typeDescriptor->properties.dataType;
            if(attr->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR)
                ldata = attr->attr.typeDescriptor->properties.arrayProperties.elementType;
        }
    }

    /* Check right const, expr, id */
    switch(right->nodeType) {
        case EXPR_NODE:
            processExprNode(right);
            break;
        case CONST_VALUE_NODE:
            processConstValueNode(right);
            break;
        case IDENTIFIER_NODE:
            processVariableRValue(right);
            break;
        default:
            printErrorMsg(assignmentNode, NOT_ASSIGNABLE);
            break;
    }
}


void checkIfStmt(AST_NODE* ifNode)
{
    AST_NODE *cond, *block, *remaining;
    cond = ifNode;
    block = cond->rightSibling;
    remaining = block->rightSibling;

    processGeneralNode(cond);
    openScope();
    processBlockNode(block);
    closeScope();
    if(remaining != NULL) {
        switch(remaining->nodeType) {
            case STMT_NODE:
                checkIfStmt(remaining);
            case BLOCK_NODE:
                openScope();
                processBlockNode(remaining);
                closeScope();
            default:
                break;
        }
    }
}

void checkWriteFunction(AST_NODE* functionCallNode)
{
    AST_NODE* param = functionCallNode->rightSibling->child;
    if(param == NULL) { printErrorMsg(functionCallNode, TOO_FEW_ARGUMENTS); return; }
    if(param->rightSibling != NULL) { printErrorMsg(functionCallNode, TOO_MANY_ARGUMENTS); return; }
    /* accept const, id */
}

void checkFunctionCall(AST_NODE* functionCallNode)
{
    if(strcmp(functionCallNode->semantic_value.identifierSemanticValue.identifierName, "write")==0)
        checkWriteFunction(functionCallNode);
    SymbolTableEntry* func = functionCallNode->semantic_value.identifierSemanticValue.symbolTableEntry;
    if(func->attribute->attributeKind != FUNCTION_SIGNATURE) { return; }
    checkParameterPassing(func->attribute->attr.functionSignature->parameterList, functionCallNode->rightSibling->child);
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter)
{
    while(formalParameter != NULL) {
        if(actualParameter == NULL) { printErrorMsg(actualParameter, TOO_FEW_ARGUMENTS); return; }
        
        switch(formalParameter->type->kind) {
            case SCALAR_TYPE_DESCRIPTOR:
                /* actualP can be expr const id, no array */
                if(actualParameter->nodeType == IDENTIFIER_NODE) {
                    char* id_name = actualParameter->semantic_value.identifierSemanticValue.identifierName;
                    SymbolTableEntry* id = retrieveSymbol(id_name);
                    if(id == NULL) { printErrorMsg(actualParameter, SYMBOL_UNDECLARED); }
                    if(id->attribute->attributeKind == FUNCTION_SIGNATURE) { printErrorMsg(actualParameter, IS_FUNCTION_NOT_VARIABLE); }
                    if(id->attribute->attributeKind == TYPE_ATTRIBUTE) { printErrorMsg(actualParameter, IS_TYPE_NOT_VARIABLE); }
                    if(id->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR) {
                        int para_count = 0, actual_count = id->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
                        AST_NODE* temp = actualParameter->child;
                        while(temp != NULL) { temp = temp->rightSibling; para_count++;}
                        if(para_count < actual_count) { printErrorMsg(actualParameter, PASS_ARRAY_TO_SCALAR); }
                        else if(para_count > actual_count) { /* impossible*/ }
                    }
                }
                if(actualParameter->nodeType == CONST_VALUE_NODE) {}
                if(actualParameter->nodeType == EXPR_NODE) {}
                break;
            case ARRAY_TYPE_DESCRIPTOR:
                if(actualParameter->nodeType != IDENTIFIER_NODE) { printErrorMsg(actualParameter, NOT_ARRAY); }
                char* id_name = actualParameter->semantic_value.identifierSemanticValue.identifierName;
                int dim_count = formalParameter->type->properties.arrayProperties.dimension;
                int para_count = 0;
                int para_total;
                AST_NODE* temp = actualParameter->child;
                SymbolTableEntry* id = retrieveSymbol(id_name);
                if(id == NULL) { printErrorMsg(actualParameter, SYMBOL_UNDECLARED); }
                para_total = id->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
                while(temp != NULL) {temp=temp->rightSibling; para_count++;}

                if(dim_count != para_total-para_count) { printErrorMsg(actualParameter, INCOMPATIBLE_ARRAY_DIMENSION); }
                break;
            default:
                break;
        }

        formalParameter = formalParameter->next;
        actualParameter = actualParameter->rightSibling;
    }
    if(actualParameter != NULL) printErrorMsg(actualParameter, TOO_MANY_ARGUMENTS);
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
    if(exprNode->nodeType != EXPR_NODE) { return; }
    AST_NODE *lvalue, *rvalue;
    switch(exprNode->semantic_value.exprSemanticValue.kind) {
        case BINARY_OPERATION:
            lvalue = exprNode->child;
            rvalue = lvalue->rightSibling;
            processGeneralNode(lvalue);
            processGeneralNode(rvalue);
        case UNARY_OPERATION:
            lvalue = exprNode->child;
            processGeneralNode(lvalue);
        default:
            break;
    }
}


void processVariableLValue(AST_NODE* idNode)
{
    int lparam_count = 0;
   if(idNode->nodeType != IDENTIFIER_NODE)
        printErrorMsg(idNode, NOT_ASSIGNABLE);
    SymbolTableEntry* id = retrieveSymbol(idNode->semantic_value.identifierSemanticValue.identifierName);
    if(id == NULL) {
        printErrorMsg(idNode, SYMBOL_UNDECLARED);
        return;
    }
    if(id->attribute->attributeKind == TYPE_ATTRIBUTE)
        printErrorMsg(idNode, IS_TYPE_NOT_VARIABLE);
    if(id->attribute->attributeKind == FUNCTION_SIGNATURE)
        printErrorMsg(idNode, IS_FUNCTION_NOT_VARIABLE);
    if(id->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR) {
        AST_NODE *temp = idNode->child;
        while(temp != NULL) { lparam_count++; temp = temp->rightSibling; }
        if(lparam_count < id->attribute->attr.typeDescriptor->properties.arrayProperties.dimension)
            printErrorMsg(idNode, PASS_ARRAY_TO_SCALAR);
    }
}

void processVariableRValue(AST_NODE* idNode)
{
    int lparam_count = 0;
    if(idNode->nodeType != IDENTIFIER_NODE) { return; }
    if(idNode->semantic_value.identifierSemanticValue.symbolTableEntry == NULL) {
        printErrorMsg(idNode, SYMBOL_UNDECLARED);
    }
    /* Check not type, ID and function OK */
    char* id_name = idNode->semantic_value.identifierSemanticValue.identifierName;
    SymbolTableEntry* id = retrieveSymbol(id_name);
    if(id->attribute->attributeKind == TYPE_ATTRIBUTE)
        printErrorMsg(idNode, IS_TYPE_NOT_VARIABLE);

    /* Check ID and array element */
    if(id->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR) {
        AST_NODE *temp = idNode->child;
        while(temp != NULL) { lparam_count++; temp = temp->rightSibling; }
        if(lparam_count < id->attribute->attr.typeDescriptor->properties.arrayProperties.dimension)
            printErrorMsg(idNode, PASS_ARRAY_TO_SCALAR);
    }
}


void processConstValueNode(AST_NODE* constValueNode)
{
    if(constValueNode->nodeType != CONST_VALUE_NODE) { return; }
    ;
}


void checkReturnStmt(AST_NODE* returnNode)
{
}


void processBlockNode(AST_NODE* blockNode)
{
    AST_NODE* now = blockNode->child;
    while(now != NULL) {
        AST_NODE* child = now->child;
        switch(now->nodeType) {
            case VARIABLE_DECL_LIST_NODE:
                while(child != NULL) {
                    processDeclarationNode(child);
                    child = child->rightSibling;
                }
                break;
            case STMT_LIST_NODE:
                while(child != NULL) {
                    processStmtNode(child);
                    child = child->rightSibling;
                }
                break;
            default:
                break;
        }
        now = now->rightSibling;
    }
}


void processStmtNode(AST_NODE* stmtNode)
{
    if(stmtNode->nodeType != STMT_NODE) { /* Error */ return; }
    switch(stmtNode->semantic_value.stmtSemanticValue.kind) {
        case WHILE_STMT:
            checkWhileStmt(stmtNode->child);
            break;
        case FOR_STMT:
            checkForStmt(stmtNode->child);
            break;
        case ASSIGN_STMT:
            checkAssignmentStmt(stmtNode->child);
            break;
        case IF_STMT:
            checkIfStmt(stmtNode->child);
            break;
        case FUNCTION_CALL_STMT:
            checkFunctionCall(stmtNode->child);
            break;
        case RETURN_STMT:
            checkReturnStmt(stmtNode->child);
            break;
        default:
            break;
    }
}


void processGeneralNode(AST_NODE *node)
{
    switch(node->nodeType) {
        case IDENTIFIER_NODE:
            processVariableRValue(node);
            break;
        case CONST_VALUE_NODE:
            processConstValueNode(node);
            break;
        case EXPR_NODE:
            processExprNode(node);
            break;
        case NUL_NODE:
            break;
        default:
            break;
    }
}

void processDeclDimList(AST_NODE* idNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize)
{
    int dim = 0;
    AST_NODE* dim_value = idNode->child;
    typeDescriptor->kind = ARRAY_TYPE_DESCRIPTOR;
    /* TODO ignore_first_dim_size @@ */
    while(dim_value != NULL) {
        if(dim_value->nodeType == CONST_VALUE_NODE) {
            if(dim_value->semantic_value.const1->const_type == INTEGERC) {
                if(dim_value->semantic_value.const1->const_u.intval > 0)
                    typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim] = dim_value->semantic_value.const1->const_u.intval;
                else
                    printErrorMsg(idNode, ARRAY_SIZE_NEGATIVE);
            } else{
                printErrorMsg(idNode, ARRAY_SIZE_NOT_INT);
            }
        } else if(dim_value->nodeType == EXPR_NODE) {
            /*TODO*/ /* processExprNode*/
        }
        dim++;
    }
    typeDescriptor->properties.arrayProperties.dimension = dim;
}

