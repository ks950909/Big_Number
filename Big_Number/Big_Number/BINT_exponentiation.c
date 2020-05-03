#include "BINT.h"


void bint_rtol_exponentiation(BINT out, const BINT g,const BINT e,const BINT m,void (*mul)(BINT ,const BINT ,const BINT,const BINT),void (*sqr)(BINT,const BINT,const BINT))
{
	BINT S,tmpe;
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == g || out == e) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	if (mul == NULL) mul = bint_mul_reduction_classical;
	if (sqr == NULL) sqr = bint_sqr_reduction_classical;

	// Step 1. A �� 1 , S �� g , e ����
	uword2bint((*tmp), 0x1);
	bint_copy(S, g);
	bint_copy(tmpe, e);

	// Step 2.
	while (tmpe->sig == POS_SIG)
	{
		// Step 2.1 A �� A*S
		if (tmpe->dat[0] & 1) mul((*tmp),m,(*tmp),S);
		// e = e / 2
		bint_rightshift(tmpe, tmpe, 1);
		// S �� S*S
		if (tmpe->sig == POS_SIG) sqr(S, m,S);
	}
	
	delbint(S);
	delbint(tmpe);
	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

void bint_ltor_exponentiation(BINT out, const BINT g,const BINT e,const BINT m,void (*mul)(BINT ,const BINT ,const BINT,const BINT),void (*sqr)(BINT,const BINT,const BINT))
{
	SINT t = e->len * BITSZ_WW - 1;
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == g || out == e || out == m) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	if (mul == NULL) mul = bint_mul_reduction_classical;
	if (sqr == NULL) sqr = bint_sqr_reduction_classical;

	// set t
	while (bint_seebit(e,t) == 0)
	{
		t--;
	}

	// Step 1. A �� 1 
	uword2bint((*tmp), 0x1);

	// Step 2.
	for(SINT i = t ; i >= 0 ; i--)
	{
		// Step 2.1 A �� A * A
		sqr((*tmp),m, (*tmp));
		// Step 2.2 if e_i =1 :A �� A * g
		if (bint_seebit(e,i) == 1) mul((*tmp),m, (*tmp), g);
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

//void bint_ltor_kary_exponentiation(BINT out,const BINT g,const BINT e,const SINT k)
void bint_ltor_kary_exponentiation(BINT out, const BINT g,const BINT e,const BINT m,const SINT k,void (*mul)(BINT ,const BINT ,const BINT,const BINT),void (*sqr)(BINT,const BINT,const BINT))
{
	SINT t = 0;
	SINT tkm1 = (1 << k) - 1;
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == g) ? TRUE : FALSE;
	BINT* saveg;
	BINT tmpe;
	UWORD* savee;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	// ��������
	assert(e->len > 0); assert(e->sig == POS_SIG);

	if (mul == NULL) mul = bint_mul_reduction_classical;
	if (sqr == NULL) sqr = bint_sqr_reduction_classical;

	// make savee
	bint_copy(tmpe, e);

	// e_i ������� �����
	savee = (UWORD*)malloc(((e->len * BITSZ_WW / k) + 1) *sizeof(UWORD));

	// t �� e_i �����
	while (tmpe->sig == POS_SIG)
	{
		savee[t++] = (tmpe->dat[0] & tkm1);
		bint_rightshift(tmpe, tmpe, k);
	}

	// Step 1. precomputation

	// g^0 ~ g^(b-1) ������� �����
	saveg = (BINT*)malloc((1<<k)*sizeof(BINT));

	// Step 1.1 g_0 �� 1
	uword2bint(saveg[0], 0x1);

	for (int i = 1; i <= tkm1; i++)
	{
		// Step 1.2 g_i �� g_(i-1) * g
		mul(saveg[i],m, saveg[i - 1], g);
	}

	// Step 2 A �� 1
	uword2bint((*tmp), 0x1);

	// Step 3
	for(int i = t-1 ; i >= 0 ; i--)
	{
		// Step 3.1 A �� A ^ (2^k)
		for (int j = 0; j < k; j++) sqr((*tmp), m, (*tmp));
		// Step 3.2 A �� A * g_e_i
		mul((*tmp),m, (*tmp), saveg[savee[i]]);
	}

	delbint(tmpe);
	free(savee);
	for (int i = 0; i <= tkm1; i++) delbint(saveg[i]);
	free(saveg);

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

//void bint_ltor_kary_exponentiation(BINT out,const BINT g,const BINT e,const SINT k)
void bint_ltor_kary_modify_exponentiation(BINT out, const BINT g,const BINT e,const BINT m,const SINT k,void (*mul)(BINT ,const BINT ,const BINT,const BINT),void (*sqr)(BINT,const BINT,const BINT))
{
	SINT t=0;
	SINT tkm1 = (1 << k) - 1;
	SINT tkmm1 = (1 << (k-1)) - 1;
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == g) ? TRUE : FALSE;
	BINT* saveg;
	BINT tmpe;
	UWORD* saveh,*saveu;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	if (mul == NULL) mul = bint_mul_reduction_classical;
	if (sqr == NULL) sqr = bint_sqr_reduction_classical;

	// make savee
	bint_copy(tmpe, e);
	// h,u�� ������ ���� ������
	saveh = (UWORD*)malloc((((e->len * BITSZ_WW) / k) + BITSZ_WW) *sizeof(UWORD));
	saveu = (UWORD*)malloc((((e->len * BITSZ_WW) / k) + BITSZ_WW) *sizeof(UWORD));

	// h,u,t�� ����
	while (tmpe->sig == POS_SIG)
	{
		// u�� e�� ���� ��
		saveu[t] = (tmpe->dat[0] & tkm1);
		// h �ʱ�ȭ
		saveh[t] = 0;
		// u �� 2�� ������ h�� 1���� ( u �� Ȧ���� ���ö� ����)
		while (saveu[t] != 0 && ((saveu[t] & 1) == 0))
		{
			saveu[t] >>= 1;
			saveh[t]++;
		}
		t++;
		bint_rightshift(tmpe, tmpe, k);
	}

	// Step 1. precomputation

	// g^0 ~ g^(b-1) ������� �����
	saveg = (BINT*)malloc((1<<k)*sizeof(BINT));

	// Step 1.1
	// g^0 = 1
	uword2bint(saveg[0], 0x1);
	// g^1 = g
	mul(saveg[1], m, saveg[0],g);
	// g^2 = g * g (k �� 1�϶� ����ϸ� �ȵ�)
	if(k != 1)mul(saveg[2], m, saveg[1],g);

	// Step 1.2
	for (int i = 3; i <= tkm1; i+=2)
	{
		// g_(i) �� g_(i-2) * g_2 , �� i �� Ȧ��������
		mul(saveg[i],m, saveg[i - 2], saveg[2]);
	}

	// Step 2 A �� 1
	uword2bint((*tmp), 0x1);

	// Step 3
	for(int i = t-1 ; i >= 0 ; i--)
	{
		// A �� A ^ (2 ^ (k-h) )
		for (int j = saveh[i]; j < k; j++) sqr((*tmp), m, (*tmp));
		// A �� A * g_u
		mul((*tmp),m, (*tmp), saveg[saveu[i]]);
		// A �� A ^ (2 ^ h)
		for (int j = 0; j < saveh[i]; j++)sqr((*tmp), m, (*tmp));
	}

	delbint(tmpe);
	free(saveu);
	free(saveh);
	delbint(saveg[0]);
	if(k != 1)delbint(saveg[2]);
	for (int i = 1; i <= tkm1; i+=2) delbint(saveg[i]);
	free(saveg);

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}

void bint_slidingwindow_exponentiation(BINT out, const BINT g,const BINT e,const BINT m,const SINT k,void (*mul)(BINT ,const BINT ,const BINT,const BINT),void (*sqr)(BINT,const BINT,const BINT))
{
	SINT t = e->len * BITSZ_WW - 1;
	SINT tkm1 = (1 << k) - 1;
	SINT tkmm1 = (1 << (k-1)) - 1;
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == g) ? TRUE : FALSE;
	BINT* saveg;
	SINT idx;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));

	if (mul == NULL) mul = bint_mul_reduction_classical;
	if (sqr == NULL) sqr = bint_sqr_reduction_classical;


	// set t
	while (bint_seebit(e,t) == 0)
	{
		t--;
	}

	// Step 1. precomputation

	// g^0 ~ g^(b-1) ������� �����
	saveg = (BINT*)malloc((1<<k)*sizeof(BINT));

	// Step 1.1 g_1 �� g , g_2 �� g^2
	uword2bint(saveg[0], 0x1);
	mul(saveg[1], m, saveg[0],g);
	// k = 1 �϶��� �����ϸ� �ȵ�
	if(k!= 1)mul(saveg[2], m, saveg[1],g);

	// Step 1.2 
	for (int i = 3; i <= tkm1; i+=2)
	{
		// g_(i) �� g_(i-2) * g_2 , �� i �� Ȧ��������
		mul(saveg[i],m, saveg[i - 2], saveg[2]);
	}

	// Step 2. A �� 1 , i �� t
	uword2bint((*tmp), 0x1);
	idx = t;

	// Step 3.
	while (idx >= 0)
	{
		//Step 3.1 e_i = 0 : A �� A^2 , i �� i-1
		if (bint_seebit(e,idx) == 0)
		{
			sqr((*tmp), m, (*tmp));
			idx--;
		}
		// Step 3.2 Otherwise
		else
		{
			// estring = e_i ... e_l
			UWORD estring = 0;
			UWORD twoexp = 1;
			UWORD saveidx = -1;
			// l�� i-k+1 �̻�
			for (int i = max(0,idx - k + 1); i <= idx; i++)
			{
				// make estring
				if (estring != 0)
				{
					estring += (bint_seebit(e, i) << twoexp);
					twoexp++;
				}
				// find l (�� �� ����) (e_idx = 1)
				else if (bint_seebit(e, i) == 1)
				{
					// l ã�����Ƿ� A �� A ^ (i-l+1)
					for (int j = i; j <= idx; j++) sqr((*tmp), m, (*tmp));
					// i �� l-1
					saveidx = i - 1;
					// estring = 1 (e_l = 1)
					estring = 1;
				}
			}
			// A �� A * g_estring
			mul((*tmp), m, (*tmp), saveg[estring]);
			// �����ص� l-1 ������ �ٲ�
			idx = saveidx;
		}
	}

	delbint(saveg[0]);
	if(k!=1)delbint(saveg[2]);
	for (int i = 1; i <= tkm1; i+=2) delbint(saveg[i]);
	free(saveg);

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}



void bint_montogomery_exponentiation(BINT out, const BINT g, const BINT e, const BINT m, const UWORD mp,const BINT rm,const BINT r2m, void (*montmul)(BINT ,const BINT ,const BINT,const BINT,const UWORD))
{
	BINT xb;
	// out �� x,y�� ����Ű�� ���� ������ üũ
	SINT eq = (out == g) ? TRUE : FALSE;
	// �ּҰ��� �������� ���
	BINT* tmp;
	// out���� �ٲپ x,y���� ��ȭ�� ����
	if (eq == FALSE) tmp = out;
	// out���� �ٲٸ� x,y���� ��ȭ�� ����Ƿ� �����Ҵ�
	else  tmp = (BINT*)malloc(sizeof(BINT));
	SINT t = e->len * BITSZ_WW - 1;
	BINT one;
	// make one
	uword2bint(one, 0x1);

	// set t
	while (bint_seebit(e,t) == 0)
	{
		t--;
	}
	if (montmul == NULL) montmul = bint_montgomery_multiplication;

	// Step 1. x bar , A �����
	
	// xb �� Mont(x , R^2)
	montmul(xb, m, g, r2m, mp);
	// A �� R
	bint_copy((*tmp), rm);

	// Step 2.
	for (int i = t; i >= 0; i-- )
	{
		// Step 2.1 A �� Mont(A,A)
		montmul(out, m, (*tmp), (*tmp), mp);
		// Step 2.2 if e_i =1  : A �� Mont(A,xb)
		if (bint_seebit(e, i) == 1) montmul((*tmp), m, (*tmp), xb, mp);
	}
	// Step 3. A �� Mont(A,1)
	montmul((*tmp), m, (*tmp), one,mp);

	delbint(xb);
	delbint(one);

	// ���� ���� ����Ų�ٸ� ���� �� ����� �޸� ����
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}


