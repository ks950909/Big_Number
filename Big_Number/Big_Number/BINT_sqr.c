#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword_2mul
// ��  �� :
// d * b^2 + c * b + out <= out + 2*x*y + c + d * b , d < 2
// ��  �� : P_UWORD out, const UWORD x,const UWORD y , P_UWORD c, P_UWORD d
// ���ϰ� : ����
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
void uword_2mul(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c, P_UWORD d)
{
	// �߰��� ���� �ӽð���
	UWORD w;
	// x = x0 * sqr(b) + x1 , y = y0 * sqr(b) + y1
	// x0,x1,y0,y1 < sqrt(b)
	UWORD x0, x1, y0, y1;
	x0 = x >> BITSZ_HW;
	x1 = x & MASK_RHW;
	y0 = y >> BITSZ_HW;
	y1 = y & MASK_RHW;

	// d * b ^ 2 + c *b + out <- out + c + d * b
	// c < 2
	w = *c;
	*c = *d;
	*d = 0;
	*d += uword_add_atmul(out, w, c);

	// d * b ^ 2 + c * b + out <- out + x1 * y1 * 2
	*d += uword_leftshift_add(c, out, x1 * y1, 1);
	// d * b ^ 2 + c * b + out <- out + x1 * y0 * 2 * sqrt(b)
	*d += uword_leftshift_add(c, out, x1 * y0, 1 + BITSZ_HW);
	// d * b ^ 2 + c * b + out <- out + x0 * y1 * 2 * sqrt(b)
	*d += uword_leftshift_add(c, out, x0 * y1, 1 + BITSZ_HW);

	// d * b ^ 2 + c * b + out <- out + x0 * y0 * 2 * b
	uword_leftshift_add(d, c, x0 * y0, 1);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_sqr
// ��  �� : out = x * x
// ��  �� : BINT out, const BINT x
// ���ϰ� : ����
// ���� ���� : x->len > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_sqr(BINT out, const BINT x)
{
	// for index i,j
	SINT i, j;
	// x�� ����
	SINT n = x->len;
	// �ڸ��ø� c,d
	UWORD c, d;
	// ���ѻ��� üũ
	assert(x->len > 0);
	// Step 1: output �����Ҵ� �� �ʱ�ȭ (������� �ִ���� : 2n+1)
	makebint_zero(out, 2 * n + 1);
	// Step 2: For i from 0 to (n-1) do the following :
	for (i = 0; i < n; ++i)
	{
		// c,d �ʱ�ȭ
		c = 0; d = 0;
		// Step 2.1: (uv) <- w[2*i] + x[i]*x[i] ,w[2*i]<-v, c<- u, d<- 0
		uword_mul(&out->dat[i + i], x->dat[i], x->dat[i], &c);
		// Step 2.2: For j from i+1 to (n-1) do the following :
		for (j = i + 1; j < n; ++j)
		{
			// (quv) <- w[i+j] + 2*x[i]*x[j] + (dc) , w[i+j]<- v,c<-u,d<-q
			uword_2mul(&out->dat[i + j], x->dat[i], x->dat[j], &c, &d);
		}
		// w[i+n] <- w[i+n] + c
		uword_add_atmul(&out->dat[i + n], c, &out->dat[i + n + 1]);
		// w[i+n+1] <- d
		//if(i != n-1) out->dat[i + n + 1] += d;
		out->dat[i + n + 1] += d;
	}
	bint_unsigned_makelen(out);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_sqr
// ��  �� : out = x * x
// ��  �� : BINT out, const BINT x
// ���ϰ� : ����
// ���� ���� : x->len > 0
//
////////////////////////////////////////////////////////////////////////
void bint_sqr(BINT out, const BINT x)
{
	// out �� x�� ����Ű�� ���� ������ üũ
	SINT eq = (out == x) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x���� ��ȭ�� ����Ƿ� �����Ҵ�
	else tmp = (BINT*)malloc(sizeof(BINT));

	// ���ѻ��� üũ
	assert(x->len > 0); assert(tmp != NULL);
	// x == 0�� ���
	if (x->sig == 0)
	{
		// out = 0
		makebint_zero((*tmp), 1);
	}
	// x != 0�� ���
	else
	{
		// tmp = x * x
		bint_unsigned_sqr((*tmp), x);
		// ����� ��ȣ�� �׻� ���
		(*tmp)->sig = POS_SIG;
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