#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_classical_modular_multiplication
// ��  �� : out = (x * y) % m
// ��  �� : BINT out1,BINT out2, const BINT x, const BINT y
// ���ϰ� : ����
// ���� ���� : x,y ���� > 0, x,y > 0
//
////////////////////////////////////////////////////////////////////////
void bint_classical_modular_multiplication(BINT out, const BINT m, const BINT x, const BINT y)
{
	// ���� �����ϱ� ���� ����
	BINT tmp;
	// ���ѻ��� üũ
	assert(x->sig == POS_SIG); assert(y->sig == POS_SIG); assert(x->len > 0); assert(y->len > 0);
	// x * y ���
	bint_mul(out, x, y);
	// (x * y) / m ��� , �� : tmp , ������ : out
	bint_div(tmp, out, out, m);
	// �ʿ���� �� �޸� ����
	delbint(tmp);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : UWORD_inv
// ��  �� : a ^ -1 (mod b)
// ��  �� : UWORD a
// ���ϰ� : ����
// ���� ���� : x,y ���� > 0, x,y > 0
//
////////////////////////////////////////////////////////////////////////
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
			q |= ttmp; // <=> q += ttmp;
			r += tmp;
		}
	}
	return q;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_montgomery_reduction
// ��  �� : TR^-1 mod m ( R = b^n)
// ��  �� : const BINT m,T , const UWORD mp ( - m^-1 (mod b) )
// ���ϰ� : BINT out
// ���� ���� : m * minv == -1 (mod b) , m�� ���� * 2= T�Ǳ���
//
////////////////////////////////////////////////////////////////////////
void bint_montgomery_reduction(BINT out, const BINT m, const BINT T,const UWORD mp)
{
	SINT i;
	SINT n = m->len;
	// �߰� �� ����
	BINT u,tmpval;
	// ���� ���� üũ
	assert(m->dat[0] * mp == MASK_WW); assert(m->len * 2 == T->len);
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == m || out == T) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	// A <- T
	bint_copy((*tmp), T);
	// u_i �� ���� �����
	uword2bint(u, 1u);

	for (i = 0; i < n; i++)
	{
		// u_i <- a_i * m' mod b
		u->dat[0] = (*tmp)->dat[i] * mp;
		// tmpval <- u_i * m
		bint_mul(tmpval, m, u);
		// tmpval <- u_i * m * b^i
		bint_leftshift(tmpval, tmpval, BITSZ_WW * i);
		// A  <- A + u_i * m * b^i
		bint_add((*tmp), (*tmp), tmpval);
		// �߰� �޸� ����
		delbint(tmpval);
	}
	// A <- A / b^n
	bint_rightshift((*tmp), (*tmp), BITSZ_WW * n);
	// A >= m -> A <- A - m
	if (bint_compare((*tmp), m) >= 0) bint_sub((*tmp), (*tmp), m);
	// A �������� ���ϱ�
	bint_unsigned_makelen(*tmp);
	// ����� �޸� ����
	delbint(u);
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
// �Լ��� : bint_montgomery_multiplication
// ��  �� : xyR^-1 mod m ( R = b^n)
// ��  �� : const BINT m,x,y , const UWORD minv ( - m^-1 (mod b) )
// ���ϰ� : BINT out
// ���� ���� : m * minv == -1 (mod b) , m�� ���� = x�Ǳ��� = y�Ǳ���
//
////////////////////////////////////////////////////////////////////////
void bint_montgomery_multiplication(BINT out, const BINT m, const BINT x,const BINT y,const UWORD minv)
{
	SINT i;
	// m �� ����
	SINT n = m->len;
	// �߰��� ����
	BINT u,xi,tmpval,tmp2val;
	// ���� ���� üũ
	assert(m->dat[0] * minv == MASK_WW); assert(m->len == x->len);assert(m->len == y->len);
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x || out == y || out == m) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	// A <- 0
	makebint_zero((*tmp),n);
	// u,xi ���ڸ��� ���
	uword2bint(u, 1u); uword2bint(xi, 1u);

	for (i = 0; i < n; i++)
	{
		// u_i <- (a_0 + x_i * y_0) * m' (mod b)
		u->dat[0] = ((*tmp)->dat[0] + x->dat[i] * y->dat[0]) * minv;
		// xi <- x_i
		xi->dat[0] = x->dat[i];
		// tmpval <- m * u_i
		bint_mul(tmpval, m, u);
		// tmp2val <- y * x_i
		bint_mul(tmp2val, xi, y);
		// A <- A + tmpval + tmp2val
		bint_add((*tmp), (*tmp), tmpval);
		bint_add((*tmp), (*tmp), tmp2val);
		// A <- A / b
		bint_rightshift((*tmp), (*tmp), BITSZ_WW);
		// �߰� �޸� ����
		delbint(tmpval);
		delbint(tmp2val);
	}
	// A �������� ���ϱ�
	bint_unsigned_makelen(*tmp);
	// A >= m -> A <- A - m 
	if (bint_compare((*tmp), m) >= 0) bint_sub((*tmp), (*tmp), m);
	// ����� �޸� ����
	delbint(u);
	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}
