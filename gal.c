// Copyright © 2025 kala_telo <kala_telo@proton.me>
// SPDX-License-Identifier: MIT

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *string;
    int length;
} String;

#define S(s) (String){.string = (s), .length = sizeof(s)-1}
#define PS(s) (s).length, (s).string

typedef enum {
    // MEMORY REFERENCE INSTRUCTIONS
    T_MEM_REF,
    // PROGRAM INTERRUPT
    T_PROG_INT,
    // HIGH-SPEED READER
    T_HS_READER,
    // HIGH-SPEED PUNCH
    T_HS_PUNCH,
    // KEYBOARD/READER
    T_KBD_READER,
    // GROUP 2 OPERATES
    T_GROUP2_OP,
    // TELEPRINTER/PUNCH
    T_TTY_PUNCH,
    // GROUP 1 OPERATES
    T_GROUP1_OP,
    // COMBINED OPERATES
    T_COMBINED_OP,
    // DECTAPE DUAL TRANSPORT TYPE 555 AND CONTROL TYPE 552
    T_DECTAPE_DUAL_555_552,
    // DECTAPE TRANSPORT TYPE TU55 AND CONTROL TYPE TC01
    T_DECTAPE_TU55_TC01,
    // MEMORY PARITY TYPE 188
    T_MEM_PARITY_188,
    // PDP-5 EAE SYMBOLS 153*
    T_PDP5_EAE_153,
    // PDP-8 EAE SYMBOLS 182
    T_PDP8_EAE_182,
    // MEMORY EXTENSION CONTROL TYPE 183
    T_MEM_EXT_CTRL_183,
    // AUTO RESTART TYPE KR01
    T_AUTO_RESTART_KR01,
    // AD CONVERTER TYPE 189
    T_AD_CONV_189,
    // AD CONVERTER/MULTIPLEXER 138E/139E
    T_AD_CONV_MUX_138E_139E,
    // OSCILLOSCOPE DISPLAY TYPE 34D
    T_OSC_DISPLAY_34D,
    // SCOPE TYPE 3ON
    T_SCOPE_3ON,
    // LIGHT PEN TYPE 370
    T_LIGHT_PEN_370,
    // PLOTTER AND CONTROL TYPE 350B
    T_PLOTTER_CTRL_350B,
    // CARD READER AND CONTROL TYPE CR01C
    T_CARD_READER_CTRL_CR01C,
    // CARD READER TYPE 451
    T_CARD_READER_451,
    // CARD PUNCH AND CONTROL TYPE 450
    T_CARD_PUNCH_CTRL_450,
    // LINE PRINTER TYPE 645
    T_LINE_PRINTER_645,
    // SERIAL DRUM 250 AND 251
    T_SERIAL_DRUM_250_251,
    // MAGNETIC TAPE TYPE 57A
    T_MAG_TAPE_57A,
    // MAGNETIC TAPE TYPE 580
    T_MAG_TAPE_580,
    // OPTION TO TYPE 139E MULTIPLEXOR
    T_OPT_139E_MUX,
    // DATA COMMUNICATION SYSTEMS TYPE 630
    T_DATA_COMM_630,
    // FLOATING-POINT INSTRUCTIONS
    T_FLOAT_PT,
} InstKind;

typedef struct {
    String name;
    int16_t opcode;
    InstKind kind;
} Mnemonic;

Mnemonic mnemonics[] = {
    {S("AND"), 00000, T_MEM_REF},
    {S("TAD"), 01000, T_MEM_REF},
    {S("ISZ"), 02000, T_MEM_REF},
    {S("DCA"), 03000, T_MEM_REF},
    {S("JMS"), 04000, T_MEM_REF},
    {S("JMP"), 05000, T_MEM_REF},
    {S("ION"), 06001, T_PROG_INT},
    {S("IOF"), 06002, T_PROG_INT},
    {S("RSF"), 06011, T_HS_READER},
    {S("RRB"), 06012, T_HS_READER},
    {S("RFC"), 06014, T_HS_READER},
    {S("PSF"), 06021, T_HS_PUNCH},
    {S("PCF"), 06022, T_HS_PUNCH},
    {S("PPC"), 06024, T_HS_PUNCH},
    {S("PLS"), 06026, T_HS_PUNCH},
    {S("KSF"), 06031, T_KBD_READER},
    {S("KCC"), 06032, T_KBD_READER},
    {S("KRS"), 06034, T_KBD_READER},
    {S("KRB"), 06036, T_KBD_READER},
    {S("NOP"), 07000, T_GROUP1_OP},
    {S("IAC"), 07001, T_GROUP1_OP},
    {S("RAL"), 07004, T_GROUP1_OP},
    {S("RTL"), 07006, T_GROUP1_OP},
    {S("RAR"), 07010, T_GROUP1_OP},
    {S("RTR"), 07012, T_GROUP1_OP},
    {S("CML"), 07020, T_GROUP1_OP},
    {S("CMA"), 07040, T_GROUP1_OP},
    {S("CLL"), 07100, T_GROUP1_OP},
    {S("CLA"), 07200, T_GROUP1_OP},
    {S("HLT"), 07402, T_GROUP2_OP},
    {S("OSR"), 07404, T_GROUP2_OP},
    {S("SKP"), 07410, T_GROUP2_OP},
    {S("SNL"), 07420, T_GROUP2_OP},
    {S("SZL"), 07430, T_GROUP2_OP},
    {S("SZA"), 07440, T_GROUP2_OP},
    {S("SNA"), 07450, T_GROUP2_OP},
    {S("SMA"), 07500, T_GROUP2_OP},
    {S("SPA"), 07510, T_GROUP2_OP},
    {S("CIA"), 07041, T_COMBINED_OP},
    {S("LAS"), 07604, T_COMBINED_OP},
    {S("STL"), 07120, T_COMBINED_OP},
    {S("GLK"), 07204, T_COMBINED_OP},
    {S("STA"), 07240, T_COMBINED_OP},
    {S("MMMM"), 06757, T_DECTAPE_DUAL_555_552},
    {S("MMSF"), 06761, T_DECTAPE_DUAL_555_552},
    {S("MMMF"), 06756, T_DECTAPE_DUAL_555_552},
    {S("MMCF"), 06772, T_DECTAPE_DUAL_555_552},
    {S("MMML"), 06766, T_DECTAPE_DUAL_555_552},
    {S("MMSC"), 06771, T_DECTAPE_DUAL_555_552},
    {S("MMLS"), 06751, T_DECTAPE_DUAL_555_552},
    {S("MMRS"), 06774, T_DECTAPE_DUAL_555_552},
    {S("MMLM"), 06752, T_DECTAPE_DUAL_555_552},
    {S("MMCC"), 06762, T_DECTAPE_DUAL_555_552},
    {S("MMLF"), 06754, T_DECTAPE_DUAL_555_552},
    {S("MMLC"), 06764, T_DECTAPE_DUAL_555_552},
    {S("DTRA"), 06761, T_DECTAPE_TU55_TC01},
    {S("DTSF"), 06771, T_DECTAPE_TU55_TC01},
    {S("DTCA"), 06762, T_DECTAPE_TU55_TC01},
    {S("DTRB"), 06772, T_DECTAPE_TU55_TC01},
    {S("DTXA"), 06764, T_DECTAPE_TU55_TC01},
    {S("DTLB"), 06774, T_DECTAPE_TU55_TC01},
    {S("SMP"), 06101, T_MEM_PARITY_188},
    {S("CMP"), 06104, T_MEM_PARITY_188},
    {S("CAM"), 06101, T_PDP5_EAE_153},
    {S("SZO"), 06114, T_PDP5_EAE_153},
    {S("LMQ"), 06102, T_PDP5_EAE_153},
    {S("DIV"), 06121, T_PDP5_EAE_153},
    {S("LAR"), 06104, T_PDP5_EAE_153},
    {S("RDM"), 06122, T_PDP5_EAE_153},
    {S("MUL"), 06111, T_PDP5_EAE_153},
    {S("SAF"), 06124, T_PDP5_EAE_153},
    {S("RDA"), 06112, T_PDP5_EAE_153},
    {S("MUY"), 07405, T_PDP8_EAE_182},
    {S("ASR"), 07415, T_PDP8_EAE_182},
    {S("DVI"), 07407, T_PDP8_EAE_182},
    {S("LSR"), 07417, T_PDP8_EAE_182},
    {S("NMI"), 07411, T_PDP8_EAE_182},
    {S("MQL"), 07421, T_PDP8_EAE_182},
    {S("SHL"), 07413, T_PDP8_EAE_182},
    {S("SCA"), 07441, T_PDP8_EAE_182},
    {S("MQA"), 07501, T_PDP8_EAE_182},
    {S("CAM"), 07621, T_PDP8_EAE_182},
    {S("CDF"), 06201, T_MEM_EXT_CTRL_183},
    {S("RIF"), 06224, T_MEM_EXT_CTRL_183},
    {S("CIF"), 06202, T_MEM_EXT_CTRL_183},
    {S("RMF"), 06244, T_MEM_EXT_CTRL_183},
    {S("RDF"), 06214, T_MEM_EXT_CTRL_183},
    {S("RIB"), 06234, T_MEM_EXT_CTRL_183},
    {S("SPL"), 06102, T_AUTO_RESTART_KR01},
    {S("ADC"), 06004, T_AD_CONV_189},
    {S("ADSF"), 06531, T_AD_CONV_MUX_138E_139E},
    {S("ADCC"), 06541, T_AD_CONV_MUX_138E_139E},
    {S("ADCV"), 06532, T_AD_CONV_MUX_138E_139E},
    {S("ADSC"), 06542, T_AD_CONV_MUX_138E_139E},
    {S("ADRB"), 06534, T_AD_CONV_MUX_138E_139E},
    {S("ADIC"), 06544, T_AD_CONV_MUX_138E_139E},
    {S("DCX"), 06051, T_OSC_DISPLAY_34D},
    {S("DYL"), 06063, T_OSC_DISPLAY_34D},
    {S("DXL"), 06053, T_OSC_DISPLAY_34D},
    {S("DIX"), 06054, T_OSC_DISPLAY_34D},
    {S("DCY"), 06061, T_OSC_DISPLAY_34D},
    {S("DIY"), 06064, T_OSC_DISPLAY_34D},
    {S("DXS"), 06057, T_OSC_DISPLAY_34D},
    {S("DYS"), 06067, T_OSC_DISPLAY_34D},
    {S("DLB"), 06074, T_SCOPE_3ON},
    {S("DSF"), 06071, T_LIGHT_PEN_370},
    {S("DCF"), 06072, T_LIGHT_PEN_370},
    {S("PLSF"), 06501, T_PLOTTER_CTRL_350B},
    {S("PLCF"), 06502, T_PLOTTER_CTRL_350B},
    {S("PLPU"), 06504, T_PLOTTER_CTRL_350B},
    {S("PLPR"), 06511, T_PLOTTER_CTRL_350B},
    {S("PLPU"), 06512, T_PLOTTER_CTRL_350B},
    {S("PLDD"), 06514, T_PLOTTER_CTRL_350B},
    {S("PLPL"), 06521, T_PLOTTER_CTRL_350B},
    {S("PLUD"), 06522, T_PLOTTER_CTRL_350B},
    {S("PLPD"), 06524, T_PLOTTER_CTRL_350B},
    {S("RCSF"), 06631, T_CARD_READER_451},
    {S("RCSP"), 06671, T_CARD_READER_451},
    {S("RCRA"), 06632, T_CARD_READER_451},
    {S("RCSE"), 06671, T_CARD_READER_451},
    {S("RCRB"), 06634, T_CARD_READER_451},
    {S("RCRD"), 06674, T_CARD_READER_451},
    {S("CRSF"), 06632, T_CARD_READER_451},
    {S("CERS"), 06634, T_CARD_READER_451},
    {S("CRRB"), 06671, T_CARD_READER_451},
    {S("CRSA"), 06672, T_CARD_READER_451},
    {S("CRSB"), 06674, T_CARD_READER_451},
    {S("CPSF"), 06631, T_CARD_PUNCH_CTRL_450},
    {S("CPSE"), 06642, T_CARD_PUNCH_CTRL_450},
    {S("CPLB"), 06644, T_CARD_PUNCH_CTRL_450},
    {S("CPCF"), 06641, T_CARD_PUNCH_CTRL_450},
    {S("LCF"), 06652, T_LINE_PRINTER_645},
    {S("LPR"), 06655, T_LINE_PRINTER_645},
    {S("LSF"), 06661, T_LINE_PRINTER_645},
    {S("LCB"), 06662, T_LINE_PRINTER_645},
    {S("LLB"), 06664, T_LINE_PRINTER_645},
    {S("DRCR"), 06603, T_SERIAL_DRUM_250_251},
    {S("DRCF"), 06611, T_SERIAL_DRUM_250_251},
    {S("DRTS"), 06615, T_SERIAL_DRUM_250_251},
    {S("DRSC"), 06622, T_SERIAL_DRUM_250_251},
    {S("DRCW"), 06605, T_SERIAL_DRUM_250_251},
    {S("DREF"), 06612, T_SERIAL_DRUM_250_251},
    {S("DRSE"), 06621, T_SERIAL_DRUM_250_251},
    {S("DRCN"), 06624, T_SERIAL_DRUM_250_251},
    {S("MSCR"), 06701, T_MAG_TAPE_57A},
    {S("MTS"), 06706, T_MAG_TAPE_57A},
    {S("MNC"), 06712, T_MAG_TAPE_57A},
    {S("MSWF"), 06721, T_MAG_TAPE_57A},
    {S("MCWF"), 06722, T_MAG_TAPE_57A},
    {S("MIWF"), 06722, T_MAG_TAPE_57A},
    {S("MDEF"), 06732, T_MAG_TAPE_57A},
    {S("MEEF"), 06732, T_MAG_TAPE_57A},
    {S("MTRS"), 06734, T_MAG_TAPE_57A},
    {S("MRWC"), 06742, T_MAG_TAPE_57A},
    {S("MCD"), 06702, T_MAG_TAPE_57A},
    {S("MSUR"), 06711, T_MAG_TAPE_57A},
    {S("MTC"), 06716, T_MAG_TAPE_57A},
    {S("MDWF"), 06722, T_MAG_TAPE_57A},
    {S("MEWF"), 06722, T_MAG_TAPE_57A},
    {S("MSEF"), 06731, T_MAG_TAPE_57A},
    {S("MCED"), 06732, T_MAG_TAPE_57A},
    {S("MIEF"), 06732, T_MAG_TAPE_57A},
    {S("MCC"), 06741, T_MAG_TAPE_57A},
    {S("MRCA"), 06744, T_MAG_TAPE_57A},
    {S("MCA"), 06745, T_MAG_TAPE_57A},
    {S("TSRD"), 06715, T_MAG_TAPE_580},
    {S("TSWR"), 06716, T_MAG_TAPE_580},
    {S("TSSR"), 06722, T_MAG_TAPE_580},
    {S("TWRT"), 06731, T_MAG_TAPE_580},
    {S("TSRS"), 06734, T_MAG_TAPE_580},
    {S("TIFM"), 06707, T_MAG_TAPE_580},
    {S("TSDF"), 06721, T_MAG_TAPE_580},
    {S("TSST"), 06724, T_MAG_TAPE_580},
    {S("TCPI"), 06732, T_MAG_TAPE_580},
    {S("TTINCR"), 06401, T_DATA_COMM_630},
    {S("TTI"), 06402, T_DATA_COMM_630},
    {S("TTO"), 06404, T_DATA_COMM_630},
    {S("TTCL"), 06411, T_DATA_COMM_630},
    {S("TTSL"), 06412, T_DATA_COMM_630},
    {S("TTRL"), 06414, T_DATA_COMM_630},
    {S("TTSKP"), 06421, T_DATA_COMM_630},
    {S("TTXON"), 06422, T_DATA_COMM_630},
    {S("TTXOF"), 06424, T_DATA_COMM_630},
    {S("FEXT"), 00000, T_FLOAT_PT},
    {S("FADD"), 01000, T_FLOAT_PT},
    {S("FSUB"), 02000, T_FLOAT_PT},
    {S("FMPY"), 03000, T_FLOAT_PT},
    {S("FDIV"), 04000, T_FLOAT_PT},
    {S("FGET"), 05000, T_FLOAT_PT},
    {S("FPUT"), 06000, T_FLOAT_PT},
    {S("FNOR"), 07000, T_FLOAT_PT},
    {S("TSF"),  06041, T_TTY_PUNCH},
    {S("TCF"),  06042, T_TTY_PUNCH},
    {S("TLS"),  06046, T_TTY_PUNCH},
    {S("TPC"),  06044, T_TTY_PUNCH},
};

#define ARRLEN(xs) (sizeof(xs)/sizeof(*(xs)))

#define UNREACHABLE()                                                          \
    do {                                                                       \
        fprintf(stderr, "%s:%d: UNREACHABLE -- It shouldn've not happen...\n", \
                __FILE__, __LINE__);                                           \
        abort();                                                               \
    } while (0)
#define TODO()                                                                 \
    do {                                                                       \
        fprintf(stderr, "%s:%d: TODO\n", __FILE__, __LINE__);                  \
        abort();                                                               \
    } while (0)

typedef enum {
    B_OCT,
    B_BIN,
    B_DEC,
    B_HEX,
} Base;

static inline int s_atoi(String s, Base base) {
    int b;
    switch (base) {
    case B_BIN:
        b = 2;
        break;
    case B_DEC:
        b = 10;
        break;
    case B_OCT:
        b = 8;
        break;
    case B_HEX:
        TODO();
        b = 16;
        break;
    }
    int64_t result = 0;
    for (int len = 0; len < s.length; len++) {
        result *= b;
        result += s.string[len] - '0';
    }
    return result;
}

static inline bool string_eq(String s1, String s2) {
    if (s1.length != s2.length) {
        return false;
    }
    return memcmp(s1.string, s2.string, s1.length) == 0;
}

#define da_append(xs, x)                                                       \
    do {                                                                       \
        if ((xs).len + 1 > (xs).cap) {                                         \
            if ((xs).cap != 0) {                                               \
                (xs).cap *= 2;                                                 \
            } else {                                                           \
                (xs).cap = 4;                                                  \
            }                                                                  \
            (xs).data = realloc((xs).data, sizeof(*(xs).data) * (xs).cap);     \
            assert((xs).data != NULL);                                         \
        }                                                                      \
        (xs).data[(xs).len++] = (x);                                           \
    } while (0)

static inline bool find_mnem(Mnemonic *out, String name) {
    for (size_t i = 0; i < ARRLEN(mnemonics); i++) {
        if (string_eq(mnemonics[i].name, name)) {
            if (out) *out = mnemonics[i];
            return true;
        }
    }
    return false;
}

#define PLOC(x) (x).file, (x).line+1, (x).col+1
typedef struct {
    uint16_t line;
    uint8_t col;
    char *file;
} Loc;

typedef enum {
    LEX_END,
    LEX_ADDRESS,
    LEX_NAME,
    LEX_INST,
    LEX_INT,
    LEX_PLUS,
    LEX_EQ,
    LEX_COMMA,
    LEX_DOT,
    LEX_MINUS,
    LEX_SEMICOLON,
    LEX_CHARACTER,
    LEX_NEWLINE,
} TokenKind;

const char *const lex_names[] = {
    [LEX_END] = "<EOF>",
    [LEX_ADDRESS] = "<address>",
    [LEX_NAME] = "<name>",
    [LEX_INST] = "<INST>",
    [LEX_INT] = "<int>",
    [LEX_PLUS] = "`+`",
    [LEX_EQ] = "`=`",
    [LEX_COMMA] = "`,`",
    [LEX_DOT] = "`.`",
    [LEX_MINUS] = "`-`",
    [LEX_SEMICOLON] = "`,`",
    [LEX_CHARACTER] = "\"<character>",
    [LEX_NEWLINE] = "<newline>",
};

typedef struct {
    TokenKind  kind;
    String str;
    Loc loc;
} Token;

typedef struct {
    int16_t addr, opcode, arg;
} Inst;

typedef struct {
    String name; int16_t value;
} NameEntry;

typedef struct {
    char *code;
    size_t len;
    Loc loc;
} Lexer;

typedef struct {
    Token cause;
    int16_t addr;
    Lexer lexer;
    Base base;
} BackpatchEntry;

// global variables
struct {
    NameEntry *data;
    size_t len, cap;
} names = { 0 };

struct {
    BackpatchEntry *data;
    size_t len, cap;
} backpatch = { 0 };

uint16_t ram[4096] = { 0 };
// ----

static inline bool find_name(int16_t *out, String name) {
    for (size_t i = 0; i < names.len; i++) {
        if (string_eq(names.data[i].name, name)) {
            *out = names.data[i].value;
            return true;
        }
    }
    return false;
}

bool eat_char(Lexer *lex) {
    if (lex->len < 1) {
        return false;
    }
    if (*lex->code++ == '\n') {
        lex->loc.line++;
        lex->loc.col = 0;
    } else {
        lex->loc.col++;
    }
    lex->len--;
    return true;
}

bool is_kind_binop(TokenKind k) {
    switch (k) {
    case LEX_PLUS:
        return true;
    case LEX_MINUS:
        return true;
    default:
        return false;
    }
}

#define expect_any(token, ...) _expect_any(token, ARRLEN(((TokenKind[]){__VA_ARGS__})), ((TokenKind[]){__VA_ARGS__}))
Token _expect_any(Token t, size_t count, TokenKind *ks) {
    for (size_t i = 0; i < count; i++) {
        if (t.kind == ks[i]) {
            return t;
        }
    }
    fprintf(stderr, "%s:%d:%d Expected any of: ", PLOC(t.loc));
    for (size_t i = 0; i < count; i++) {
        fprintf(stderr, "%s, ", lex_names[ks[i]]);
    }
    fprintf(stderr, "but got %s\n", lex_names[t.kind]);
    exit(1);
    return (Token){0};
}

Token expect(Token t, TokenKind k) {
    if (t.kind != k) {
        fprintf(stderr, "%s:%d:%d Expected %s but got %s\n", PLOC(t.loc),
                lex_names[k], lex_names[t.kind]);
        exit(1);
    }
    return t;
}

Token next_token(Lexer *lex) {
    while ((*lex->code == ' ' || *lex->code == '\t' || *lex->code == '\r') && lex->len > 0) {
        eat_char(lex);
    }
    if (lex->len == 0) {
        return (Token){
            .kind = LEX_END, .str = (String){lex->code, 0}, .loc = lex->loc};
    }
    switch (*lex->code) {
    case '/':
        while (*lex->code != '\n')
            eat_char(lex);
        return next_token(lex);
    case '*': {
        eat_char(lex);
        String addr = {
            .string = lex->code,
            .length = 0,
        };
        while (isdigit(*lex->code)) {
            eat_char(lex);
            addr.length++;
        }
        return (Token){
            .kind = LEX_ADDRESS,
            .str = addr,
            .loc = lex->loc
        };
    } break;
    case '=':
        eat_char(lex);
        return (Token){.kind = LEX_EQ,
                       .str = (String){lex->code-1, 1},
                       .loc = lex->loc};
    case ',':
        eat_char(lex);
        return (Token){.kind = LEX_COMMA,
                       .str = (String){lex->code-1, 1},
                       .loc = lex->loc};
    case '.':
        eat_char(lex);
        return (Token){.kind = LEX_DOT,
                       .str = (String){lex->code-1, 1},
                       .loc = lex->loc};
    case '-':
        eat_char(lex);
        return (Token){.kind = LEX_MINUS,
                       .str = (String){lex->code-1, 1},
                       .loc = lex->loc};
    case '+':
        eat_char(lex);
        return (Token){.kind = LEX_PLUS,
                       .str = (String){lex->code-1, 1},
                       .loc = lex->loc};
    case ';':
        eat_char(lex);
        return (Token){.kind = LEX_SEMICOLON,
                       .str = (String){lex->code-1, 1},
                       .loc = lex->loc};
    case '"':
        eat_char(lex); eat_char(lex);
        return (Token){.kind = LEX_CHARACTER,
                       .str = (String){lex->code-1, 1},
                       .loc = lex->loc};
    case '\n':
        eat_char(lex);
        return (Token){.kind = LEX_NEWLINE,
                       .str = (String){lex->code, 1},
                       .loc = lex->loc};
    case '$':
        return (Token){.kind = LEX_END,
                       .str = (String){lex->code, 1},
                       .loc = lex->loc};
    default:
        if (isdigit(*lex->code)) {
            String word = {
                .string = lex->code,
                .length = 1,
            };
            if (!eat_char(lex))
                goto fail;
            while (isalnum(*lex->code)) {
                word.length++;
                if (!eat_char(lex))
                    goto fail;
            }
            return (Token){.kind = LEX_INT, .str = word, .loc = lex->loc};
        } else if (isalnum(*lex->code)) {
            String word = {
                .string = lex->code,
                .length = 1,
            };
            if (!eat_char(lex))
                goto fail;
            while (isalnum(*lex->code)) {
                word.length++;
                if (!eat_char(lex))
                    goto fail;
            }
            if (find_mnem(NULL, word))
                return (Token){.kind = LEX_INST, .str = word, .loc = lex->loc};
            else
                return (Token){.kind = LEX_NAME, .str = word, .loc = lex->loc};
        }
    }
fail:
    fprintf(stderr, "%s:%d:%d Unexpected value '%c' (%d)\n", lex->loc.file,
            lex->loc.line + 1, lex->loc.col, *lex->code, *lex->code);
    TODO();
    return (Token) { 0 };
}

Token peek_token(Lexer *lex) {
    Lexer copy = *lex;
    return next_token(&copy);
}

Token peek_token_n(Lexer *lex, size_t n) {
    Lexer copy = *lex;
    assert(n > 0);
    for (size_t i = 0; i < n-1; i++)
        next_token(&copy);
    return next_token(&copy);
}

int16_t parse_var_or_int(Lexer *lex, Base base, int16_t addr) {
    Token t = expect_any(next_token(lex), LEX_NAME, LEX_INT, LEX_DOT);
    switch (t.kind) {
    case LEX_NAME: {
        int16_t v;
        if (find_name(&v, t.str)) {
            return v;
        } else {
            return -1;
        }
    } break;
    case LEX_INT:
        return s_atoi(t.str, base);
    case LEX_DOT:
        return addr;
    default:
        UNREACHABLE();
        break;
    }
    UNREACHABLE();
    return -1;
}

int16_t parse_expr(Lexer *lex, Base base, int16_t addr, Token *bp_cause) {
    Token potential_bp_cause = peek_token(lex);
    int16_t v = parse_var_or_int(lex, base, addr);
    if (v < 0)
        *bp_cause = potential_bp_cause;
    while (is_kind_binop(peek_token(lex).kind)) {
        TokenKind op = next_token(lex).kind;
        int16_t dv = parse_var_or_int(lex, base, addr);
        switch (op) {
        case LEX_PLUS:
            if (v >= 0) v += dv;
            break;
        case LEX_MINUS:
            if (v >= 0) v -= dv;
            break;
        default:
            TODO();
        }
    }
    return v;
}

int16_t assemble_mnemonic(Lexer *lex, Mnemonic mnem, Base *base, int16_t *addr, Token *bp_cause) {
    Loc l = expect(next_token(lex), LEX_INST).loc;
    switch (mnem.kind) {
    case T_MEM_REF: {
        int16_t I = 0, Z = 0;
        if (string_eq(peek_token(lex).str, S("I"))) {
            next_token(lex);
            I = 1<<8;
        }
        int16_t v = parse_expr(lex, *base, *addr, bp_cause);
        if (v > 0200) {
            Z = 1<<7;
        }
        if (v >= 0) {
            if (v/128 != *addr/128 && Z != 0) {
                fprintf(stderr, "%s:%d:%d: %o is not on the same page as %o\n",
                        PLOC(l), v, *addr);
                exit(1);
            }
            return mnem.opcode | I | Z | (v & 0x7F);
        } else {
            return -1;
        }
    } break;
    case T_PROG_INT:
        TODO();
        break;
    case T_HS_READER:
        TODO();
        break;
    case T_HS_PUNCH:
        TODO();
        break;
    case T_KBD_READER:
    case T_GROUP2_OP:
    case T_TTY_PUNCH:
    case T_COMBINED_OP:
    case T_PDP8_EAE_182:
    case T_GROUP1_OP:
        return mnem.opcode;
    case T_DECTAPE_DUAL_555_552:
        TODO();
        break;
    case T_DECTAPE_TU55_TC01:
        TODO();
        break;
    case T_MEM_PARITY_188:
        TODO();
        break;
    case T_PDP5_EAE_153:
        TODO();
        break;
    case T_MEM_EXT_CTRL_183:
        TODO();
        break;
    case T_AUTO_RESTART_KR01:
        TODO();
        break;
    case T_AD_CONV_189:
        TODO();
        break;
    case T_AD_CONV_MUX_138E_139E:
        TODO();
        break;
    case T_OSC_DISPLAY_34D:
        TODO();
        break;
    case T_SCOPE_3ON:
        TODO();
        break;
    case T_LIGHT_PEN_370:
        TODO();
        break;
    case T_PLOTTER_CTRL_350B:
        TODO();
        break;
    case T_CARD_READER_CTRL_CR01C:
        TODO();
        break;
    case T_CARD_READER_451:
        TODO();
        break;
    case T_CARD_PUNCH_CTRL_450:
        TODO();
        break;
    case T_LINE_PRINTER_645:
        TODO();
        break;
    case T_SERIAL_DRUM_250_251:
        TODO();
        break;
    case T_MAG_TAPE_57A:
        TODO();
        break;
    case T_MAG_TAPE_580:
        TODO();
        break;
    case T_OPT_139E_MUX:
        TODO();
        break;
    case T_DATA_COMM_630:
        TODO();
        break;
    case T_FLOAT_PT:
        TODO();
        break;
    }
    return -1;
}

void assemble_once(Lexer *lex, Base *base, int16_t *addr) {
    switch (peek_token(lex).kind) {
    case LEX_ADDRESS:
        *addr = s_atoi(next_token(lex).str, *base);
        break;
    case LEX_INST: {
        Mnemonic mnem;
        BackpatchEntry potential_bp = (BackpatchEntry){
            .cause = peek_token(lex),
            .addr = *addr,
            .base = *base,
            .lexer = *lex,
        };
        if (peek_token_n(lex, 2).kind == LEX_EQ) {
            Token t = next_token(lex);
            next_token(lex);
            if (!find_mnem(&mnem, t.str)) UNREACHABLE();
            int16_t n = parse_expr(lex, *base, *addr, &potential_bp.cause);
            if (n < 0) {
                da_append(backpatch, potential_bp);
                break;
            }
            if (n != mnem.opcode) {
                fprintf(stderr,
                        "%s:%d:%d Redefining mnemonics is not supported! "
                        "(%.*s)\n",
                        PLOC(t.loc), PS(t.str));
                exit(1);
            }
            break;
        }
        int16_t r = 0;
        while (peek_token(lex).kind != LEX_NEWLINE && peek_token(lex).kind != LEX_END) {
            Token t = expect(peek_token(lex), LEX_INST);
            potential_bp.cause = t;
            if (find_mnem(&mnem, t.str)) {
                int16_t o = assemble_mnemonic(lex, mnem, base, addr, &potential_bp.cause);
                if (o >= 0) r |= o;
                else da_append(backpatch, potential_bp);
            } else UNREACHABLE();
        }
        ram[(*addr)++] = r;
    } break;
    case LEX_NAME: {
        if (string_eq(peek_token(lex).str, S("DECIMAL"))) {
            next_token(lex);
            *base = B_DEC;
            break;
        }
        if (string_eq(peek_token(lex).str, S("OCTAL"))) {
            next_token(lex);
            *base = B_OCT;
            break;
        }
        BackpatchEntry potential_bp = (BackpatchEntry){
            .addr = *addr,
            .base = *base,
            .lexer = *lex,
        };
        Token t = next_token(lex);
        switch (peek_token(lex).kind) {
        case LEX_EQ:
            next_token(lex);
            String vs = expect(next_token(lex), LEX_INT).str;
            int16_t v = s_atoi(vs, *base);
            da_append(names, ((NameEntry){t.str, v}));
            break;
        case LEX_COMMA:
            da_append(names, ((NameEntry){t.str, *addr}));
            next_token(lex);
            break;
        default: {
            *lex = potential_bp.lexer;
            *addr = potential_bp.addr;
            *base = potential_bp.base;
            potential_bp.cause = t;
            int16_t v = parse_expr(lex, *base, *addr, &potential_bp.cause);
            if (v < 0) {
                da_append(backpatch, potential_bp);
                (*addr)++;
            } else {
                ram[(*addr)++] = v;
            }
        } break;
        }
    } break;
    case LEX_INT:
        ram[(*addr)++] = s_atoi(next_token(lex).str, *base);
        break;
    case LEX_EQ:
        fprintf(stderr, "%s:%d:%d\n", PLOC(peek_token(lex).loc));
        TODO();
        break;
    case LEX_COMMA:
        fprintf(stderr, "%s:%d:%d\n", PLOC(peek_token(lex).loc));
        TODO();
        break;
    case LEX_DOT:
        TODO();
        break;
    case LEX_MINUS:
        fprintf(stderr, "%s:%d:%d\n", PLOC(peek_token(lex).loc));
        TODO();
        break;
    case LEX_PLUS:
        TODO();
        break;
    case LEX_NEWLINE:
        next_token(lex);
        break;
    case LEX_SEMICOLON:
        TODO();
        break;
    case LEX_CHARACTER:
        ram[(*addr)++] = next_token(lex).str.string[0];
        break;
    case LEX_END:
        break;
    }
}

void assemble(Lexer *lex) {
    Base base = B_OCT;
    int16_t addr = 0200;

    while (peek_token(lex).kind != LEX_END) {
        assemble_once(lex, &base, &addr);
    }
    size_t bp_count = backpatch.len;
    for (size_t i = 0; i < bp_count; i++) {
        BackpatchEntry bp = backpatch.data[i];
        assemble_once(&bp.lexer, &bp.base, &bp.addr);
    }
    // if new requests for backpatching were introduced during backpatching,
    // then those are undefined variables
    for (size_t i = bp_count; i < backpatch.len; i++) { 
        BackpatchEntry bp = backpatch.data[i];
        printf("%s:%d:%d: Error: Undefined name `%.*s`\n", PLOC(bp.cause.loc), PS(bp.cause.str));
    }
    if (backpatch.len > bp_count) {
        exit(1);
    }
    backpatch.len = 0;
}

void export_dec_obj(FILE *out) {
#define O(x) fputc((checksum += (x), (x)), out)
    for (int i = 1; i < 240; i++) {
        fputc(0200, out);
    }
    uint16_t checksum = 0;
    O(0x10);
    O(0x00);
    for (size_t i = 1; i < ARRLEN(ram); i++) {
        O((ram[i] >> 6) & 0x3F);
        O(ram[i] & 0x3F);
    }
    fputc((checksum >> 6) & 0x3F, out);
    fputc(checksum & 0x3F, out);
    fputc(0200, out);
#undef O
}

char *next_arg(int* argc, char ***argv, char* error) {
    if (*argc == 0) {
        if (error != NULL) {
            fprintf(stderr, "%s\n", error);
        }
        exit(1);
    }
    char *result = **argv;
    (*argc)--;
    (*argv)++;
    return result;
}

int main(int argc, char *argv[]) {
    char *program_name = next_arg(&argc, &argv, NULL),
         *input_file   = NULL,
         *output_file  = NULL;
    while (argc) {
        char *arg = next_arg(&argc, &argv, NULL);
        if (strcmp(arg, "-o") == 0) {
            output_file = next_arg(&argc, &argv, "Argument `-o` expects output filename next");
        } else if (strcmp(arg, "-static") == 0) { // just compatibility with GAS
        } else {
            if (!input_file) {
                input_file = arg;
            } else {
                fprintf(stderr, "GAL doesn't accept more than 1 input file: %s, %s provided.\n", input_file, arg);
                return 1;
            }
        }
    }
    if (!input_file) {
        fprintf(stderr, "No input file was provided.\n");
        return 1;
    }
    if (!output_file) {
        fprintf(stderr, "No output file was provided.\n");
        return 1;
    }
    FILE *f = fopen(input_file, "r");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s\n", input_file);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    String str = {0};
    str.string = malloc(size * sizeof(char));
    str.length = size;

    size = fread(str.string, 1, size, f);
    fclose(f);

    // -1 for \0, though it still must be in memory for
    // lexer to not read into unallocated memory
    Lexer lex = (Lexer){
        .len = size - 1,
        .code = str.string,
        .loc = (Loc){0, 0, input_file},
    };
    assemble(&lex);

    f = fopen(output_file, "wb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open `%s`\n", output_file);
        return 1;
    }
    export_dec_obj(f);
    fclose(f);
}
