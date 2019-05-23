
# antlr4=
# ANTLR_INC=/home/tparks/.m2/repository/org/antlr/antlr4-runtime/4.7.2/Cpp/run/usr/local/include/antlr4-runtime/

OUTPUT=output
GENERATED=generated
CCARGS=-c -I/home/tparks/.m2/repository/org/antlr/antlr4-runtime/4.7.2/Cpp/run/usr/local/include/antlr4-runtime/ -I $(GENERATED) -std=c++11 -Wall
LDARGS=-g
LIBS=/home/tparks/.m2/repository/org/antlr/antlr4-runtime/4.7.2/Cpp/run/usr/local/lib/libantlr4-runtime.a
CC=g++
GRAMMAR=Expr
ANTLR4=java -jar /home/tparks/.m2/repository/org/antlr/antlr4/4.7.2/antlr4-4.7.2-complete.jar
# ANTLR4=antlr4

ANTLRGEN=BaseListener Lexer Listener Parser
OBJS=$(addsuffix .o,$(addprefix $(OUTPUT)/$(GRAMMAR),$(ANTLRGEN)))
GSOURCES=$(addsuffix .cpp,$(addprefix $(GENERATED)/$(GRAMMAR),$(ANTLRGEN)))

.precious: $(GSOURCES)

all: parser

parser: dirs antlr4 parser.cpp $(OBJS)
	$(CC) $(CCARGS) parser.cpp  -o $(OUTPUT)/parser.o
	$(CC) $(LDARGS) $(OUTPUT)/parser.o $(OBJS) $(LIBS) -o parser

antlr4: $(GENERATED)/.generated;

$(GENERATED)/.generated: $(GRAMMAR).g4
	$(ANTLR4) -Dlanguage=Cpp -visitor -o $(GENERATED) $(GRAMMAR).g4
	@touch $(GENERATED)/.generated

$(OUTPUT)/%.o : $(GENERATED)/%.cpp
	$(CC) $(CCARGS) $< -o $@

$(GENERATED)/%.cpp: $(GENERATED)/.generated;

dirs:; mkdir -p $(OUTPUT) $(GENERATED)
clean:; rm -rf $(OUTPUT) $(GENERATED)
