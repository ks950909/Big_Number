#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h> // debug 용도

#ifndef _BINT_H_
#define _BINT_H_

/*
#ifdef _DEBUG
#include <crtdbg.h>
#define malloc(s) _malloc_dbg(s,_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
*/

typedef struct  {
	SINT	sig;	/* 1:positive,0:zero,-1:negative */
	UWORD	*dat;
	SINT	len;
} _BINT;
typedef _BINT BINT[1];
// len = 0 : 아직 할당 안된 BINT

/* Macro Constants */

#define POS_SIG	 	 1
#define NEG_SIG		-1
#define ZERO_SIG	 0

#define NEGATIVE(x) ((x) == ZERO_SIG ? ZERO_SIG : ((x) == POS_SIG ? NEG_SIG : POS_SIG))

// debug macro 주석처리 : debug
//#define NDEBUG

/* util */
SINT print(const char nstr[], const BINT n);

SINT fprint(FILE* f, const BINT n);
SINT fscan(FILE* f, BINT n);

/* Big Number delete 0 */
void bint_unsigned_makelen(BINT out);

/* Big Number Compare */
/* Absolute Value */
SINT bint_unsigned_compare(const BINT x, const BINT y);
/* Value */
SINT bint_compare(const BINT x, const BINT y);

// see bit
inline BOOL bint_seebit(const BINT x, SINT where) { if(x->len * BITSZ_WW <= where || where < 0) return 0; return ((x->dat[where / BITSZ_WW] >> (where & BITSZ_WW_1)) & 0x1); }

/* Make Big Number */

void bint_copy(BINT x, const BINT y);

void makebint(BINT n,const SINT size);
/* Make Data 0 */
void makebint_zero(BINT n, const SINT size);
/* Make Data from UWORD */
void uword2bint(BINT n, const UWORD x);
/* Make Data from UWORD Array */
void uwordarr2bint(BINT n, const P_UWORD x, const SINT sz, const SINT sig);
/* Memory Delete on Heap */
void delbint(BINT n);



/* Big Number Operation */

/* UWORD Addition */
void uword_add(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c);
void uword_add_new(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c);

/* Big Number Absolute Value Addition */
void bint_unsigned_add(BINT out, const BINT x, const BINT y);

/* Big Number Addition */
void bint_add(BINT out, const BINT x, const BINT y);

/* UWORD Subtraction */
void uword_sub(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c);

/* Big Number Absolute Value Subtraction */
void bint_unsigned_sub(BINT out, const BINT x, const BINT y);

/* Big Number Subtraction */
void bint_sub(BINT out, const BINT x, const BINT y);


UWORD uword_add_atmul(P_UWORD out, const UWORD x, P_UWORD c);
void uword_mul(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c);

/* Big Number Absolute Value Multiplication */
void bint_unsigned_mul(BINT out, const BINT x, const BINT y);

/* Big Number Multiplication */
void bint_mul(BINT out, const BINT x, const BINT y);

void uword_2mul(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c, P_UWORD d);
/* Big Number Absolute Value Square*/
void bint_unsigned_sqr(BINT out, const BINT x);

/* Big Number Square*/
void bint_sqr(BINT out, const BINT x);

SINT make_leftshift_val(UWORD num, const BINT x, const BINT y, BINT nx, BINT ny);

UWORD bisection(const BINT x, const BINT y);
void bint_unsigned_div_step2(BINT out1, BINT newx, const BINT newy, const SINT nmt);
void bint_unsigned_div_step3_1(const BINT newx, const BINT newy, BINT out1, BINT tmp, const BINT yt, const SINT idx, const SINT t);
void bint_unsigned_div_step3_3(BINT newx, const BINT newy, BINT out1, const SINT idx, const SINT t);
void bint_unsigned_div_makesubarr(BINT out1, BINT tmp, BINT yt, const BINT newy, const SINT n, const SINT t);
void bint_unsigned_div_free(BINT newx, BINT newy, BINT tmp, BINT yt);

/* Big Number Absolute Value Divide*/
void bint_unsigned_div(BINT out1, BINT out2, const BINT x, const BINT y);
/* Big Number Divide*/
void bint_div(BINT out1, BINT out2, const BINT x, const BINT y);

/* shift*/
UWORD uword_leftshift_add(P_UWORD out1, P_UWORD out2, const UWORD x, UWORD z);
SINT make_leftshift_val(UWORD num, const BINT x, const BINT y, BINT nx, BINT ny);
void bint_leftshift(BINT out, const BINT x, const SINT y);
void bint_rightshift(BINT out, const BINT x, const SINT y);

/* Montgomery reduction*/

void bint_classical_modular_multiplication(BINT out, const BINT m, const BINT x, const BINT y);
UWORD UWORD_inv(UWORD x);
void bint_montgomery_reduction(BINT out, const BINT m, const BINT T,const UWORD minv);
void bint_montgomery_multiplication(BINT out, const BINT m, const BINT x, const BINT y, const UWORD minv);



// headear del
//void bint_mul_reduction_classical_test(BINT out, const BINT m, const BINT x, const BINT y);

void bint_mul_reduction_classical(BINT out, const BINT m, const BINT x, const BINT y);
void bint_sqr_reduction_classical(BINT out, const BINT m, const BINT x);

void makeconst_montgomery();
BINT M_montgomery;
UWORD MP_montgomery;

void bint_mul_reduction_montgomery(BINT out, const BINT m, const BINT x, const BINT y);
void bint_sqr_reduction_montgomery(BINT out, const BINT m, const BINT x);

void bint_rtol_exponentiation(BINT out, const BINT g, const BINT e, const BINT m, void (*mul)(BINT, const BINT, const BINT, const BINT), void (*sqr)(BINT, const BINT, const BINT));
void bint_ltor_exponentiation(BINT out, const BINT g, const BINT e, const BINT m, void (*mul)(BINT, const BINT, const BINT, const BINT), void (*sqr)(BINT, const BINT, const BINT));
void bint_ltor_kary_exponentiation(BINT out, const BINT g, const BINT e, const BINT m, const SINT k, void (*mul)(BINT, const BINT, const BINT, const BINT), void (*sqr)(BINT, const BINT, const BINT));
void bint_ltor_kary_modify_exponentiation(BINT out, const BINT g, const BINT e, const BINT m, const SINT k, void (*mul)(BINT, const BINT, const BINT, const BINT), void (*sqr)(BINT, const BINT, const BINT));
void bint_slidingwindow_exponentiation(BINT out, const BINT g, const BINT e, const BINT m, const SINT k, void (*mul)(BINT, const BINT, const BINT, const BINT), void (*sqr)(BINT, const BINT, const BINT));
void bint_montogomery_exponentiation(BINT out, const BINT g, const BINT e, const BINT m, const UWORD mp, const BINT rm, const BINT r2m, void (*montmul)(BINT ,const BINT ,const BINT,const BINT,const UWORD));


void bint_montmul_redmul(BINT out, const BINT m, const BINT x, const BINT y, const UWORD mp);
void bint_montmul_redmulsqr(BINT out, const BINT m, const BINT x, const BINT y, const UWORD mp);
void bint_montmul_montmul(BINT out, const BINT m, const BINT x, const BINT y, const UWORD mp);
#endif
