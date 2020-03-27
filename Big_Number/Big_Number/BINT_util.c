#include "BINT.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_makelen
// ��  �� : �ֻ��� ��Ʈ 0�� �������� , �ش� ���� 0�̸� out�� sig�� ZERO_SIG, �ƴϸ� POS_SIG�� ����
// ��  �� : �ش� BINT out
// ���ϰ� : ����
// ���� ���� : out->len > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_makelen(BINT out)
{
	// ���ѻ��� üũ
	assert(out->len > 0);
	// �ֻ��� ��Ʈ�� 0�̶�� ������
	// out->len�� 1���� �۾����� �ȵ�
	while (out->len > 1 && out->dat[out->len - 1] == 0)
	{
		// out->len �� �ٿ����� 0�̸� ������
		out->len--;
	}
	// �־���ϳ�?
	if (out->len == 1 && out->dat[0] == 0)
	{
		out->sig = ZERO_SIG;
	}
	else out->sig = POS_SIG;
	//
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_unsigned_compare
// ��  �� : �� ���� ������ ������
// ��  �� : ���� �� const BINT x,y
// ���ϰ� : x > y : POS_SIG , x == y : ZERO_SIG , x < y : NEG_SIG
// ���� ���� : �̸� bint_unsigned_makelen �����ֱ�
//
////////////////////////////////////////////////////////////////////////
SINT bint_unsigned_compare(const BINT x, const BINT y)
{
	int i; /* for index i */
	// �μ��� ���̰� �ٸ� ��� , ���̰� �� �� ���� ũ��.
	if (x->len != y->len) return x->len > y->len ? POS_SIG : NEG_SIG;
	// ū index���� ū ���� ������ �� ũ��.
	for (i = x->len - 1; i >= 0; --i)
	{
		// ��� index���� �� ���ڰ� �ٸ��ٸ� �� ���ڸ� ��
		if (x->dat[i] != y->dat[i]) return x->dat[i] > y->dat[i] ? POS_SIG : NEG_SIG;
	}
	// ��� index���� ���ڰ� ���ٸ� �� ���� ����
	return ZERO_SIG;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_compare
// ��  �� : �� ���� ������
// ��  �� : ���� �� const BINT x,y
// ���ϰ� : x > y : POS_SIG , x == y : ZERO_SIG , x < y : NEG_SIG
// ���� ���� : �̸� bint_unsigned_makelen �����ֱ�
//
////////////////////////////////////////////////////////////////////////
SINT bint_compare(const BINT x, const BINT y)
{
	// �� ���� ��ȣ�� �ٸ� ���
	if (x->sig != y->sig) return x->sig > y->sig ? POS_SIG : NEG_SIG;
	// x�� 0�� ��� , y�� ��ȣ�� x�� �����Ƿ� y�� 0��
	if (x->sig == ZERO_SIG) return ZERO_SIG;
	// x,y�� ����� ��� , ������ ū ���� ŭ
	if (x->sig == POS_SIG) return bint_unsigned_compare(x, y);
	// x,y�� ������ ��� , ������ ���� ���� ŭ
	else return NEGATIVE(bint_unsigned_compare(x, y));
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_copy
// ��  �� : y�� ������ x�� ����
// ��  �� : ���簴ü BINT x, �Ǻ��簴ü const BINT y
// ���ϰ� : ����
// ���� ���� : y->len > 0
//
////////////////////////////////////////////////////////////////////////
void bint_copy(BINT x, const BINT y)
{
	assert(y->len > 0);
	makebint(x, y->len);
	memcpy(x->dat, y->dat, sizeof(UWORD) * y->len);
	x->sig = y->sig;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : makebint
// ��  �� : BINT�� ����(�����Ҵ�), �ʱⰪ : len = size, sig = ZERO_SIG
// ��  �� : ������ BINT , const SINT size
// ���ϰ� : ����
// ���� ���� : size > 0
//
////////////////////////////////////////////////////////////////////////
void makebint(BINT n, const SINT size)
{
	assert(size > 0);
	// size ��ŭ �����Ҵ�
	n->dat = (UWORD*)malloc(size * sizeof(UWORD));
	// 0���� �ʱ�ȭ �ϹǷ� 0
	n->sig = ZERO_SIG;
	// ���̴� size�� ��
	n->len = size;
}
////////////////////////////////////////////////////////////////////////
//
// �Լ��� : makebint_zero
// ��  �� : BINT�� ����(�����Ҵ�), �ʱⰪ : data = all zero ,len = size, sig = ZERO_SIG
// ��  �� : ������ BINT , const SINT size
// ���ϰ� : ����
// ���� ���� : size > 0
//
////////////////////////////////////////////////////////////////////////
void makebint_zero(BINT n, const SINT size)
{
	assert(size > 0);
	// size ��ŭ �����Ҵ�
	n->dat = (UWORD*)calloc(size, sizeof(UWORD));
	// 0���� �ʱ�ȭ �ϹǷ� 0
	n->sig = ZERO_SIG;
	// ���̴� size�� ��
	n->len = size;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uword2bint
// ��  �� : UWORD���� �̿��Ͽ� BINT�� ����, �ʱⰪ : data = x ,len = 1, sig = 1
// ��  �� : ������ BINT , �ش� const UWORD x
// ���ϰ� : ����
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
void uword2bint(BINT n, const UWORD x)
{
	// size 1�� n�� �����
	makebint(n, 1);
	// dat[0]�� x����
	n->dat[0] = x;
	// ��ȣ�� 0 �Ǵ� ���
	n->sig = x == 0 ? ZERO_SIG : POS_SIG;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : uwordarr2bint
// ��  �� : UWORD Array ���� �̿��Ͽ� BINT�� ����, �ʱⰪ : data = x ,len = sz, sig = sig
// ��  �� : ������ BINT , �ش� UWORD Array x, �ش� ���� const SINT size , �ش� ��ȣ const SINT sig
// ���ϰ� : ����
// ���� ���� : sz > 0
//
////////////////////////////////////////////////////////////////////////
void uwordarr2bint(BINT n, const P_UWORD x, const SINT sz, const SINT sig)
{
	// size�� sz�� n�� �����
	makebint(n, sz);
	// �����ʹ� x�� ����
	memcpy(n->dat, x, sizeof(UWORD) * sz);
	// ��ȣ�� sig
	n->sig = sig;
}

////////////////////////////////////////////////////////////////////////
//
// �Լ��� : delbint
// ��  �� : �ش� BINT dat �޸� ���� �� �ٸ��� �ʱ�ȭ (���� 0�� BINT�� �޸� ��������)
// ��  �� : �ش� BINT
// ���ϰ� : ����
// ���� ���� : ����
//
////////////////////////////////////////////////////////////////////////
void delbint(BINT n)
{
	// �޸� ���� , ���̰� 0�� �ƴ϶�� ������ �Ҵ�Ǿ��ִ� ����
	if (n->len != 0) free(n->dat);
	// ��ȣ 0���� �ʱ�ȭ
	n->sig = ZERO_SIG;
	// ���� 0���� �ʱ�ȭ
	n->len = 0;
}