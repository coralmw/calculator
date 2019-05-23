grammar Expr;

main: expr EOF;
expr: expr ';' expr # pair
    | 'let ' IDENT '=' expr # assignment
    | IDENT '=' expr # update
    | IDENT # var
    | expr '*' expr # mul
    | expr '/' expr # div
    | expr '+' expr # add
    | expr '-' expr # sub
    | INT # lit
    | '(' expr ')' # group
    | command # meta
    | # nothing
    ;

command : HELP
        | QUIT ;

HELP : ('h'|'H'|'help') ;
QUIT : ('q'|'\n') ;

INT     : [0-9]+ ;
IDENT   : [a-z]+ ;

WS : (' ' | '\t')+ -> channel(HIDDEN);
