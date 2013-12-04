#include <iostream>

#include "lolcode_stmt.h"
#include "lolcode.tab.h"

using namespace std;

int yyparse();

// flex externals
extern int yylineno;
extern FILE *yyin;

StmtList *program;

void yyerror(string error) {
    cerr << "ParserError: syntax error, line: " << yylineno << endl;
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        exit(-1);
    }
    if (!(yyin = fopen(argv[1], "r"))) {
        cerr << "IOError: failed to open file: " << argv[1] << endl;
        exit(-1);
    }
    program = new StmtList();
    yyparse();
    program->execute();
    return 0;
}

