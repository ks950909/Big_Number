#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_add
// 기  능 : 두 uword의 덧셈
// c' * b + out = x+y+c , (c,c' < 2)
// x = x0 * (b/2) + x1 ( x0 < 2, x1 < (b/2))
// y = y0 * (b/2) + y1 ( y0 < 2, y1 < (b/2))
// 인  자 : P_UWORD out, const UWORD x, cosnt UWORD y , P_UWORD c ( 해당 설명 위)
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void uword_add(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	// (b/2) -1 ( 최상위 비트 제외 나머지 모두 1)
	const UWORD MASK_except_LMSB_ONE = MASK_WW ^ LMSB_ONE;
	// x1+y1+c 를 out에 저장
	// x1 + y1 + c <= (b/2 - 1) + (b/2 - 1) + 1 = (b - 1)  ( out에 저장가능)
	*out = (x & MASK_except_LMSB_ONE) + (y & MASK_except_LMSB_ONE) + *c;
	// c'을 새로이 만드는데 x0 + y0 + (x1+y1+c)의 최상위 비트를 더함
	// c'은 (x+y+c)>> BITSZ_WW_1 과 같음
	*c = ((x & LMSB_ONE) ? 1u : 0u) + ((y & LMSB_ONE) ? 1u : 0u) + ((*out & LMSB_ONE) ? 1u : 0u);
	// out을 x1+y1+c 의 하위 BITSZ_WW_1비트 저장
	*out &= MASK_except_LMSB_ONE;
	// out의 최상위 비트를 c'을 이용하여 정함
	*out ^= ((*c & 0x1) ? LMSB_ONE : 0u);
	// c' 은 원래 (x+y+c)>> BITSZ_WW이므로 1칸 더 밀어줌
	// c' <= 3이므로 새로운 c'<= 1
	*c >>= 1;
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_add_new
// 기  능 : 두 uword의 덧셈(new version)
// c' * 2^32 + out = x+y+c , (c,c' < 2)
// 인  자 : P_UWORD out, const UWORD x, cosnt UWORD y , P_UWORD c ( 해당 설명 위)
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void uword_add_new(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	// out = x+y+c
	*out = x + y + *c;
	// carry bit = 0 : ((out > x, out > y, c = 1) or (c = 0 , out >= x , out >= y))  the other : carry bit = 1
	*c = (((*out) > x && (*out) > y && (*c) == 1u) || ((*out) >= x && (*out) >= y && (*c) == 0u)) ? 0u : 1u;
}
#define uword_add_new2(out,x,y,c) (out) = (x)+(y)+(c);(c)=((out)>=(x)+(c))&&((out)>=(y)+(c)) ? 0u : 1u;

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_add
// 기  능 : 두 BINT의 절댓값 덧셈
// out = |x| + |y|
// 인  자 : BINT out, const BINT x, const BINT y (해당 설명 위)
// 리턴값 : 없음
// 제한 사항 : x,y의 길이 > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_add(BINT out, const BINT x, const BINT y)
{
	// 자리올림 c
	UWORD c;
	// for index i , out의 최대길이 n
	SINT i, n;
	// 제한사항 체크
	assert(x->len > 0); assert(y->len > 0);
	// 최대 길이 계산
	n = (x->len > y->len ? x->len : y->len) + 1;
	// 동적 할당
	makebint(out, n);
	// STEP 1 : c←0 (c is the carry digit).
	c = 0;
	// STEP 2 :  For i from 0 to n - 1 do the following:
	for (i = 0; i < n; ++i)
	{
		// STEP 2.1 : (c'* b + w[i]) <- x[i] + y[i] + c , c' <- c
		// 없는 부분은 참조안하고 0대입 ( 0 padding on the high-order positions)
		//uword_add_new(&(out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), &c);
		uword_add_new2((out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), c);
		//c = _addcarry_u32(c, (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), &(out->dat[i]));
	}
	// STEP 3 : 필요없는 0제거(최상위 자리)
	bint_unsigned_makelen(out);
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_add
// 기  능 : 두 BINT의 덧셈
// out = x + y
// 인  자 : BINT out, const BINT x, const BINT y (해당 설명 위)
// 리턴값 : 없음
// 제한 사항 : x,y의 길이 > 0
//
////////////////////////////////////////////////////////////////////////
void bint_add(BINT out, const BINT x, const BINT y)
{
	// out 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq = (out == x || out == y) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// 제한사항 체크
	assert(x->len > 0); assert(y->len > 0);
	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else tmp = (BINT*)malloc(sizeof(BINT));

	// x = 0
	if (x->sig == ZERO_SIG)
	{
		// out = y
		bint_copy(*tmp, y);
	}
	// y = 0
	else if (y->sig == ZERO_SIG)
	{
		// out = x
		bint_copy(*tmp, x);
	}
	// x와 y의 부호가 같은 경우
	else if (x->sig == y->sig)
	{
		// 결과값의 절댓값은 x,y의 절댓값의 합
		//bint_unsigned_add(tmp, x, y);
		bint_unsigned_add(*tmp, x, y);
		// 결과값의 부호는 x의 부호와 같음
		(*tmp)->sig = x->sig;
	}
	// x와 y의 부호가 다른 경우
	else
	{
		// x와 y의 절댓값의 크기비교
		SINT xybig = bint_unsigned_compare(x, y);
		// x의 절댓값이 y보다 큰 경우
		if (xybig > 0)
		{
			// 결과값의 절댓값은 x,y의 절댓값의 차
			bint_unsigned_sub(*tmp, x, y);
			// 결과값의 부호는 x의 부호를 따름
			(*tmp)->sig = x->sig;
		}
		// y의 절댓값이 x보다 큰 경우
		else if (xybig < 0)
		{
			// 결과값의 절댓값은 y,x의 절댓값의 차
			bint_unsigned_sub(*tmp, y, x);
			// 결과값의 부호는 y의 부호를 따름
			(*tmp)->sig = y->sig;
		}
		// x와 y의 절댓값이 같은 경우
		else
		{
			// 결과값은 0
			makebint_zero(*tmp, 1);
		}
	}
	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, *tmp);
		delbint(*tmp);
		free(tmp);
	}
}