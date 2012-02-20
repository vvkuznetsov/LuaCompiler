

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


namespace Lua {


void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::Lua() {
		Chunk();
}

void Parser::Chunk() {
		while (StartOf(1)) {
			Stat();
			if (la->kind == 12) {
				Get();
			}
		}
		if (la->kind == 32 || la->kind == 33) {
			Laststat();
			if (la->kind == 12) {
				Get();
			}
		}
}

void Parser::Stat() {
		if (IsCommaOrEql()) {
			Varlist();
			Expect(8);
			Explist();
		} else if (la->kind == 1 || la->kind == 15) {
			Functioncall();
		} else if (la->kind == 19) {
			Get();
			Block();
			Expect(20);
		} else if (la->kind == 21) {
			Get();
			Exp();
			Expect(19);
			Block();
			Expect(20);
		} else if (la->kind == 22) {
			Get();
			Block();
			Expect(23);
			Exp();
		} else if (la->kind == 24) {
			Get();
			Exp();
			Expect(25);
			Block();
			while (la->kind == 26) {
				Get();
				Exp();
				Expect(25);
				Block();
			}
			if (la->kind == 27) {
				Get();
				Block();
			}
			Expect(20);
		} else if (la->kind == 28) {
			Get();
			Expect(1);
			if (la->kind == 8) {
				Get();
				Exp();
				Expect(9);
				Exp();
				if (la->kind == 9) {
					Get();
					Exp();
				}
				Expect(19);
				Block();
				Expect(20);
			} else if (la->kind == 9 || la->kind == 29) {
				while (la->kind == 9) {
					Get();
					Expect(1);
				}
				Expect(29);
				Explist();
				Expect(19);
				Block();
				Expect(20);
			} else SynErr(56);
		} else if (la->kind == 30) {
			Get();
			Funcname();
			Funcbody();
		} else if (la->kind == 31) {
			Get();
			if (la->kind == 30) {
				Get();
				Expect(1);
				Funcbody();
			} else if (la->kind == 1) {
				Namelist();
				if (la->kind == 8) {
					Get();
					Explist();
				}
			} else SynErr(57);
		} else SynErr(58);
}

void Parser::Laststat() {
		if (la->kind == 32) {
			Get();
			if (StartOf(2)) {
				Explist();
			}
		} else if (la->kind == 33) {
			Get();
		} else SynErr(59);
}

void Parser::Block() {
		Chunk();
}

void Parser::Varlist() {
		Var();
		while (la->kind == 9) {
			Get();
			Var();
		}
}

void Parser::Explist() {
		Exp();
		while (la->kind == 9) {
			Get();
			Exp();
		}
}

void Parser::Functioncall() {
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 15) {
			Get();
			Exp();
			Expect(16);
		} else SynErr(60);
		Prefixexp2();
		if (la->kind == 11) {
			Get();
			Expect(1);
		}
		Args();
}

void Parser::Exp() {
		Exp1();
		while (StartOf(3)) {
			Binop();
			Exp1();
		}
}

void Parser::Funcname() {
		Expect(1);
		while (la->kind == 10) {
			Get();
			Expect(1);
		}
		if (la->kind == 11) {
			Get();
			Expect(1);
		}
}

void Parser::Funcbody() {
		Expect(15);
		if (la->kind == 1 || la->kind == 37) {
			Parlist();
		}
		Expect(16);
		Block();
		Expect(20);
}

void Parser::Namelist() {
		Expect(1);
		while (la->kind == 9) {
			Get();
			Expect(1);
		}
}

void Parser::Var() {
		Expect(1);
		while (la->kind == 10 || la->kind == 13) {
			if (la->kind == 13) {
				Get();
				Exp();
				Expect(14);
			} else {
				Get();
				Expect(1);
			}
		}
}

void Parser::Exp1() {
		if (StartOf(4)) {
			Exp2();
		} else if (la->kind == 39 || la->kind == 53 || la->kind == 54) {
			Unop();
			Exp2();
		} else SynErr(61);
}

void Parser::Binop() {
		switch (la->kind) {
		case 38: {
			Get();
			break;
		}
		case 39: {
			Get();
			break;
		}
		case 40: {
			Get();
			break;
		}
		case 41: {
			Get();
			break;
		}
		case 42: {
			Get();
			break;
		}
		case 43: {
			Get();
			break;
		}
		case 44: {
			Get();
			break;
		}
		case 45: {
			Get();
			break;
		}
		case 46: {
			Get();
			break;
		}
		case 47: {
			Get();
			break;
		}
		case 48: {
			Get();
			break;
		}
		case 49: {
			Get();
			break;
		}
		case 50: {
			Get();
			break;
		}
		case 51: {
			Get();
			break;
		}
		case 52: {
			Get();
			break;
		}
		default: SynErr(62); break;
		}
}

void Parser::Exp2() {
		switch (la->kind) {
		case 34: {
			Get();
			break;
		}
		case 35: {
			Get();
			break;
		}
		case 36: {
			Get();
			break;
		}
		case 2: case 3: case 4: case 5: {
			Number();
			break;
		}
		case 6: case 7: {
			String();
			break;
		}
		case 37: {
			Get();
			break;
		}
		case 30: {
			Function();
			break;
		}
		case 1: case 15: {
			Prefixexp();
			break;
		}
		case 17: {
			Tableconstructor();
			break;
		}
		default: SynErr(63); break;
		}
}

void Parser::Unop() {
		if (la->kind == 39) {
			Get();
		} else if (la->kind == 53) {
			Get();
		} else if (la->kind == 54) {
			Get();
		} else SynErr(64);
}

void Parser::Number() {
		if (la->kind == 2) {
			Get();
		} else if (la->kind == 3) {
			Get();
		} else if (la->kind == 4) {
			Get();
		} else if (la->kind == 5) {
			Get();
		} else SynErr(65);
}

void Parser::String() {
		if (la->kind == 6) {
			Get();
		} else if (la->kind == 7) {
			Get();
		} else SynErr(66);
}

void Parser::Function() {
		Expect(30);
		Funcbody();
}

void Parser::Prefixexp() {
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 15) {
			Get();
			Exp();
			Expect(16);
		} else SynErr(67);
		Prefixexp1();
}

void Parser::Tableconstructor() {
		Expect(17);
		if (StartOf(5)) {
			Fieldlist();
		}
		Expect(18);
}

void Parser::Prefixexp2() {
		if (la->kind == 10 || la->kind == 13) {
			if (la->kind == 13) {
				Get();
				Exp();
				Expect(14);
			} else {
				Get();
				Expect(1);
				Prefixexp2();
			}
		} else if (StartOf(6)) {
		} else SynErr(68);
}

void Parser::Args() {
		if (la->kind == 15) {
			Get();
			if (StartOf(2)) {
				Explist();
			}
			Expect(16);
		} else if (la->kind == 17) {
			Tableconstructor();
		} else if (la->kind == 6 || la->kind == 7) {
			String();
		} else SynErr(69);
}

void Parser::Prefixexp1() {
		if (StartOf(6)) {
			if (la->kind == 11) {
				Get();
				Expect(1);
			}
			Args();
		} else if (la->kind == 10 || la->kind == 13) {
			if (la->kind == 13) {
				Get();
				Exp();
				Expect(14);
				Prefixexp1();
			} else {
				Get();
				Expect(1);
				Prefixexp1();
			}
		} else if (StartOf(7)) {
		} else SynErr(70);
}

void Parser::Parlist() {
		if (la->kind == 1) {
			Get();
			Parlist1();
		} else if (la->kind == 37) {
			Get();
		} else SynErr(71);
}

void Parser::Parlist1() {
		if (IsCommaAndName()) {
			Expect(9);
			Expect(1);
			Parlist1();
		} else if (la->kind == 9 || la->kind == 16) {
			if (la->kind == 9) {
				Get();
				Expect(37);
			}
		} else SynErr(72);
}

void Parser::Fieldlist() {
		Field();
		while (la->kind == 9 || la->kind == 12) {
			Fieldsep();
			Field();
		}
}

void Parser::Field() {
		if (la->kind == 13) {
			Get();
			Exp();
			Expect(14);
			Expect(8);
			Exp();
		} else if (IsNameAndEql()) {
			Expect(1);
			Expect(8);
			Exp();
		} else if (StartOf(2)) {
			Exp();
		} else SynErr(73);
}

void Parser::Fieldsep() {
		if (la->kind == 9) {
			Get();
		} else if (la->kind == 12) {
			Get();
		} else SynErr(74);
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	Lua();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 55;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[8][57] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,x,x,T, x,T,T,x, T,x,x,x, T,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,T,T, T,T,T,T, x,x,x,x, x,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, T,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x,x,x, x},
		{x,T,T,T, T,T,T,T, x,x,x,x, x,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,T,T, T,T,T,T, x,x,x,x, x,T,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, T,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x},
		{x,x,x,x, x,x,T,T, x,x,x,T, x,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{T,T,x,x, x,x,x,x, x,T,x,x, T,x,T,T, T,x,T,T, T,T,T,T, T,T,T,T, T,x,T,T, T,T,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x,x,x, x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"Name expected"); break;
			case 2: s = coco_string_create(L"Int expected"); break;
			case 3: s = coco_string_create(L"Float expected"); break;
			case 4: s = coco_string_create(L"Expnum expected"); break;
			case 5: s = coco_string_create(L"Hex expected"); break;
			case 6: s = coco_string_create(L"Normalstring expected"); break;
			case 7: s = coco_string_create(L"Longstring expected"); break;
			case 8: s = coco_string_create(L"Eql expected"); break;
			case 9: s = coco_string_create(L"Comma expected"); break;
			case 10: s = coco_string_create(L"Point expected"); break;
			case 11: s = coco_string_create(L"Colon expected"); break;
			case 12: s = coco_string_create(L"Semicolon expected"); break;
			case 13: s = coco_string_create(L"Slb expected"); break;
			case 14: s = coco_string_create(L"Srb expected"); break;
			case 15: s = coco_string_create(L"Rlb expected"); break;
			case 16: s = coco_string_create(L"Rrb expected"); break;
			case 17: s = coco_string_create(L"Flb expected"); break;
			case 18: s = coco_string_create(L"Frb expected"); break;
			case 19: s = coco_string_create(L"\"do\" expected"); break;
			case 20: s = coco_string_create(L"\"end\" expected"); break;
			case 21: s = coco_string_create(L"\"while\" expected"); break;
			case 22: s = coco_string_create(L"\"repeat\" expected"); break;
			case 23: s = coco_string_create(L"\"until\" expected"); break;
			case 24: s = coco_string_create(L"\"if\" expected"); break;
			case 25: s = coco_string_create(L"\"then\" expected"); break;
			case 26: s = coco_string_create(L"\"elseif\" expected"); break;
			case 27: s = coco_string_create(L"\"else\" expected"); break;
			case 28: s = coco_string_create(L"\"for\" expected"); break;
			case 29: s = coco_string_create(L"\"in\" expected"); break;
			case 30: s = coco_string_create(L"\"function\" expected"); break;
			case 31: s = coco_string_create(L"\"local\" expected"); break;
			case 32: s = coco_string_create(L"\"return\" expected"); break;
			case 33: s = coco_string_create(L"\"break\" expected"); break;
			case 34: s = coco_string_create(L"\"nil\" expected"); break;
			case 35: s = coco_string_create(L"\"false\" expected"); break;
			case 36: s = coco_string_create(L"\"true\" expected"); break;
			case 37: s = coco_string_create(L"\"...\" expected"); break;
			case 38: s = coco_string_create(L"\"+\" expected"); break;
			case 39: s = coco_string_create(L"\"-\" expected"); break;
			case 40: s = coco_string_create(L"\"*\" expected"); break;
			case 41: s = coco_string_create(L"\"/\" expected"); break;
			case 42: s = coco_string_create(L"\"^\" expected"); break;
			case 43: s = coco_string_create(L"\"%\" expected"); break;
			case 44: s = coco_string_create(L"\"..\" expected"); break;
			case 45: s = coco_string_create(L"\"<\" expected"); break;
			case 46: s = coco_string_create(L"\"<=\" expected"); break;
			case 47: s = coco_string_create(L"\">\" expected"); break;
			case 48: s = coco_string_create(L"\">=\" expected"); break;
			case 49: s = coco_string_create(L"\"==\" expected"); break;
			case 50: s = coco_string_create(L"\"~=\" expected"); break;
			case 51: s = coco_string_create(L"\"and\" expected"); break;
			case 52: s = coco_string_create(L"\"or\" expected"); break;
			case 53: s = coco_string_create(L"\"not\" expected"); break;
			case 54: s = coco_string_create(L"\"#\" expected"); break;
			case 55: s = coco_string_create(L"??? expected"); break;
			case 56: s = coco_string_create(L"invalid Stat"); break;
			case 57: s = coco_string_create(L"invalid Stat"); break;
			case 58: s = coco_string_create(L"invalid Stat"); break;
			case 59: s = coco_string_create(L"invalid Laststat"); break;
			case 60: s = coco_string_create(L"invalid Functioncall"); break;
			case 61: s = coco_string_create(L"invalid Exp1"); break;
			case 62: s = coco_string_create(L"invalid Binop"); break;
			case 63: s = coco_string_create(L"invalid Exp2"); break;
			case 64: s = coco_string_create(L"invalid Unop"); break;
			case 65: s = coco_string_create(L"invalid Number"); break;
			case 66: s = coco_string_create(L"invalid String"); break;
			case 67: s = coco_string_create(L"invalid Prefixexp"); break;
			case 68: s = coco_string_create(L"invalid Prefixexp2"); break;
			case 69: s = coco_string_create(L"invalid Args"); break;
			case 70: s = coco_string_create(L"invalid Prefixexp1"); break;
			case 71: s = coco_string_create(L"invalid Parlist"); break;
			case 72: s = coco_string_create(L"invalid Parlist1"); break;
			case 73: s = coco_string_create(L"invalid Field"); break;
			case 74: s = coco_string_create(L"invalid Fieldsep"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}

} // namespace

