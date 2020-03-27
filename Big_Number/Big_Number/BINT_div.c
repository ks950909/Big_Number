#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : make_leftshift_val
// ��  �� : make new x,y(normalization) and return how many shift
// ��  �� : UWORD num(y�� �ֻ��� ����) , const BINT x, const BINT y,BINt nx, BINT ny
// ���ϰ� : SINT (# of leftshift)
// ���� ���� : x->len > 0 , y->len > 0
//
////////////////////////////////////////////////////////////////////////
SINT make_leftshift_val(UWORD num, const BINT x, const BINT y, BINT nx, BINT ny)
{
	// �����
	SINT leftshift_val = 0;
	// ���ѻ��� üũ
	assert(x->len > 0); assert(y->len > 0);
	// y�� �ֻ��� ���ڸ� LMSB_ONE�̻����� ������
	while (num < LMSB_ONE)
	{
		num <<= 1;
		// �׶� �ʿ��� leftshift������ ����
		leftshift_val++;
	}
	// �Ʊ� ���� leftshift ��ŭ x�� leftshift���� nx�� ����
	bint_leftshift(nx, x, leftshift_val);
	// �Ʊ� ���� leftshift ��ŭ y�� leftshift���� ny�� ����
	bint_leftshift(ny, y, leftshift_val);

	// ����� ����
	return leftshift_val;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bisection
// ��  �� : (x[i]*b^2 + x[i-1]*b+x[i-2])/(y[t]*b+y[t-1]) �� ã�� �Լ�
// ��  �� : const BINT x, const BINT y  (x/y)
// ���ϰ� : UWORD �����
// ���� ���� : bint_unsigned_div_step3_1������ ����
//
////////////////////////////////////////////////////////////////////////
UWORD bisection(const BINT x, const BINT y)
{
	BINT out, tmp;
	// �ּ� �ִ� ����
	UWORD lo = 0, hi = MASK_WW;
	UWORD output;
	SINT compare_tmp;
	// out�� ������ 1�Ҵ�
	makebint(out, 1); out->sig = POS_SIG;
	// �̺�Ž��
	while (lo + 1 < hi)
	{
		// out->dat[0] = (lo + hi) / 2
		out->dat[0] = (lo + hi) >> 1;
		if (out->dat[0] < lo) out->dat[0] |= LMSB_ONE;

		// tmp = out * y �� �� tmp �� x�� ũ�� ��
		bint_mul(tmp, out, y);
		compare_tmp = bint_unsigned_compare(x, tmp);
		delbint(tmp);
		if (compare_tmp >= 0) lo = out->dat[0];
		else hi = out->dat[0];
	}
	out->dat[0] = lo + 3u > 2u ? lo + 3u : MASK_WW;
	bint_mul(tmp, out, y);
	while (TRUE)
	{
		compare_tmp = bint_unsigned_compare(x, tmp);
		// tmp > x �� ���
		if (compare_tmp < 0)
		{
			out->dat[0]--;
			bint_sub(tmp, tmp, y);
		}
		else break;
	}
	output = out->dat[0];

	delbint(tmp);
	delbint(out);
	return output;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_div_step2
// ��  �� :
// ��  �� : BINT out1,BINT newx, const BINT newy , const SINT nmt
// ���ϰ� : ����
// ���� ���� : bint_unsigned_div������ ����
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div_step2(BINT out1, BINT newx, const BINT newy, const SINT nmt)
{
	BINT tmp;
	SINT compare_tmp;
	// tmp = y*b^(n-t)�����
	bint_leftshift(tmp, newy, BITSZ_WW * (nmt));
	// �� �� ���� �� ����
	compare_tmp = bint_unsigned_compare(newx, tmp);
	// newx >= tmp �� ���
	if (compare_tmp >= 0)
	{
		// q[n-t] += 1
		out1->dat[nmt]++;
		// x -= y*b^(n-t)
		bint_sub(newx, newx, tmp);
	}
	// ����� �޸� ����
	delbint(tmp);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_div_step3_1
// ��  �� :
// ��  �� : const BINT newx,const BINT newy, BINT out1, BINT tmp,BINT yt, CONST SINT idx,CONST SINT t
// ���ϰ� : ����
// ���� ���� : bint_unsigned_div������ ����
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div_step3_1(const BINT newx, const BINT newy, BINT out1, BINT tmp, const BINT yt, const SINT idx, const SINT t)
{
	// tmp = x[i]*b^2 +x[i-1]*b+x[i-2]
	tmp->dat[0] = idx == 1 ? 0u : newx->dat[idx - 2];
	tmp->dat[1] = newx->dat[idx - 1];
	tmp->dat[2] = newx->dat[idx];
	// q[i-t-1] = tmp / yt
	out1->dat[idx - t - 1] = bisection(tmp, yt);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_div_step3_3
// ��  �� :
// ��  �� : BINT newx, const BINT newy, BINt out1, const SINT idx, const SINT t
// ���ϰ� : ����
// ���� ���� : bint_unsigned_div_step3_3������ ����
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div_step3_3(BINT newx, const BINT newy, BINT out1, const SINT idx, const SINT t)
{
	BINT tmpy1, tmpy2, tmpy3;
	SINT compare_tmp;
	// tmpy1 = y * b^(i-t-1)
	bint_leftshift(tmpy1, newy, BITSZ_WW * (idx - t - 1));

	// tmpy3 = q[i-t-1]
	uword2bint(tmpy3, out1->dat[idx - t - 1]);

	// tmpy2 = q[i-t-1] * y * b^(i-t-1)
	bint_mul(tmpy2, tmpy1, tmpy3);

	// x�� tmpy2��
	compare_tmp = bint_unsigned_compare(newx, tmpy2);
	// x < tmpy2
	if (compare_tmp < 0)
	{
		// q[i-t-1]-=1 , tmpy2 �ǹ̿� �°� tmpy1 ����
		out1->dat[idx - t - 1]--;
		bint_sub(tmpy2, tmpy2, tmpy1);
	}

	// x -= tmpy2
	bint_sub(newx, newx, tmpy2);

	// �޸� ����
	delbint(tmpy1);
	delbint(tmpy2);
	delbint(tmpy3);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_div_makesubarr
// ��  �� : out1�� ó�� �Ҵ� �� ���α׷� ������ �Ҵ� ����,tmp, yt�� �������̷� ��� �����̹Ƿ� �̸� �޸� �Ҵ�
// ��  �� : BINT out1, BINt tmp, BINT yt,BINt tmpy3, const BINt newy,const SINT n cosnt SINT t
// ���ϰ� : ����
// ���� ���� : bint_unsigned_div������ ����
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div_makesubarr(BINT out1, BINT tmp, BINT yt, const BINT newy, const SINT n, const SINT t)
{
	// out1 : ���� n-t+1, 0���� �ʱ�ȭ
	makebint_zero(out1, n - t + 1);
	// ������ ����� ����ϹǷ� �̸� POS_SIG ��
	out1->sig = POS_SIG;

	// tmp : ���� 3¥�� x[i]*b^2 + x[i-1]*b + x[i-2]�� ��� , ���ķ� ���� ��� ���ϹǷ� ���Ŀ� ����
	makebint(tmp, 3);
	tmp->sig = POS_SIG;
	// yt : ���� 2¥�� y[t] * b + y[t-1]�� ��� , ���ķ� ���� ������ �����Ƿ� ���� ���� ( -1�� �ε����� �������� ��� -1����)
	makebint(yt, 2);
	yt->dat[0] = t == 0 ? 0u : newy->dat[t - 1];
	yt->dat[1] = newy->dat[t];
	yt->sig = POS_SIG;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_div_free
// ��  �� : �� BINT �޸� ����
// ��  �� : BINT newx, BINT newy, BInt tmp, BINT yt
// ���ϰ� : ����
// ���� ���� : bint_unsigned_div������ ����
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div_free(BINT newx, BINT newy, BINT tmp, BINT yt)
{
	delbint(newx);
	delbint(newy);
	delbint(tmp);
	delbint(yt);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_div
// ��  �� : |x| = |y| * out1 + out2 , |y| > |out2|
// ��  �� : BINT out1,BINT out2, const BINT x, const BINT y
// ���ϰ� : ����
// ���� ���� : x,y ���� > 0,
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div(BINT out1, BINT out2, const BINT x, const BINT y)
{
	// ����
	SINT n, t;
	// for index i
	SINT i;
	// normalize ��� �����
	SINT leftshift_val;
	// �μ��� ��Һ� ��� �����
	SINT compare_tmp;
	// �߰� ���� ���
	BINT newx, newy, tmp, yt;
	// y�� �ֻ��� ����
	UWORD ytmp = y->dat[y->len - 1];

	// ���ѻ��� üũ
	assert(x->len > 0); assert(y->len > 0);

	// |y| �� |x|�� ũ���
	compare_tmp = bint_unsigned_compare(x, y);
	// |y| > |x| ��� out1 = 0 , out2 = x ������ ����
	if (compare_tmp < 0)
	{
		makebint_zero(out1, 1);
		bint_copy(out2, x);
		return;
	}

	// normalization
	leftshift_val = make_leftshift_val(ytmp, x, y, newx, newy);

	// n,t ����
	n = newx->len - 1; t = newy->len - 1;

	// ���� �Ҵ�
	bint_unsigned_div_makesubarr(out1, tmp, yt, newy, n, t);

	// STEP 2 :
	bint_unsigned_div_step2(out1, newx, newy, n - t);

	// STEP 3 :
	for (i = n; i > t; --i)
	{
		// STEP 3.1 , 3.2
		bint_unsigned_div_step3_1(newx, newy, out1, tmp, yt, i, t);

		// STEP 3.3 , 3.4
		bint_unsigned_div_step3_3(newx, newy, out1, i, t);
	}

	// normalization
	bint_rightshift(out2, newx, leftshift_val);

	// make len
	bint_unsigned_makelen(out1);
	bint_unsigned_makelen(out2);

	// �޸� ����
	bint_unsigned_div_free(newx, newy, tmp, yt);
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_div
// ��  �� :
// x = out1 * y + out2 ( out2 < |y|)
// ��  �� : BINT out1,BINT out2, const BINT x, const BINT y
// ���ϰ� : ����
// ���� ���� : x->len > 0 , y->len > 0, out1 != out2
//
////////////////////////////////////////////////////////////////////////
void bint_div(BINT out1, BINT out2, const BINT x, const BINT y)
{
	// out1 �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq1 = (out1 == x || out1 == y) ? TRUE : FALSE;
	// out2 �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq2 = (out2 == x || out2 == y) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp1, * tmp2;
	// 1�����
	BINT one;

	// out1���� �ٲپ x,y���� ��ȭ�� ����
	if (eq1 == FALSE) tmp1 = out1;
	// out1���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else tmp1 = (BINT*)malloc(sizeof(BINT));
	// out2���� �ٲپ x,y���� ��ȭ�� ����
	if (eq2 == FALSE) tmp2 = out2;
	// out2���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else tmp2 = (BINT*)malloc(sizeof(BINT));

	// ���� ���� üũ
	assert(x->len > 0); assert(y->len > 0); assert(out1 != out2); assert(tmp1 != NULL); assert(tmp2 != NULL);

	// y = 0 , ��ǻ� ����
	if (y->sig == ZERO_SIG)
	{
		makebint_zero((*tmp1), 1);
		bint_copy((*tmp2), x);
	}
	// x = 0 , ��,������ ��� 0
	else if (x->sig == ZERO_SIG)
	{
		makebint_zero((*tmp1), 1);
		makebint_zero((*tmp2), 1);
	}
	// x > 0
	else if (x->sig == POS_SIG)
	{
		bint_unsigned_div((*tmp1), (*tmp2), x, y);
		// �������� �׻� r �� ����
		// ��
		// y > 0 : q
		// y < 0 : -q
		if (x->sig != y->sig && (*tmp1)->sig == POS_SIG) (*tmp1)->sig = NEG_SIG;
	}
	// x < 0
	else
	{
		bint_unsigned_div((*tmp1), (*tmp2), x, y);
		if ((*tmp2)->sig != 0)
		{
			// ������
			// y > 0 : y - r , y < 0 : -y - r
			// make -r
			(*tmp2)->sig = NEG_SIG;
			// y > 0 : +y
			if (y->sig == POS_SIG) bint_add((*tmp2), (*tmp2), y);
			// y < 0 : -y
			else bint_sub((*tmp2), (*tmp2), y);
			// ��
			// y > 0 : -1 - q , y < 0 : q + 1
			// q�� ���� ����
			if (x->sig != y->sig && (*tmp1)->sig == POS_SIG) (*tmp1)->sig = NEG_SIG;
			// 1�� �����
			uword2bint(one, 1u);
			// y > 0 : -1 - q
			if (y->sig == POS_SIG)
				bint_sub((*tmp1), (*tmp1), one);
			// y < 0 : q + 1
			else
				bint_add((*tmp1), (*tmp1), one);
			// ����� one �޸� ����
			delbint(one);
		}
	}

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq1 == TRUE)
	{
		delbint(out1);
		bint_copy(out1, (*tmp1));
		delbint((*tmp1));
		free(tmp1);
	}
	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq2 == TRUE)
	{
		delbint(out2);
		bint_copy(out2, (*tmp2));
		delbint((*tmp2));
		free(tmp2);
	}
}