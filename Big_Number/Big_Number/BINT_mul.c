#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_add_atmul
// ��  �� :
// c * b + out <- c * b + (x+out)
// ��  �� : P_UWORD out, const UWORD x , P_UWORD c
// ���ϰ� : �ڸ��ø� �Ǿ��°�(b^2) ( true : 1 , false : 0
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
UWORD uword_add_atmul(P_UWORD out, const UWORD x, P_UWORD c)
{
	// �ڸ��ø��� ���� �ӽ�����
	UWORD tmp = *out;
	// out�� x�� ����
	*out += x;
	// ���� �ڸ��ø�(b)�� �Ͼ�ٸ� *out�� tmp ���� �۾���
	*c += ((*out) >= tmp) ? 0u : 1u;
	// *c�� 0�̶�� �ڸ��ø� �Ͼ(b^2)
	return ((*c == 0u && (*out < tmp)) ? 1u : 0u);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_mul
// ��  �� :
// c * b + out <- out + x*y + c
// ��  �� : P_UWORD out, const UWORD x , const UWORD y, P_UWORD c
// ���ϰ� : ����
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
void uword_mul(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c)
{
	// �߰��� �����ӽð���
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
// �Լ��� : bint_unsigned_mul
// ��  �� : �� BINt�� ���� ��
// out = |x|*|y|
// ��  �� : BINT out, const BINT x,const BINT y
// ���ϰ� : ����
// ���� ���� : x,y�� ���� > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_mul(BINT out, const BINT x, const BINT y)
{
	// for index i,j
	SINT i, j;
	// n ,t �� x,y�� ����
	SINT n = x->len, t = y->len;
	// �ڸ��ø� c
	UWORD c;
	// ���ѻ��� üũ
	assert(n > 0); assert(t > 0);
	// Step 1: output �����Ҵ� �� �ʱ�ȭ (������� �ִ���� : n+t)
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
	// STEP 3. �ֻ��� �ڸ� 0����
	bint_unsigned_makelen(out);
}

//c = _mulx_u32(x->dat[j], y->dat[i], &out->dat[i + j]); // c*b+(out->dat[i+j])=x[j]*y[i]
////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_mul
// ��  �� : �� BINT �� ��
// out = x*y
// ��  �� : BINT out, const BINT x,const BINT y
// ���ϰ� : ����
// ���� ���� : x,y�� ���� > 0
//
////////////////////////////////////////////////////////////////////////
void bint_mul(BINT out, const BINT x, const BINT y)
{
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x || out == y) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	// ���ѻ��� üũ
	assert(x->len > 0); assert(y->len > 0); assert(tmp != NULL);
	// x == 0 or y == 0
	if (x->sig == ZERO_SIG || y->sig == ZERO_SIG)
	{
		// �� ���� �ϳ��� 0�̶�� 0
		makebint_zero((*tmp), 1);
	}
	else
	{
		// �� ������ ���ؼ� tmp�� ����
		bint_unsigned_mul((*tmp), x, y);
		// ������� ��ȣ�� �� ��ȣ�� ������ ���, �ٸ��� ����
		(*tmp)->sig = (x->sig == y->sig ? POS_SIG : NEG_SIG);
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