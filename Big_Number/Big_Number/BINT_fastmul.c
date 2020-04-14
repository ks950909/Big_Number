#include "BINT.h"
#include <stdlib.h>


void bint_fastmul_add(BINT out, const BINT x, const BINT y)
{

}


////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_fast_unsigned_mul
// 기  능 : 두 BINt의 절댓값 곱을 빠르게 구해줌
// out = |x|*|y|
// 인  자 : x,y
// 리턴값 : out
// 제한 사항 : x,y의 길이 > 0
//
////////////////////////////////////////////////////////////////////////
void bint_fast_unsigned_mul(BINT out, const BINT x, const BINT y)
{
	SINT len = max(x->len, y->len);
	SINT half = len / 2;

}
