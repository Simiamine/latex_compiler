/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "algo.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALGOS 100
#define MAX_PARAMS 10
#define MAX_VARS 100

typedef struct ExprNode ExprNode;

typedef struct ArgNode {
    ExprNode *expr;
    struct ArgNode *next;
} ArgNode;

typedef enum { ISET, IRETURN, ICALL, IFOR, IIF } InstrType;
typedef enum { E_CONST, E_VAR, E_ADD, E_SUB, E_MUL, E_DIV, E_EQ, E_CALL } ExprType;

struct ExprNode {
    ExprType type;
    int value;
    char *var_name;
    ExprNode *left;
    ExprNode *right;
    char *call_name;
    ArgNode *args;
};

typedef struct Instruction {
    InstrType type;
    char *var_name;
    ExprNode *expr;
    char *call_name;
    ArgNode *args;
    struct Instruction *next;
    char *loop_var;
    ExprNode *from_expr;
    ExprNode *to_expr;
    struct Instruction *for_body;
    ExprNode *cond;
    struct Instruction *if_body;
    struct Instruction *else_body;
} Instruction;

typedef struct {
    char *name;
    char *params[MAX_PARAMS];
    int param_count;
    Instruction *instructions;
} Algo;

typedef struct {
    char *name;
    int value;
} Variable;

Algo algo_table[MAX_ALGOS];
int algo_count = 0;
char *param_list[MAX_PARAMS];
int param_index = 0;
Variable vars[MAX_VARS];
int var_count = 0;

int yylex(void);
void yyerror(const char *s);
int call_function(char* name, ArgNode* args);
int exec_instructions(Instruction *instr);

int get_var(const char *name) {
    for (int i = 0; i < var_count; i++)
        if (!strcmp(vars[i].name, name))
            return vars[i].value;
    fprintf(stderr, "Variable inconnue : %s\n", name);
    exit(1);
}

void set_var(const char *name, int val) {
    for (int i = 0; i < var_count; i++) {
        if (!strcmp(vars[i].name, name)) {
            vars[i].value = val;
            return;
        }
    }
    vars[var_count].name = strdup(name);
    vars[var_count].value = val;
    var_count++;
}

ArgNode* new_arg(ExprNode* e) {
    ArgNode* n = malloc(sizeof(*n));
    n->expr = e;
    n->next = NULL;
    return n;
}

ArgNode* add_arg(ArgNode* l, ExprNode* e) {
    ArgNode* n = new_arg(e);
    if (!l) return n;
    ArgNode* t = l;
    while (t->next) t = t->next;
    t->next = n;
    return l;
}

int eval_expr(ExprNode *e) {
    switch (e->type) {
        case E_CONST: return e->value;
        case E_VAR: return get_var(e->var_name);
        case E_ADD: return eval_expr(e->left) + eval_expr(e->right);
        case E_SUB: return eval_expr(e->left) - eval_expr(e->right);
        case E_MUL: return eval_expr(e->left) * eval_expr(e->right);
        case E_DIV: return eval_expr(e->left) / eval_expr(e->right);
        case E_EQ: return eval_expr(e->left) == eval_expr(e->right);
        case E_CALL: {
            printf("[EVAL] eval CALL %s(...)\n", e->call_name);
            return call_function(e->call_name, e->args);
        }
    }
    return 0;
}

int call_function(char* name, ArgNode* args) {
    Algo* target = NULL;
    for (int i = 0; i < algo_count; i++) {
        if (strcmp(algo_table[i].name, name) == 0) {
            target = &algo_table[i];
            break;
        }
    }

    if (!target) {
        printf("[ERROR] fonction inconnue : %s\n", name);
        exit(1);
    }

    ArgNode* current = args;
    for (int i = 0; i < target->param_count; i++) {
        if (!current) {
            printf("[ERROR] pas assez d'arguments pour %s\n", name);
            exit(1);
        }
        int val = eval_expr(current->expr);
        set_var(target->params[i], val);
        current = current->next;
    }

    if (current) {
        printf("[ERROR] trop d'arguments pour %s\n", name);
        exit(1);
    }

    printf("[EXEC] CALL %s(", name);
    for (int i = 0; i < target->param_count; i++) {
        printf("%d", get_var(target->params[i]));
        if (i < target->param_count - 1) printf(", ");
    }
    printf(")\n");

    int result = exec_instructions(target->instructions);
    return result;
}

int exec_instructions(Instruction *instr) {
    while (instr) {
        switch (instr->type) {
            case ISET:
                printf("[EXEC] SET %s = ...\n", instr->var_name);
                set_var(instr->var_name, eval_expr(instr->expr));
                break;
            case IRETURN: {
                int ret = eval_expr(instr->expr);
                printf("[EXEC] RETURN → %d\n", ret);
                return ret;
            }
            case ICALL:
                printf("[EXEC] CALL %s(...)\n", instr->call_name);
                call_function(instr->call_name, instr->args);
                break;
            case IFOR: {
                int start = eval_expr(instr->from_expr);
                int end = eval_expr(instr->to_expr);
                printf("[EXEC] FOR %s FROM %d TO %d\n", instr->loop_var, start, end);
                for (int i = start; i <= end; i++) {
                    set_var(instr->loop_var, i);
                    exec_instructions(instr->for_body);
                }
                break;
            }
            case IIF: {
                int cond = eval_expr(instr->cond);
                printf("[EXEC] IF condition = %d\n", cond);
                if (cond) {
                    return exec_instructions(instr->if_body);
                } else if (instr->else_body) {
                    return exec_instructions(instr->else_body);
                }
                break;
            }
        }
        instr = instr->next;
    }
    return 0;
}

#line 277 "algo.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "algo.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_NUMBER = 4,                     /* NUMBER  */
  YYSYMBOL_TSET = 5,                       /* TSET  */
  YYSYMBOL_TRETURN = 6,                    /* TRETURN  */
  YYSYMBOL_TIF = 7,                        /* TIF  */
  YYSYMBOL_TELSE = 8,                      /* TELSE  */
  YYSYMBOL_TFI = 9,                        /* TFI  */
  YYSYMBOL_TDOWHILE = 10,                  /* TDOWHILE  */
  YYSYMBOL_TDOFORI = 11,                   /* TDOFORI  */
  YYSYMBOL_TOD = 12,                       /* TOD  */
  YYSYMBOL_TCALL = 13,                     /* TCALL  */
  YYSYMBOL_TBEGINALGO = 14,                /* TBEGINALGO  */
  YYSYMBOL_TENDALGO = 15,                  /* TENDALGO  */
  YYSYMBOL_PLUS = 16,                      /* PLUS  */
  YYSYMBOL_MINUS = 17,                     /* MINUS  */
  YYSYMBOL_TIMES = 18,                     /* TIMES  */
  YYSYMBOL_DIVIDE = 19,                    /* DIVIDE  */
  YYSYMBOL_ASSIGN = 20,                    /* ASSIGN  */
  YYSYMBOL_EQ = 21,                        /* EQ  */
  YYSYMBOL_NEQ = 22,                       /* NEQ  */
  YYSYMBOL_LE = 23,                        /* LE  */
  YYSYMBOL_GE = 24,                        /* GE  */
  YYSYMBOL_LT = 25,                        /* LT  */
  YYSYMBOL_GT = 26,                        /* GT  */
  YYSYMBOL_AND = 27,                       /* AND  */
  YYSYMBOL_OR = 28,                        /* OR  */
  YYSYMBOL_NOT = 29,                       /* NOT  */
  YYSYMBOL_LBRACE = 30,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 31,                    /* RBRACE  */
  YYSYMBOL_COMMA = 32,                     /* COMMA  */
  YYSYMBOL_YYACCEPT = 33,                  /* $accept  */
  YYSYMBOL_programme = 34,                 /* programme  */
  YYSYMBOL_call_stmt = 35,                 /* call_stmt  */
  YYSYMBOL_algos = 36,                     /* algos  */
  YYSYMBOL_algo = 37,                      /* algo  */
  YYSYMBOL_params = 38,                    /* params  */
  YYSYMBOL_bloc = 39,                      /* bloc  */
  YYSYMBOL_instruction = 40,               /* instruction  */
  YYSYMBOL_for_block = 41,                 /* for_block  */
  YYSYMBOL_if_block = 42,                  /* if_block  */
  YYSYMBOL_else_block = 43,                /* else_block  */
  YYSYMBOL_args = 44,                      /* args  */
  YYSYMBOL_expression = 45                 /* expression  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   129

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  33
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  32
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  92

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   287


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   234,   234,   240,   248,   249,   253,   266,   267,   268,
     272,   273,   282,   290,   297,   305,   306,   310,   323,   335,
     336,   340,   341,   342,   346,   352,   358,   365,   372,   379,
     386,   393,   400
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER", "NUMBER",
  "TSET", "TRETURN", "TIF", "TELSE", "TFI", "TDOWHILE", "TDOFORI", "TOD",
  "TCALL", "TBEGINALGO", "TENDALGO", "PLUS", "MINUS", "TIMES", "DIVIDE",
  "ASSIGN", "EQ", "NEQ", "LE", "GE", "LT", "GT", "AND", "OR", "NOT",
  "LBRACE", "RBRACE", "COMMA", "$accept", "programme", "call_stmt",
  "algos", "algo", "params", "bloc", "instruction", "for_block",
  "if_block", "else_block", "args", "expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-61)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      11,   -13,    33,    14,   -61,    36,   -61,    19,   -61,   -61,
      10,    71,    46,    53,    93,    78,   -61,    20,    69,    84,
     106,   -61,   -61,    80,    23,    82,    83,    85,    86,    87,
     108,    25,   -61,   -61,   -61,   -61,   109,   -61,    69,    69,
      69,    69,    69,    69,    69,   111,    69,    69,   115,    88,
     -61,   -61,    89,    82,    82,    82,    82,    82,    82,    82,
      90,    -5,     3,    91,    94,    95,    96,   -61,    84,    97,
      69,    69,    69,    72,    69,    73,    75,    27,    84,   114,
      44,   -61,   -61,   -61,    84,   -61,    98,    69,    50,    84,
      81,   -61
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     4,     0,     1,     0,     2,     5,
       0,     0,     0,     0,     7,     0,     8,     0,    21,     0,
       0,    25,    24,     0,     0,    22,     0,     0,     0,     0,
       0,     0,    10,    15,    16,     9,     0,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       6,    11,     0,    23,    26,    27,    28,    29,    31,    30,
       0,     0,     0,     0,     0,     0,     0,    13,     0,     0,
      21,    21,     0,    19,     0,     0,     0,     0,     0,     0,
       0,    14,    32,    12,    20,    18,     0,     0,     0,     0,
       0,    17
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -61,   -61,   -61,   -61,   126,   -61,   -60,   -31,   -61,   -61,
     -61,   -14,   -37
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     8,     3,     4,    17,    31,    32,    33,    34,
      79,    24,    25
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      51,    53,    54,    55,    56,    57,    58,    59,    73,    61,
      62,    39,    40,    41,    42,    43,    44,     5,    84,    39,
      40,    41,    42,    43,    44,     1,    67,     7,     1,    90,
      26,    27,    28,     6,    68,    77,    29,    80,    30,    10,
      50,    12,    51,    39,    40,    41,    42,    43,    44,    11,
      88,    19,    20,    51,    37,    38,    75,    76,    83,    51,
      39,    40,    41,    42,    43,    44,    39,    40,    41,    42,
      43,    44,    21,    22,    13,    86,    14,    26,    27,    28,
      78,    89,    23,    29,    15,    30,    26,    27,    28,    26,
      27,    28,    29,    91,    30,    29,    16,    30,    39,    40,
      41,    42,    43,    44,    81,    38,    82,    38,    18,    35,
      36,    49,    52,    45,    60,    46,    47,    48,    63,    64,
      65,    66,    69,    85,    70,    71,    72,    74,    87,     9
};

static const yytype_int8 yycheck[] =
{
      31,    38,    39,    40,    41,    42,    43,    44,    68,    46,
      47,    16,    17,    18,    19,    20,    21,    30,    78,    16,
      17,    18,    19,    20,    21,    14,    31,    13,    14,    89,
       5,     6,     7,     0,    31,    72,    11,    74,    13,     3,
      15,    31,    73,    16,    17,    18,    19,    20,    21,    30,
      87,    31,    32,    84,    31,    32,    70,    71,    31,    90,
      16,    17,    18,    19,    20,    21,    16,    17,    18,    19,
      20,    21,     3,     4,     3,    31,    30,     5,     6,     7,
       8,    31,    13,    11,    31,    13,     5,     6,     7,     5,
       6,     7,    11,    12,    13,    11,     3,    13,    16,    17,
      18,    19,    20,    21,    31,    32,    31,    32,    30,     3,
      30,     3,     3,    30,     3,    30,    30,    30,     3,    31,
      31,    31,    31,     9,    30,    30,    30,    30,    30,     3
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    14,    34,    36,    37,    30,     0,    13,    35,    37,
       3,    30,    31,     3,    30,    31,     3,    38,    30,    31,
      32,     3,     4,    13,    44,    45,     5,     6,     7,    11,
      13,    39,    40,    41,    42,     3,    30,    31,    32,    16,
      17,    18,    19,    20,    21,    30,    30,    30,    30,     3,
      15,    40,     3,    45,    45,    45,    45,    45,    45,    45,
       3,    45,    45,     3,    31,    31,    31,    31,    31,    31,
      30,    30,    30,    39,    30,    44,    44,    45,     8,    43,
      45,    31,    31,    31,    39,     9,    31,    30,    45,    31,
      39,    12
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    33,    34,    35,    36,    36,    37,    38,    38,    38,
      39,    39,    40,    40,    40,    40,    40,    41,    42,    43,
      43,    44,    44,    44,    45,    45,    45,    45,    45,    45,
      45,    45,    45
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     7,     1,     2,     9,     0,     1,     3,
       1,     2,     7,     4,     6,     1,     1,    12,     7,     0,
       2,     0,     1,     3,     1,     1,     3,     3,     3,     3,
       3,     3,     7
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* programme: algos call_stmt  */
#line 234 "algo.y"
                    {
        printf("[DEBUG] programme → algos call_stmt\n");
    }
#line 1367 "algo.tab.c"
    break;

  case 3: /* call_stmt: TCALL LBRACE IDENTIFIER RBRACE LBRACE args RBRACE  */
#line 240 "algo.y"
                                                      {
        printf("[DEBUG] call_stmt → CALL {%s} {%p}\n", (yyvsp[-4].str), (yyvsp[-1].arglist));
        int res = call_function((yyvsp[-4].str), (yyvsp[-1].arglist));
        printf("[RESULT] Résultat de l'appel à %s : %d\n", (yyvsp[-4].str), res);
    }
#line 1377 "algo.tab.c"
    break;

  case 6: /* algo: TBEGINALGO LBRACE IDENTIFIER RBRACE LBRACE params RBRACE bloc TENDALGO  */
#line 253 "algo.y"
                                                                           {
        printf("[DEBUG] algo → begin %s\n", (yyvsp[-6].str));
        algo_table[algo_count].name = strdup((yyvsp[-6].str));
        algo_table[algo_count].param_count = param_index;
        for (int i = 0; i < param_index; i++)
            algo_table[algo_count].params[i] = strdup(param_list[i]);
        algo_table[algo_count].instructions = (yyvsp[-1].instr);
        algo_count++;
        param_index = 0;
    }
#line 1392 "algo.tab.c"
    break;

  case 7: /* params: %empty  */
#line 266 "algo.y"
               { param_index = 0; }
#line 1398 "algo.tab.c"
    break;

  case 8: /* params: IDENTIFIER  */
#line 267 "algo.y"
                 { param_list[param_index++] = strdup((yyvsp[0].str)); }
#line 1404 "algo.tab.c"
    break;

  case 9: /* params: params COMMA IDENTIFIER  */
#line 268 "algo.y"
                              { param_list[param_index++] = strdup((yyvsp[0].str)); }
#line 1410 "algo.tab.c"
    break;

  case 10: /* bloc: instruction  */
#line 272 "algo.y"
                { (yyval.instr) = (yyvsp[0].instr); }
#line 1416 "algo.tab.c"
    break;

  case 11: /* bloc: bloc instruction  */
#line 273 "algo.y"
                       {
        Instruction *last = (yyvsp[-1].instr);
        while (last->next) last = last->next;
        last->next = (yyvsp[0].instr);
        (yyval.instr) = (yyvsp[-1].instr);
    }
#line 1427 "algo.tab.c"
    break;

  case 12: /* instruction: TSET LBRACE IDENTIFIER RBRACE LBRACE expression RBRACE  */
#line 282 "algo.y"
                                                           {
        Instruction *i = malloc(sizeof(*i));
        i->type = ISET;
        i->var_name = strdup((yyvsp[-4].str));
        i->expr = (yyvsp[-1].expr);
        i->next = NULL;
        (yyval.instr) = i;
    }
#line 1440 "algo.tab.c"
    break;

  case 13: /* instruction: TRETURN LBRACE expression RBRACE  */
#line 290 "algo.y"
                                       {
        Instruction *i = malloc(sizeof(*i));
        i->type = IRETURN;
        i->expr = (yyvsp[-1].expr);
        i->next = NULL;
        (yyval.instr) = i;
    }
#line 1452 "algo.tab.c"
    break;

  case 14: /* instruction: TCALL IDENTIFIER RBRACE LBRACE args RBRACE  */
#line 297 "algo.y"
                                                 {
        Instruction *i = malloc(sizeof(*i));
        i->type = ICALL;
        i->call_name = strdup((yyvsp[-4].str));
        i->args = (yyvsp[-1].arglist);
        i->next = NULL;
        (yyval.instr) = i;
    }
#line 1465 "algo.tab.c"
    break;

  case 15: /* instruction: for_block  */
#line 305 "algo.y"
                { (yyval.instr) = (yyvsp[0].instr); }
#line 1471 "algo.tab.c"
    break;

  case 16: /* instruction: if_block  */
#line 306 "algo.y"
               { (yyval.instr) = (yyvsp[0].instr); }
#line 1477 "algo.tab.c"
    break;

  case 17: /* for_block: TDOFORI LBRACE IDENTIFIER RBRACE LBRACE expression RBRACE LBRACE expression RBRACE bloc TOD  */
#line 310 "algo.y"
                                                                                                {
        Instruction *i = malloc(sizeof(*i));
        i->type = IFOR;
        i->loop_var = strdup((yyvsp[-9].str));
        i->from_expr = (yyvsp[-6].expr);
        i->to_expr = (yyvsp[-3].expr);
        i->for_body = (yyvsp[-1].instr);
        i->next = NULL;
        (yyval.instr) = i;
    }
#line 1492 "algo.tab.c"
    break;

  case 18: /* if_block: TIF LBRACE expression RBRACE bloc else_block TFI  */
#line 323 "algo.y"
                                                     {
        Instruction *i = malloc(sizeof(*i));
        i->type = IIF;
        i->cond = (yyvsp[-4].expr);
        i->if_body = (yyvsp[-2].instr);
        i->else_body = (yyvsp[-1].instr);
        i->next = NULL;
        (yyval.instr) = i;
    }
#line 1506 "algo.tab.c"
    break;

  case 19: /* else_block: %empty  */
#line 335 "algo.y"
               { (yyval.instr) = NULL; }
#line 1512 "algo.tab.c"
    break;

  case 20: /* else_block: TELSE bloc  */
#line 336 "algo.y"
                 { (yyval.instr) = (yyvsp[0].instr); }
#line 1518 "algo.tab.c"
    break;

  case 21: /* args: %empty  */
#line 340 "algo.y"
               { (yyval.arglist) = NULL; }
#line 1524 "algo.tab.c"
    break;

  case 22: /* args: expression  */
#line 341 "algo.y"
                 { (yyval.arglist) = new_arg((yyvsp[0].expr)); }
#line 1530 "algo.tab.c"
    break;

  case 23: /* args: args COMMA expression  */
#line 342 "algo.y"
                            { (yyval.arglist) = add_arg((yyvsp[-2].arglist), (yyvsp[0].expr)); }
#line 1536 "algo.tab.c"
    break;

  case 24: /* expression: NUMBER  */
#line 346 "algo.y"
           {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_CONST;
        e->value = (yyvsp[0].ival);
        (yyval.expr) = e;
    }
#line 1547 "algo.tab.c"
    break;

  case 25: /* expression: IDENTIFIER  */
#line 352 "algo.y"
                 {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_VAR;
        e->var_name = strdup((yyvsp[0].str));
        (yyval.expr) = e;
    }
#line 1558 "algo.tab.c"
    break;

  case 26: /* expression: expression PLUS expression  */
#line 358 "algo.y"
                                 {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_ADD;
        e->left = (yyvsp[-2].expr);
        e->right = (yyvsp[0].expr);
        (yyval.expr) = e;
    }
#line 1570 "algo.tab.c"
    break;

  case 27: /* expression: expression MINUS expression  */
#line 365 "algo.y"
                                  {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_SUB;
        e->left = (yyvsp[-2].expr);
        e->right = (yyvsp[0].expr);
        (yyval.expr) = e;
    }
#line 1582 "algo.tab.c"
    break;

  case 28: /* expression: expression TIMES expression  */
#line 372 "algo.y"
                                  {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_MUL;
        e->left = (yyvsp[-2].expr);
        e->right = (yyvsp[0].expr);
        (yyval.expr) = e;
    }
#line 1594 "algo.tab.c"
    break;

  case 29: /* expression: expression DIVIDE expression  */
#line 379 "algo.y"
                                   {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_DIV;
        e->left = (yyvsp[-2].expr);
        e->right = (yyvsp[0].expr);
        (yyval.expr) = e;
    }
#line 1606 "algo.tab.c"
    break;

  case 30: /* expression: expression EQ expression  */
#line 386 "algo.y"
                               {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_EQ;
        e->left = (yyvsp[-2].expr);
        e->right = (yyvsp[0].expr);
        (yyval.expr) = e;
    }
#line 1618 "algo.tab.c"
    break;

  case 31: /* expression: expression ASSIGN expression  */
#line 393 "algo.y"
                                   {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_EQ;
        e->left = (yyvsp[-2].expr);
        e->right = (yyvsp[0].expr);
        (yyval.expr) = e;
    }
#line 1630 "algo.tab.c"
    break;

  case 32: /* expression: TCALL LBRACE IDENTIFIER RBRACE LBRACE args RBRACE  */
#line 400 "algo.y"
                                                        {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_CALL;
        e->call_name = strdup((yyvsp[-4].str));
        e->args = (yyvsp[-1].arglist);
        (yyval.expr) = e;
    }
#line 1642 "algo.tab.c"
    break;


#line 1646 "algo.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 407 "algo.y"


void yyerror(const char *s) {
    fprintf(stderr, "[ERROR] yyerror: %s\n", s);
}

int main(void) {
    printf("[INFO] Début du parsing\n");
    yyparse();
    printf("[INFO] Fin d'exécution\n");
    return 0;
}
