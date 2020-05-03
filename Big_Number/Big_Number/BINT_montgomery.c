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
	BINT tmp,tmp1;
	// ���ѻ��� üũ
	assert(x->sig == POS_SIG); assert(y->sig == POS_SIG); assert(x->len > 0); assert(y->len > 0);
	// x * y ���
	bint_mul(tmp, x, y);
	// (x * y) / m ��� , �� : tmp , ������ : out
	bint_div(tmp1, out, tmp, m);
	// �ʿ���� �� �޸� ����
	delbint(tmp);
	delbint(tmp1);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : UWORD_inv
// ��  �� : a ^ -1 (mod b)
// ��  �� : UWORD a
// ���ϰ� : ����
// ���� ���� : a �� Ȧ��
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
	assert(m->dat[0] * mp == MASK_WW); 
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
		u->dat[0] = ((*tmp)->len > i ? (*tmp)->dat[i]:0) * mp;
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
	// if A >= m ,then A <- A - m
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
void bint_montgomery_multiplication(BINT out, const BINT m, const BINT x,const BINT y,const UWORD mp)
{
	SINT i;
	// m �� ����
	SINT n = m->len;
	// �߰��� ����
	BINT u,xi,tmpval,tmp2val;
	// ���� ���� üũ
	assert(m->dat[0] * mp == MASK_WW); 
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
		u->dat[0] = ((*tmp)->dat[0] + (x->len > i ? x->dat[i] : 0) * y->dat[0]) * mp;
		// xi <- x_i
		xi->dat[0] = (x->len > i ? x->dat[i] : 0);

		// tmpval <- m * u_i
		bint_mul(tmpval, m, u);
		// tmp2val <- y * x_i
		bint_mul(tmp2val, xi, y);
		// A <- A + m * u_i + y * x_i 
		bint_add((*tmp), (*tmp), tmpval);
		bint_add((*tmp), (*tmp), tmp2val);
		// A <- A / b
		bint_rightshift((*tmp), (*tmp), BITSZ_WW);

		// �߰� �޸� ����
		delbint(tmpval);
		delbint(tmp2val);
	}
	// if A >= m then A <- A - m 
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

void makeconst_montgomery()
{
	UWORD M_arr[33] = {0x616B52B7 ,0xBEC18FFD ,0x28D3166A ,0xEEE3B067 ,
                  0x0827ABBF ,0x1FE6BFFC ,0x279DECDD ,0x3B5FF0DA ,
                  0x7945EA76 ,0xB09FF817 ,0x3F4C599A ,0x57256C9A ,
                  0x377C1B96 ,0x07EFCC36 ,0x47E66E7F ,0x2D55CC39 ,
                  0xC7E2E48C ,0x7AD9F24A ,0x5B2E13FE ,0x6C47CA03 ,
                  0xCD72D203 ,0x631ED5DB ,0x18471F09 ,0xC0A6D4E4 ,
                  0x31CB4500 ,0x019D3848 ,0xFE4E80FB ,0xF95F7D71 ,
                  0xE36C0B5F ,0xB8DDCD1C ,0x0BEFDD5A ,0x9083F615 };
	MP_montgomery = 0x71cbd4f9;
	uwordarr2bint(M_montgomery, M_arr, 32, POS_SIG);
}

void bint_mul_reduction_classical(BINT out,const BINT m,const BINT x,const BINT y)
{
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x || out == y || out == m) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));
	BINT ttmp,tttmp;
	bint_mul(ttmp, x, y);
	bint_div(tttmp, (*tmp), ttmp, m);
	delbint(ttmp);
	delbint(tttmp);
	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

void bint_sqr_reduction_classical(BINT out, const BINT m, const BINT x)
{
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x || out == m) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));
	BINT ttmp;
	bint_sqr(ttmp, x);
	bint_div(ttmp, (*tmp), ttmp, m);
	delbint(ttmp);

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

//5
void bint_montmul_redmul(BINT out, const BINT m, const BINT x, const BINT y, const UWORD mp)
{
	BINT tmp;
	bint_mul(tmp, x, y);
	bint_montgomery_reduction(out, m, tmp, mp);
	delbint(tmp);
}
//6
void bint_montmul_redmulsqr(BINT out, const BINT m, const BINT x, const BINT y, const UWORD mp)
{
	BINT tmp;
	if (x == y) bint_sqr(tmp, x);
	else bint_mul(tmp, x, y);
	bint_montgomery_reduction(out, m, tmp, mp);
	delbint(tmp);
}
//7
void bint_montmul_montmul(BINT out, const BINT m, const BINT x, const BINT y, const UWORD mp)
{
	bint_montgomery_multiplication(out, m, x, y, mp);
}
