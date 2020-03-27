#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_sub
// ��  �� : �� uword�� ����
// -c' * b + out = x - y - c (c,c' < 2)
// ��  �� : P_UWORD out, const UWORD x, const UWORD y,P_UWORD c (�ش� ���� ��)
// ���ϰ� : ����
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
void uword_sub(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	// y�� b - 1�̸� c�� 1�̶�� y+c = b
	// c' = 1, out = x
	if (y == MASK_WW && *c == 1)
	{
		*out = x; *c = 1;
	}
	// �� �̿��� ��� y+c < 2^32
	else
	{
		// x�� �� ū ��� c' = 0 , out = x-y-c, 0 <= x-y-c < b
		if (x >= (y + *c))
		{
			*out = x - (y + *c); *c = 0;
		}
		// x�� �� ���� ��� c' = 1 , out = b + x - y - c  (0 < b + x - y - c < b)
		// b + x - y - c = (b - 1 + x - y - c) + 1
		// b - 1 + x - y - c = ~(y + c - x) , y + c - x > 0 �̹Ƿ� ��� ����
		else
		{
			*out = ~((y + *c) - x) + 1; *c = 1;
		}
	}
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_sub_new
// ��  �� : �� uword�� ����
// -c' * b + out = x - y - c (c,c' < 2)
// ��  �� : P_UWORD out, const UWORD x, const UWORD y,P_UWORD c (�ش� ���� ��)
// ���ϰ� : ����
// ���� ���� : ����
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
// �Լ��� : bint_unsigned_sub
// ��  �� : �� BINT�� ���� ����
// out = |x| - |y|
// ��  �� : BINT out, const BINT x,  const BINT y
// ���ϰ� : ����
// ���� ���� : |x| >= |y| (�� �̿��� ��� �������) , x,y�� ���� > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_sub(BINT out, const BINT x, const BINT y)
{
	// �ڸ����� c
	UWORD c;
	// for index i , �ִ���� n
	SINT i, n;
	// xycompare : x,y�� ������ ���Ͽ� x > y :1 , x = y : 0 , x < y : -1
	SINT xycompare;

	// ���ѻ��� üũ
	assert(x->len > 0); assert(y->len > 0);

	// xycompare : x,y�� ������ ���Ͽ� x > y :1 , x = y : 0 , x < y : -1
	xycompare = bint_unsigned_compare(x, y);
	// y > x�� ��� ���� ����ϸ� out�� 0 ����
	if (xycompare < 0)
	{
		// ������ ����
		assert(0);
		// ���� ���
		fprintf(stderr, "Error at BINT.c -> bint_unsigned_sub -> x,y compare x < y\n");
		// out �� 0 ����
		makebint_zero(out, 1);
		// �Լ� ����
		return;
	}
	// y = x�� ��� 0���
	if (xycompare == 0)
	{
		// out �� 0 ����
		makebint_zero(out, 1);
		// �Լ� ����
		return;
	}
	// �ִ� ���� ���
	n = x->len;
	// ���� �Ҵ�
	makebint(out, n);
	// STEP 1 : c��0 (c is the carry digit).
	c = 0;
	// STEP 2 :  For i from 0 to n-1 do the following:
	for (i = 0; i < n; ++i)
	{
		// STEP 2.1 : (w[i] - b * c') <- x[i] - y[i] - c , c' <- c
		//uword_sub_new(&(out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), &c);
		uword_sub_new2((out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), c);
		//subborrow_u32(c,(x->len > i ? x->dat[i] : 0u),(y->len > i ? y->dat[i] : 0u),(out->dat[i]));
	}
	// STEP 3 : �ֻ����ڸ� 0����
	bint_unsigned_makelen(out);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_sub
// ��  �� : �� BINT�� ����
// out = x - y
// ��  �� : BINT out, const BINT x,  const BINT y
// ���ϰ� : ����
// ���� ���� : x,y�� ���� > 0
//
////////////////////////////////////////////////////////////////////////
void bint_sub(BINT out, const BINT x, const BINT y)
{
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x || out == y) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// ���ѻ��� üũ
	assert(x->len > 0); assert(y->len > 0);
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else tmp = (BINT*)malloc(sizeof(BINT));
	// x = 0
	if (x->sig == 0)
	{
		// ������� -y�� ����
		bint_copy((*tmp), y);
		(*tmp)->sig = NEGATIVE((*tmp)->sig);
	}
	// y = 0
	else if (y->sig == 0)
	{
		// ������� x�� ����
		bint_copy((*tmp), x);
	}
	// x �� y�� ��ȣ�� �ٸ����
	else if (x->sig != y->sig)
	{
		// ������� ������ x,y�� ������ ��
		bint_unsigned_add((*tmp), x, y);
		// x�� ��ȣ�� ����
		(*tmp)->sig = x->sig;
	}
	// x�� y�� ��ȣ�� ���� ���
	else
	{
		// x�� y�� ������ ũ���
		SINT xybig = bint_unsigned_compare(x, y);
		// x�� ������ y�� ���񰪺��� ū ���
		if (xybig > 0)
		{
			// ������� ������ x,y�� ������ ��
			bint_unsigned_sub((*tmp), x, y);
			// ������� ��ȣ�� x�� ��ȣ�� ����
			(*tmp)->sig = x->sig;
		}
		// y�� ������ x�� ���񰪺��� ū ���
		else if (xybig < 0)
		{
			// ������� ������ y,x�� ������ ��
			bint_unsigned_sub((*tmp), y, x);
			// ������� ��ȣ�� x�� ��ȣ�� �ݴ�
			(*tmp)->sig = -x->sig;
		}
		// x�� y�� ������ ���� ���
		else
		{
			// ������� 0
			makebint_zero((*tmp), 1);
		}
	}
	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}