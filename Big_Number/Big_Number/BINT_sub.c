#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_sub
// 기  능 : 두 uword의 뺄셈
// -c' * b + out = x - y - c (c,c' < 2)
// 인  자 : P_UWORD out, const UWORD x, const UWORD y,P_UWORD c (해당 설명 위)
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void uword_sub(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	// y가 b - 1이며 c가 1이라면 y+c = b
	// c' = 1, out = x
	if (y == MASK_WW && *c == 1)
	{
		*out = x; *c = 1;
	}
	// 그 이외의 경우 y+c < 2^32
	else
	{
		// x가 더 큰 경우 c' = 0 , out = x-y-c, 0 <= x-y-c < b
		if (x >= (y + *c))
		{
			*out = x - (y + *c); *c = 0;
		}
		// x가 더 작은 경우 c' = 1 , out = b + x - y - c  (0 < b + x - y - c < b)
		// b + x - y - c = (b - 1 + x - y - c) + 1
		// b - 1 + x - y - c = ~(y + c - x) , y + c - x > 0 이므로 계산 가능
		else
		{
			*out = ~((y + *c) - x) + 1; *c = 1;
		}
	}
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_sub_new
// 기  능 : 두 uword의 뺄셈
// -c' * b + out = x - y - c (c,c' < 2)
// 인  자 : P_UWORD out, const UWORD x, const UWORD y,P_UWORD c (해당 설명 위)
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void uword_sub_new(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	*out = x - y - *c;
	*c = (x >= y + *c) ? 0u : 1u;
}
#define uword_sub_new2(out,x,y,c) (out)=(x)-(y)-(c);(c)=((x) >= (y) + (c)) ? 0u : 1u;


////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_sub
// 기  능 : 두 BINT의 절댓값 뺄셈
// out = |x| - |y|
// 인  자 : BINT out, const BINT x,  const BINT y
// 리턴값 : 없음
// 제한 사항 : |x| >= |y| (그 이외의 경우 에러출력) , x,y의 길이 > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_sub(BINT out, const BINT x, const BINT y)
{
	// 자리내림 c
	UWORD c;
	// for index i , 최대길이 n
	SINT i, n;
	// xycompare : x,y의 절댓값을 비교하여 x > y :1 , x = y : 0 , x < y : -1
	SINT xycompare;

	// 제한사항 체크
	assert(x->len > 0); assert(y->len > 0);

	// xycompare : x,y의 절댓값을 비교하여 x > y :1 , x = y : 0 , x < y : -1
	xycompare = bint_unsigned_compare(x, y);
	// y > x인 경우 오류 출력하며 out에 0 대입
	if (xycompare < 0)
	{
		// 디버깅시 에러
		assert(0);
		// 오류 출력
		fprintf(stderr, "Error at BINT.c -> bint_unsigned_sub -> x,y compare x < y\n");
		// out 에 0 대입
		makebint_zero(out, 1);
		// 함수 종료
		return;
	}
	// y = x인 경우 0출력
	if (xycompare == 0)
	{
		// out 에 0 대입
		makebint_zero(out, 1);
		// 함수 종료
		return;
	}
	// 최대 길이 계산
	n = x->len;
	// 동적 할당
	makebint(out, n);
	// STEP 1 : c←0 (c is the carry digit).
	c = 0;
	// STEP 2 :  For i from 0 to n-1 do the following:
	for (i = 0; i < n; ++i)
	{
		// STEP 2.1 : (w[i] - b * c') <- x[i] - y[i] - c , c' <- c
		//uword_sub_new(&(out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), &c);
		uword_sub_new2((out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), c);
		//subborrow_u32(c,(x->len > i ? x->dat[i] : 0u),(y->len > i ? y->dat[i] : 0u),(out->dat[i]));
	}
	// STEP 3 : 최상위자리 0제거
	bint_unsigned_makelen(out);
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_sub
// 기  능 : 두 BINT의 뺄셈
// out = x - y
// 인  자 : BINT out, const BINT x,  const BINT y
// 리턴값 : 없음
// 제한 사항 : x,y의 길이 > 0
//
////////////////////////////////////////////////////////////////////////
void bint_sub(BINT out, const BINT x, const BINT y)
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
	if (x->sig == 0)
	{
		// 결과값은 -y와 같음
		bint_copy((*tmp), y);
		(*tmp)->sig = NEGATIVE((*tmp)->sig);
	}
	// y = 0
	else if (y->sig == 0)
	{
		// 결과값은 x와 같음
		bint_copy((*tmp), x);
	}
	// x 와 y의 부호가 다른경우
	else if (x->sig != y->sig)
	{
		// 결과값의 절댓값은 x,y의 절댓값의 합
		bint_unsigned_add((*tmp), x, y);
		// x의 부호를 따라감
		(*tmp)->sig = x->sig;
	}
	// x와 y의 부호가 같은 경우
	else
	{
		// x와 y의 절댓값의 크기비교
		SINT xybig = bint_unsigned_compare(x, y);
		// x의 절댓값이 y의 절댓값보다 큰 경우
		if (xybig > 0)
		{
			// 결과값의 절댓값은 x,y의 절댓값의 차
			bint_unsigned_sub((*tmp), x, y);
			// 결과값의 부호는 x의 부호와 같음
			(*tmp)->sig = x->sig;
		}
		// y의 절댓값이 x의 절댓값보다 큰 경우
		else if (xybig < 0)
		{
			// 결과값의 절댓값은 y,x의 절댓값의 차
			bint_unsigned_sub((*tmp), y, x);
			// 결과값의 부호는 x의 부호와 반대
			(*tmp)->sig = -x->sig;
		}
		// x와 y의 절댓값이 같은 경우
		else
		{
			// 결과값은 0
			makebint_zero((*tmp), 1);
		}
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