#include <iostream>
#include <strstream>
#include <string>
#include <memory>
#include <map>
#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"
#include "ExprVisitor.h"

using namespace std;
int DEBUG = 0;

string HELPTEXT =
"Tom's fancy calculator.\n"
"expressions involving *,/,+,- are parsed.\n"
"declare lits with let NAME = expression and update with NAME = expression.\n";
/*
Parser should make use of the expression generator to walk the parse tree specified
in the book, generate a parse, and evaluate it.
*/

class MyParserErrorListener: public antlr4::BaseErrorListener {
  virtual void syntaxError(
      antlr4::Recognizer *recognizer,
      antlr4::Token *offendingSymbol,
      size_t line,
      size_t charPositionInLine,
      const std::string &msg,
      std::exception_ptr e) override {
    std::ostrstream s;
    s << "Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ")";
    throw std::invalid_argument(s.str());
  }
};


// this will need to be contructed with the context, and modify it to run updates
// ...
class  ExprEvaluator : ExprVisitor {

public:

  map<string, int>& locals;
  int retval;

  ExprEvaluator(map<string, int>& locals_) : locals(locals_), retval(0) {}

  antlrcpp::Any visitMain(ExprParser::MainContext *context) {
    // need to extract the single Expr and visit it
    if (DEBUG) cout << "visiting main expression at" << &context << endl;
    return visit(context->expr());
  }

  virtual antlrcpp::Any visitDiv(ExprParser::DivContext *context) {
      int leftval = visit(context->expr(0));
      int rightval = visit(context->expr(1));
      return leftval / rightval;
  }

  virtual antlrcpp::Any visitAdd(ExprParser::AddContext *context) {
      int leftval = visit(context->expr(0));
      int rightval = visit(context->expr(1));
      return leftval + rightval;
  }

  virtual antlrcpp::Any visitSub(ExprParser::SubContext *context) {
      int leftval = visit(context->expr(0));
      int rightval = visit(context->expr(1));
      return leftval - rightval;
  }

  virtual antlrcpp::Any visitMul(ExprParser::MulContext *context) {
      int leftval = visit(context->expr(0));
      int rightval = visit(context->expr(1));
      return leftval * rightval;
  }

  virtual antlrcpp::Any visitAssignment(ExprParser::AssignmentContext *context) {
      // add a new entry to the locals map, and return the new value
      int value = visit(context->expr());
      string target = context->IDENT()->getText();
      locals[target] = value;
      return value;
  }

  virtual antlrcpp::Any visitLit(ExprParser::LitContext *context) {
    return stoi(context->INT()->getText());
  }

  virtual antlrcpp::Any visitUpdate(ExprParser::UpdateContext *context) {
      // update the value in locals map, and return OLD value (why not.)
      int value = visit(context->expr());
      string target = context->IDENT()->getText();
      int old = locals[target];
      locals[target] = value;
      return old;
  }

  virtual antlrcpp::Any visitVar(ExprParser::VarContext *context) {
      string target = context->IDENT()->getText();
      if ( locals.find(target) == locals.end() ) {
        cout << "Var " << target << " not set." << endl;
        return 0;
      } else {
        return locals[target];
      }
    }


  virtual antlrcpp::Any visitPair(ExprParser::PairContext *context) {
    int leftval = visit(context->expr(0));
    int rightval = visit(context->expr(1));
    return rightval;
  }

  virtual antlrcpp::Any visitGroup(ExprParser::GroupContext *context) {
    return visit(context->expr());
  }

  virtual antlrcpp::Any visitMeta(ExprParser::MetaContext *context) {
    cout << "processing command" << endl;
    visit(context->command());
    return 0;
  }

  virtual antlrcpp::Any visitCommand(ExprParser::CommandContext *context) {
    if (context->HELP() != nullptr) {
      cout << HELPTEXT << endl;
      return 0;
    }
    if (context->QUIT() != nullptr) {
      cout << "goodbye" << endl;
      exit(0);
    }
  }

  virtual antlrcpp::Any visitNothing(ExprParser::NothingContext *context) {
    return 0;
  }


};

int main(int argc, char *argv[]) {
  string tmp;
  map<string, int> locals;
  MyParserErrorListener errorListner;

  while (1) {
    cout << "> ";
    getline(cin,tmp);

    try {
        // parse the input line
        antlr4::ANTLRInputStream input(tmp);
        ExprLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);


        tokens.fill();
        // Only if you want to list the tokens
        if (DEBUG) {
          for (auto token : tokens.getTokens()) {
           std::cout << token->toString() << std::endl;
          }
        }

        ExprParser parser(&tokens);
        parser.removeErrorListeners();
        parser.addErrorListener(&errorListner);
        ExprParser::MainContext* tree = parser.main();
        if (DEBUG) cout << "TREE: " << tree->toStringTree() << endl;
        ExprEvaluator eval(locals);
        int retval = (int)eval.visitMain(tree);
        cout << "= " << retval << endl;
    } catch (invalid_argument &e) {
        cout << e.what() << endl;
        exit(1);
    }
  }

}
