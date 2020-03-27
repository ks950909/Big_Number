#include "BINT.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_makelen
// 기  능 : 최상위 비트 0을 제거해줌 , 해당 값이 0이면 out의 sig를 ZERO_SIG, 아니면 POS_SIG로 설정
// 인  자 : 해당 BINT out
// 리턴값 : 없음
// 제한 사항 : out->len > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_makelen(BINT out)
{
	// 제한사항 체크
	assert(out->len > 0);
	// 최상위 비트가 0이라면 없애줌
	// out->len이 1보다 작아지면 안됨
	while (out->len > 1 && out->dat[out->len - 1] == 0)
	{
		// out->len 을 줄여가며 0이면 지워줌
		out->len--;
	}
	// 넣어야하나?
	if (out->len == 1 && out->dat[0] == 0)
	{
		out->sig = ZERO_SIG;
	}
	else out->sig = POS_SIG;
	//
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_compare
// 기  능 : 두 수의 절댓값을 비교해줌
// 인  자 : 비교할 두 const BINT x,y
// 리턴값 : x > y : POS_SIG , x == y : ZERO_SIG , x < y : NEG_SIG
// 제한 사항 : 미리 bint_unsigned_makelen 돌려주기
//
////////////////////////////////////////////////////////////////////////
SINT bint_unsigned_compare(const BINT x, const BINT y)
{
	int i; /* for index i */
	// 두수의 길이가 다른 경우 , 길이가 더 긴 수가 크다.
	if (x->len != y->len) return x->len > y->len ? POS_SIG : NEG_SIG;
	// 큰 index에서 큰 수의 절댓값이 더 크다.
	for (i = x->len - 1; i >= 0; --i)
	{
		// 어떠한 index에서 두 숫자가 다르다면 두 숫자를 비교
		if (x->dat[i] != y->dat[i]) return x->dat[i] > y->dat[i] ? POS_SIG : NEG_SIG;
	}
	// 모든 index에서 숫자가 같다면 두 수는 같음
	return ZERO_SIG;
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_compare
// 기  능 : 두 수를 비교해줌
// 인  자 : 비교할 두 const BINT x,y
// 리턴값 : x > y : POS_SIG , x == y : ZERO_SIG , x < y : NEG_SIG
// 제한 사항 : 미리 bint_unsigned_makelen 돌려주기
//
////////////////////////////////////////////////////////////////////////
SINT bint_compare(const BINT x, const BINT y)
{
	// 두 수의 부호가 다른 경우
	if (x->sig != y->sig) return x->sig > y->sig ? POS_SIG : NEG_SIG;
	// x가 0인 경우 , y의 부호가 x와 같으므로 y도 0임
	if (x->sig == ZERO_SIG) return ZERO_SIG;
	// x,y가 양수인 경우 , 절댓값이 큰 쪽이 큼
	if (x->sig == POS_SIG) return bint_unsigned_compare(x, y);
	// x,y가 음수인 경우 , 절댓값이 작은 쪽이 큼
	else return NEGATIVE(bint_unsigned_compare(x, y));
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_copy
// 기  능 : y를 복사해 x에 넣음
// 인  자 : 복사객체 BINT x, 피복사객체 const BINT y
// 리턴값 : 없음
// 제한 사항 : y->len > 0
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
// 함수명 : makebint
// 기  능 : BINT값 생성(동적할당), 초기값 : len = size, sig = ZERO_SIG
// 인  자 : 생성할 BINT , const SINT size
// 리턴값 : 없음
// 제한 사항 : size > 0
//
////////////////////////////////////////////////////////////////////////
void makebint(BINT n, const SINT size)
{
	assert(size > 0);
	// size 만큼 동적할당
	n->dat = (UWORD*)malloc(size * sizeof(UWORD));
	// 0으로 초기화 하므로 0
	n->sig = ZERO_SIG;
	// 길이는 size로 함
	n->len = size;
}
////////////////////////////////////////////////////////////////////////
//
// 함수명 : makebint_zero
// 기  능 : BINT값 생성(동적할당), 초기값 : data = all zero ,len = size, sig = ZERO_SIG
// 인  자 : 생성할 BINT , const SINT size
// 리턴값 : 없음
// 제한 사항 : size > 0
//
////////////////////////////////////////////////////////////////////////
void makebint_zero(BINT n, const SINT size)
{
	assert(size > 0);
	// size 만큼 동적할당
	n->dat = (UWORD*)calloc(size, sizeof(UWORD));
	// 0으로 초기화 하므로 0
	n->sig = ZERO_SIG;
	// 길이는 size로 함
	n->len = size;
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword2bint
// 기  능 : UWORD값을 이용하여 BINT값 생성, 초기값 : data = x ,len = 1, sig = 1
// 인  자 : 생성할 BINT , 해당 const UWORD x
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void uword2bint(BINT n, const UWORD x)
{
	// size 1인 n을 만들고
	makebint(n, 1);
	// dat[0]에 x대입
	n->dat[0] = x;
	// 부호는 0 또는 양수
	n->sig = x == 0 ? ZERO_SIG : POS_SIG;
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uwordarr2bint
// 기  능 : UWORD Array 값을 이용하여 BINT값 생성, 초기값 : data = x ,len = sz, sig = sig
// 인  자 : 생성할 BINT , 해당 UWORD Array x, 해당 길이 const SINT size , 해당 부호 const SINT sig
// 리턴값 : 없음
// 제한 사항 : sz > 0
//
////////////////////////////////////////////////////////////////////////
void uwordarr2bint(BINT n, const P_UWORD x, const SINT sz, const SINT sig)
{
	// size가 sz인 n을 만들고
	makebint(n, sz);
	// 데이터는 x와 같음
	memcpy(n->dat, x, sizeof(UWORD) * sz);
	// 부호는 sig
	n->sig = sig;
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : delbint
// 기  능 : 해당 BINT dat 메모리 해제 및 다른값 초기화 (길이 0인 BINT는 메모리 해제상태)
// 인  자 : 해당 BINT
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void delbint(BINT n)
{
	// 메모리 제거 , 길이가 0이 아니라면 데이터 할당되어있다 가정
	if (n->len != 0) free(n->dat);
	// 부호 0으로 초기화
	n->sig = ZERO_SIG;
	// 길이 0으로 초기화
	n->len = 0;
}