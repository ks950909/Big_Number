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
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == m || out == T) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
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
