

#if !defined(Lua_COCO_PARSER_H__)
#define Lua_COCO_PARSER_H__



#include "Scanner.h"

namespace Lua {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_Name=1,
		_Int=2,
		_Float=3,
		_Expnum=4,
		_Hex=5,
		_Normalstring=6,
		_Longstring=7,
		_Eql=8,
		_Comma=9,
		_Point=10,
		_Colon=11,
		_Semicolon=12,
		_Slb=13,
		_Srb=14,
		_Rlb=15,
		_Rrb=16,
		_Flb=17,
		_Frb=18
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

bool IsNameAndEql()
{
    scanner->ResetPeek();
    Token* next = scanner->Peek();
    return la->kind == _Name && next->kind == _Eql;
}

/* return true, if lookahead token is _Comma and next token is _Name */
bool IsCommaAndName()
{
    scanner->ResetPeek();
    Token* next = scanner->Peek();
    return la->kind == _Comma && next->kind == _Name;
}

/* resolve LL(1) conflict in "stat" production */
bool IsCommaOrEql()
{
    scanner->ResetPeek();
    Token* next = scanner->Peek();
    if (next->kind == _Slb)
    {    
        int i = 0;
        while (next->kind != _EOF)
        {
            if (next->kind == _Slb) i++;
            else if (next->kind == _Srb) i--;
            if (i == 0) break;
            next = scanner->Peek();
        }
        next = scanner->Peek();
        return next->kind == _Comma || next->kind == _Eql || next->kind == _Slb;
    }
    if (next->kind == _Point)
    {
        next = scanner->Peek();
        if (next->kind == _Name)
        {
            next = scanner->Peek();
            return next->kind == _Comma || next->kind == _Eql;
        }
        return false;
    }
    return next->kind == _Comma || next->kind == _Eql;
}



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void Lua();
	void Chunk();
	void Stat();
	void Laststat();
	void Block();
	void Varlist();
	void Explist();
	void Functioncall();
	void Exp();
	void Funcname();
	void Funcbody();
	void Namelist();
	void Var();
	void Exp1();
	void Binop();
	void Exp2();
	void Unop();
	void Number();
	void String();
	void Function();
	void Prefixexp();
	void Tableconstructor();
	void Prefixexp2();
	void Args();
	void Prefixexp1();
	void Parlist();
	void Parlist1();
	void Fieldlist();
	void Field();
	void Fieldsep();

	void Parse();

}; // end Parser

} // namespace


#endif

