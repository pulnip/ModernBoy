#include <print>
#include <angelscript.h>
#include "script/as_helper.hpp"

void ModernBoy::Script::printExceptionInfo(asIScriptContext *ctx){
    // Determine the exception that occurred
    std::println("desc: {}", ctx->GetExceptionString());
 
    // Determine the function where the exception occurred
    const asIScriptFunction *function = ctx->GetExceptionFunction();
    std::println("func: {}", function->GetDeclaration());
    std::print("modl: {}", function->GetModuleName());

    const char* sectionName;
    function->GetDeclaredAt(&sectionName, nullptr, nullptr);
    std::print("sect: {}", sectionName);
 
    // Determine the line number where the exception occurred
    std::println("line: {}", ctx->GetExceptionLineNumber());
}
