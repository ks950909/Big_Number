#include <assert.h> /* assert */
#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_leftshift_add
// ��  �� : leftshift �� ����
// b * out1 + out2 += x << z
// z �� BITSZ_WW�� ���� �������� ���
// ��  �� : P_UWORD out1, P_UWORD out2, const UWORD x, const UWORD z
// ���ϰ� : UWORD �ڸ��ø� (b * out1 + out2)�� b^2�� �Ѿ�� 1 , �ƴϸ� 0
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
UWORD uword_leftshift_add(P_UWORD out1, P_UWORD out2, const UWORD x, const UWORD z)
{
	UWORD y = *out1;
	// z�� 32�� ���� ������ ó��
	UWORD zz = z % BITSZ_WW;

	// leftshift �� b�� �Ѿ�� �κ�
	if (zz != 0)	*out1 += (x >> (BITSZ_WW - zz));
	// leftshift �Ŀ��� b�� ���� ���ϴ� �κ�
	uword_add_atmul(out2, x << zz, out1);
	//
	return (y <= *out1 ? 0u : 1u);
}
////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_leftshift
// ��  �� : out = x << y , out�� ��ȣ�� �������ȳ���(����)
// ��  �� : BINT out, const BINT x,  const SINT y
// ���ϰ� : ����
// ���� ���� : y >= 0 , xlen > 0
//
////////////////////////////////////////////////////////////////////////
void bint_leftshift(BINT out, const BINT x, const SINT y)
{
	// out �� x�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// y / BITSZ_WW , y % BITSZ_WW ����
	SINT ydiv = y / BITSZ_WW, ymod = y % BITSZ_WW;
	// for index i
	SINT i;

	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else tmp = (BINT*)malloc(sizeof(BINT));

	// ���� ���� üũ
	assert(y >= 0); assert(x->len > 0);
	// out�� �ִ� ���� = x->len + ydiv + 1
	makebint_zero((*tmp), x->len + ydiv + 1);
	for (i = x->len - 1; i >= 0; --i)
	{
		// ��ġ�� �ʰ� �������� �ڸ��ø� ���� ���ص� ��
		uword_leftshift_add(&(*tmp)->dat[i + 1 + ydiv], &(*tmp)->dat[i + ydiv], x->dat[i], ymod);
	}
	// �ֻ����ڸ� 0 ����
	bint_unsigned_makelen((*tmp));

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
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
// �Լ��� : bint_rightshift
// ��  �� : out = x >> y; , out�� ��ȣ�� �������ȳ���(����)
// ��  �� : BINT out , const BINT x, const SINT y
// ���ϰ� : ����
// ���� ���� : y >= 0 , xlen > 0
//
////////////////////////////////////////////////////////////////////////
void bint_rightshift(BINT out, const BINT x, const SINT y)
{
	// out �� x�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// y / BITSZ_WW , y % BITSZ_WW ����
	SINT ydiv = y / BITSZ_WW, ymod = y % BITSZ_WW;
	// for index i
	SINT i;

	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else tmp = (BINT*)malloc(sizeof(BINT));

	// ���� ���� üũ
	assert(y >= 0); assert(x->len > 0);

	// out�� �ִ� ���� = x->len - ydiv
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
	// �ֻ����ڸ� 0 ����
	bint_unsigned_makelen((*tmp));

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

