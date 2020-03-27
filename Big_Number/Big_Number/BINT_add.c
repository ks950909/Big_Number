#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_add
// ��  �� : �� uword�� ����
// c' * b + out = x+y+c , (c,c' < 2)
// x = x0 * (b/2) + x1 ( x0 < 2, x1 < (b/2))
// y = y0 * (b/2) + y1 ( y0 < 2, y1 < (b/2))
// ��  �� : P_UWORD out, const UWORD x, cosnt UWORD y , P_UWORD c ( �ش� ���� ��)
// ���ϰ� : ����
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
void uword_add(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	// (b/2) -1 ( �ֻ��� ��Ʈ ���� ������ ��� 1)
	const UWORD MASK_except_LMSB_ONE = MASK_WW ^ LMSB_ONE;
	// x1+y1+c �� out�� ����
	// x1 + y1 + c <= (b/2 - 1) + (b/2 - 1) + 1 = (b - 1)  ( out�� ���尡��)
	*out = (x & MASK_except_LMSB_ONE) + (y & MASK_except_LMSB_ONE) + *c;
	// c'�� ������ ����µ� x0 + y0 + (x1+y1+c)�� �ֻ��� ��Ʈ�� ����
	// c'�� (x+y+c)>> BITSZ_WW_1 �� ����
	*c = ((x & LMSB_ONE) ? 1u : 0u) + ((y & LMSB_ONE) ? 1u : 0u) + ((*out & LMSB_ONE) ? 1u : 0u);
	// out�� x1+y1+c �� ���� BITSZ_WW_1��Ʈ ����
	*out &= MASK_except_LMSB_ONE;
	// out�� �ֻ��� ��Ʈ�� c'�� �̿��Ͽ� ����
	*out ^= ((*c & 0x1) ? LMSB_ONE : 0u);
	// c' �� ���� (x+y+c)>> BITSZ_WW�̹Ƿ� 1ĭ �� �о���
	// c' <= 3�̹Ƿ� ���ο� c'<= 1
	*c >>= 1;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_add_new
// ��  �� : �� uword�� ����(new version)
// c' * 2^32 + out = x+y+c , (c,c' < 2)
// ��  �� : P_UWORD out, const UWORD x, cosnt UWORD y , P_UWORD c ( �ش� ���� ��)
// ���ϰ� : ����
// ���� ���� : ����
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
// �Լ��� : bint_unsigned_add
// ��  �� : �� BINT�� ���� ����
// out = |x| + |y|
// ��  �� : BINT out, const BINT x, const BINT y (�ش� ���� ��)
// ���ϰ� : ����
// ���� ���� : x,y�� ���� > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_add(BINT out, const BINT x, const BINT y)
{
	// �ڸ��ø� c
	UWORD c;
	// for index i , out�� �ִ���� n
	SINT i, n;
	// ���ѻ��� üũ
	assert(x->len > 0); assert(y->len > 0);
	// �ִ� ���� ���
	n = (x->len > y->len ? x->len : y->len) + 1;
	// ���� �Ҵ�
	makebint(out, n);
	// STEP 1 : c��0 (c is the carry digit).
	c = 0;
	// STEP 2 :  For i from 0 to n - 1 do the following:
	for (i = 0; i < n; ++i)
	{
		// STEP 2.1 : (c'* b + w[i]) <- x[i] + y[i] + c , c' <- c
		// ���� �κ��� �������ϰ� 0���� ( 0 padding on the high-order positions)
		//uword_add_new(&(out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), &c);
		uword_add_new2((out->dat[i]), (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), c);
		//c = _addcarry_u32(c, (x->len > i ? x->dat[i] : 0u), (y->len > i ? y->dat[i] : 0u), &(out->dat[i]));
	}
	// STEP 3 : �ʿ���� 0����(�ֻ��� �ڸ�)
	bint_unsigned_makelen(out);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_add
// ��  �� : �� BINT�� ����
// out = x + y
// ��  �� : BINT out, const BINT x, const BINT y (�ش� ���� ��)
// ���ϰ� : ����
// ���� ���� : x,y�� ���� > 0
//
////////////////////////////////////////////////////////////////////////
void bint_add(BINT out, const BINT x, const BINT y)
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
	// x�� y�� ��ȣ�� ���� ���
	else if (x->sig == y->sig)
	{
		// ������� ������ x,y�� ������ ��
		//bint_unsigned_add(tmp, x, y);
		bint_unsigned_add(*tmp, x, y);
		// ������� ��ȣ�� x�� ��ȣ�� ����
		(*tmp)->sig = x->sig;
	}
	// x�� y�� ��ȣ�� �ٸ� ���
	else
	{
		// x�� y�� ������ ũ���
		SINT xybig = bint_unsigned_compare(x, y);
		// x�� ������ y���� ū ���
		if (xybig > 0)
		{
			// ������� ������ x,y�� ������ ��
			bint_unsigned_sub(*tmp, x, y);
			// ������� ��ȣ�� x�� ��ȣ�� ����
			(*tmp)->sig = x->sig;
		}
		// y�� ������ x���� ū ���
		else if (xybig < 0)
		{
			// ������� ������ y,x�� ������ ��
			bint_unsigned_sub(*tmp, y, x);
			// ������� ��ȣ�� y�� ��ȣ�� ����
			(*tmp)->sig = y->sig;
		}
		// x�� y�� ������ ���� ���
		else
		{
			// ������� 0
			makebint_zero(*tmp, 1);
		}
	}
	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, *tmp);
		delbint(*tmp);
		free(tmp);
	}
}