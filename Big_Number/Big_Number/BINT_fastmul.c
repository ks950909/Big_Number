#include "BINT.h"
#include <stdlib.h>


void bint_fastmul_add(BINT out, const BINT x, const BINT y)
{

}


////////////////////////////////////////////////////////////////////////
//
// �Լ��� : bint_fast_unsigned_mul
// ��  �� : �� BINt�� ���� ���� ������ ������
// out = |x|*|y|
// ��  �� : x,y
// ���ϰ� : out
// ���� ���� : x,y�� ���� > 0
//
////////////////////////////////////////////////////////////////////////
void bint_fast_unsigned_mul(BINT out, const BINT x, const BINT y)
{
	SINT len = max(x->len, y->len);
	SINT half = len / 2;

}
