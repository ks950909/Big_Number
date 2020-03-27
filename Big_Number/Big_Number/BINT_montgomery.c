#include "BINT.h"


UWORD UWORD_inv(UWORD a)
{
	UWORD q = 1u,tmp = a,ttmp=1u;
	UWORD r = a;
	while (r != 1u)
	{
		ttmp <<= 1;
		tmp  <<= 1;
		if ((r & ttmp) != 0u)
		{
			q += ttmp;
			r += tmp;
		}
	}
	return q;
}

void bint_montgomery_reduction(BINT out, const BINT m, const BINT T)
{
	SINT i;
	SINT n = m->len;
	BINT u,tmpval;
	UWORD minv = -UWORD_inv(m->dat[0]);
	// out 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq = (out == m || out == T) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else  tmp = (BINT*)malloc(sizeof(BINT));

	bint_copy((*tmp), T);
	uword2bint(u, 1u);

	for (i = 0; i < n; i++)
	{
		u->dat[0] = (*tmp)->dat[i] * minv;
		bint_mul(tmpval, m, u);
		bint_leftshift(tmpval, tmpval, BITSZ_WW * i);
		bint_add((*tmp), (*tmp), tmpval);
		delbint(tmpval);
	}
	bint_rightshift((*tmp), (*tmp), BITSZ_WW * n);
	bint_unsigned_makelen(*tmp);
	delbint(u);
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}
