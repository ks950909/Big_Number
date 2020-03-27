#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_add_atmul
// 기  능 :
// c * b + out <- c * b + (x+out)
// 인  자 : P_UWORD out, const UWORD x , P_UWORD c
// 리턴값 : 자리올림 되었는가(b^2) ( true : 1 , false : 0
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
UWORD uword_add_atmul(P_UWORD out, const UWORD x, P_UWORD c)
{
	// 자리올림을 위한 임시저장
	UWORD tmp = *out;
	// out에 x를 더함
	*out += x;
	// 만약 자리올림(b)이 일어났다면 *out이 tmp 보다 작아짐
	*c += ((*out) >= tmp) ? 0u : 1u;
	// *c가 0이라면 자리올림 일어남(b^2)
	return ((*c == 0u && (*out < tmp)) ? 1u : 0u);
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_mul
// 기  능 :
// c * b + out <- out + x*y + c
// 인  자 : P_UWORD out, const UWORD x , const UWORD y, P_UWORD c
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void uword_mul(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	// 중간값 저장임시공간
	UWORD w;
	// x = x0 * sqr(b) + x1 , y = y0 * sqr(b) + y1
	// x0,x1,y0,y1 < sqr(b)
	UWORD x0, x1, y0, y1;
	x0 = x >> BITSZ_HW;
	x1 = x & MASK_RHW;
	y0 = y >> BITSZ_HW;
	y1 = y & MASK_RHW;

	// c * b + out <- out + c
	// c <= 1
	w = *c;
	*c = 0;
	uword_add_atmul(out, w, c);

	// c * b + out += x0 * y0 * b
	*c += (x0 * y0);
	// c * b + out += x1 * y1
	uword_add_atmul(out, x1 * y1, c);
	// c * b + out += x1 * y0 * sqrt(b)
	uword_leftshift_add(c, out, x1 * y0, BITSZ_HW);
	// c * b + out += x0 * y1 * sqrt(b)
	uword_leftshift_add(c, out, x0 * y1, BITSZ_HW);

}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_mul
// 기  능 : 두 BINt의 절댓값 곱
// out = |x|*|y|
// 인  자 : BINT out, const BINT x,const BINT y
// 리턴값 : 없음
// 제한 사항 : x,y의 길이 > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_mul(BINT out, const BINT x, const BINT y)
{
	// for index i,j
	SINT i, j;
	// n ,t 는 x,y의 길이
	SINT n = x->len, t = y->len;
	// 자리올림 c
	UWORD c;
	// 제한사항 체크
	assert(n > 0); assert(t > 0);
	// Step 1: output 동적할당 및 초기화 (결과값의 최대길이 : n+t)
	makebint_zero(out, n + t);

	// STEP 2 :  For i from 0 to (t-1) do the following:
	for (i = 0; i < t; ++i)
	{
		// STEP 2.1 : c <- 0
		c = 0u;
		// STEP 2.2 : For j from 0 to (n-1) do the following
		for (j = 0; j < n; ++j)
		{
			// compute (uv) = w[i+j] + x[j] * y[i] + c , w[i+j] = v, c = u
			uword_mul(&out->dat[i + j], x->dat[j], y->dat[i], &c);
		}
		// STEP 2.3 :  w[i+n] = c
		out->dat[i + n] = c;
	}
	// STEP 3. 최상위 자리 0제거
	bint_unsigned_makelen(out);
}

//c = _mulx_u32(x->dat[j], y->dat[i], &out->dat[i + j]); // c*b+(out->dat[i+j])=x[j]*y[i]
////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_mul
// 기  능 : 두 BINT 의 곱
// out = x*y
// 인  자 : BINT out, const BINT x,const BINT y
// 리턴값 : 없음
// 제한 사항 : x,y의 길이 > 0
//
////////////////////////////////////////////////////////////////////////
void bint_mul(BINT out, const BINT x, const BINT y)
{
	// out 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq = (out == x || out == y) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else  tmp = (BINT*)malloc(sizeof(BINT));

	// 제한사항 체크
	assert(x->len > 0); assert(y->len > 0); assert(tmp != NULL);
	// x == 0 or y == 0
	if (x->sig == ZERO_SIG || y->sig == ZERO_SIG)
	{
		// 두 값중 하나가 0이라면 0
		makebint_zero((*tmp), 1);
	}
	else
	{
		// 두 절댓값을 곱해서 tmp에 대입
		bint_unsigned_mul((*tmp), x, y);
		// 결과값의 부호는 두 부호가 같으면 양수, 다르면 음수
		(*tmp)->sig = (x->sig == y->sig ? POS_SIG : NEG_SIG);
	}

	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}