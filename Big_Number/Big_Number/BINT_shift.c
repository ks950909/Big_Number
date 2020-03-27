#include <assert.h> /* assert */
#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_leftshift_add
// 기  능 : leftshift 후 덧셈
// b * out1 + out2 += x << z
// z 는 BITSZ_WW로 나눈 나머지만 계산
// 인  자 : P_UWORD out1, P_UWORD out2, const UWORD x, const UWORD z
// 리턴값 : UWORD 자리올림 (b * out1 + out2)가 b^2을 넘어가면 1 , 아니면 0
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
UWORD uword_leftshift_add(P_UWORD out1, P_UWORD out2, const UWORD x, const UWORD z)
{
	UWORD y = *out1;
	// z를 32로 나눈 나머지 처리
	UWORD zz = z % BITSZ_WW;

	// leftshift 후 b가 넘어가는 부분
	if (zz != 0)	*out1 += (x >> (BITSZ_WW - zz));
	// leftshift 후에도 b를 넘지 못하는 부분
	uword_add_atmul(out2, x << zz, out1);
	//
	return (y <= *out1 ? 0u : 1u);
}
////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_leftshift
// 기  능 : out = x << y , out의 부호는 음수가안나옴(주의)
// 인  자 : BINT out, const BINT x,  const SINT y
// 리턴값 : 없음
// 제한 사항 : y >= 0 , xlen > 0
//
////////////////////////////////////////////////////////////////////////
void bint_leftshift(BINT out, const BINT x, const SINT y)
{
	// out 이 x와 가르키는 곳이 같은지 체크
	SINT eq = (out == x) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// y / BITSZ_WW , y % BITSZ_WW 저장
	SINT ydiv = y / BITSZ_WW, ymod = y % BITSZ_WW;
	// for index i
	SINT i;

	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else tmp = (BINT*)malloc(sizeof(BINT));

	// 제한 사항 체크
	assert(y >= 0); assert(x->len > 0);
	// out의 최대 길이 = x->len + ydiv + 1
	makebint_zero((*tmp), x->len + ydiv + 1);
	for (i = x->len - 1; i >= 0; --i)
	{
		// 겹치지 않게 더해져서 자리올림 걱정 안해도 됨
		uword_leftshift_add(&(*tmp)->dat[i + 1 + ydiv], &(*tmp)->dat[i + ydiv], x->dat[i], ymod);
	}
	// 최상위자리 0 제거
	bint_unsigned_makelen((*tmp));

	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_rightshift
// 기  능 : out = x >> y; , out의 부호는 음수가안나옴(주의)
// 인  자 : BINT out , const BINT x, const SINT y
// 리턴값 : 없음
// 제한 사항 : y >= 0 , xlen > 0
//
////////////////////////////////////////////////////////////////////////
void bint_rightshift(BINT out, const BINT x, const SINT y)
{
	// out 이 x와 가르키는 곳이 같은지 체크
	SINT eq = (out == x) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// y / BITSZ_WW , y % BITSZ_WW 저장
	SINT ydiv = y / BITSZ_WW, ymod = y % BITSZ_WW;
	// for index i
	SINT i;

	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else tmp = (BINT*)malloc(sizeof(BINT));

	// 제한 사항 체크
	assert(y >= 0); assert(x->len > 0);

	// out의 최대 길이 = x->len - ydiv
	makebint_zero((*tmp), x->len - ydiv);
	// ymod == 0
	if (ymod == 0)
	{
		for (i = x->len - 1; i >= ydiv; --i)
		{
			(*tmp)->dat[i - ydiv] = x->dat[i];
		}
	}
	// ymod != 0
	else
	{
		for (i = x->len - 1; i >= ydiv; --i)
		{
			(*tmp)->dat[i - ydiv] += (x->dat[i] >> ymod);
			if (i != ydiv || ydiv != 0)
				(*tmp)->dat[i - 1 - ydiv] += (x->dat[i] << (BITSZ_WW - ymod));
		}
	}
	// 최상위자리 0 제거
	bint_unsigned_makelen((*tmp));

	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

