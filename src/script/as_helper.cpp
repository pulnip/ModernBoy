#include <print>
#include <angelscript.h>
#include "script/as_helper.hpp"

void ModernBoy::Script::printExceptionInfo(asIScriptContext *ctx){
    // Determine the exception that occurred
    std::print("desc: {}\n", ctx->GetExceptionString());
 
    // Determine the function where the exception occurred
    const asIScriptFunction *function = ctx->GetExceptionFunction();
    std::print("func: {}\n", function->GetDeclaration());
    std::print("modl: {}\n", function->GetModuleName());
    std::print("sect: {}\n", function->GetScriptSectionName());
 
    // Determine the line number where the exception occurred
    std::print("line: {}\n", ctx->GetExceptionLineNumber());
}
