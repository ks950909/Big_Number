#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_classical_modular_multiplication
// 기  능 : out = (x * y) % m
// 인  자 : BINT out1,BINT out2, const BINT x, const BINT y
// 리턴값 : 없음
// 제한 사항 : x,y 길이 > 0, x,y > 0
//
////////////////////////////////////////////////////////////////////////
void bint_classical_modular_multiplication(BINT out, const BINT m, const BINT x, const BINT y)
{
	// 몫을 저장하기 위한 공간
	BINT tmp,tmp1;
	// 제한사항 체크
	assert(x->sig == POS_SIG); assert(y->sig == POS_SIG); assert(x->len > 0); assert(y->len > 0);
	// x * y 계산
	bint_mul(tmp, x, y);
	// (x * y) / m 계산 , 몫 : tmp , 나머지 : out
	bint_div(tmp1, out, tmp, m);
	// 필요없는 몫 메모리 해제
	delbint(tmp);
	delbint(tmp1);
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : UWORD_inv
// 기  능 : a ^ -1 (mod b)
// 인  자 : UWORD a
// 리턴값 : 없음
// 제한 사항 : a 는 홀수
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
// 함수명 : bint_montgomery_reduction
// 기  능 : TR^-1 mod m ( R = b^n)
// 인  자 : const BINT m,T , const UWORD mp ( - m^-1 (mod b) )
// 리턴값 : BINT out
// 제한 사항 : m * minv == -1 (mod b) , m의 길이 * 2= T의길이
//
////////////////////////////////////////////////////////////////////////
void bint_montgomery_reduction(BINT out, const BINT m, const BINT T,const UWORD mp)
{
	SINT i;
	SINT n = m->len;
	// 중간 값 저장
	BINT u,tmpval;
	// 제한 사항 체크
	assert(m->dat[0] * mp == MASK_WW); 
	// out 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq = (out == m || out == T) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else  tmp = (BINT*)malloc(sizeof(BINT));

	// A <- T
	bint_copy((*tmp), T);
	// u_i 들어갈 공간 만들기
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
		// 중간 메모리 해제
		delbint(tmpval);
	}
	// A <- A / b^n
	bint_rightshift((*tmp), (*tmp), BITSZ_WW * n);
	// if A >= m ,then A <- A - m
	if (bint_compare((*tmp), m) >= 0) bint_sub((*tmp), (*tmp), m);
	// A 최종길이 정하기
	bint_unsigned_makelen(*tmp);
	// 사용한 메모리 해제
	delbint(u);
	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
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
// 함수명 : bint_montgomery_multiplication
// 기  능 : xyR^-1 mod m ( R = b^n)
// 인  자 : const BINT m,x,y , const UWORD minv ( - m^-1 (mod b) )
// 리턴값 : BINT out
// 제한 사항 : m * minv == -1 (mod b) , m의 길이 = x의길이 = y의길이
//
////////////////////////////////////////////////////////////////////////
void bint_montgomery_multiplication(BINT out, const BINT m, const BINT x,const BINT y,const UWORD mp)
{
	SINT i;
	// m 의 길이
	SINT n = m->len;
	// 중간값 정리
	BINT u,xi,tmpval,tmp2val;
	// 제한 사항 체크
	assert(m->dat[0] * mp == MASK_WW); 
	// out 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq = (out == x || out == y || out == m) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else  tmp = (BINT*)malloc(sizeof(BINT));

	// A <- 0
	makebint_zero((*tmp),n);
	// u,xi 한자리수 사용
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

		// 중간 메모리 해제
		delbint(tmpval);
		delbint(tmp2val);
	}
	// if A >= m then A <- A - m 
	if (bint_compare((*tmp), m) >= 0) bint_sub((*tmp), (*tmp), m);
	// A 최종길이 정하기
	bint_unsigned_makelen(*tmp);
	// 사용한 메모리 해제
	delbint(u);
	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
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
	// out 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq = (out == x || out == y || out == m) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else  tmp = (BINT*)malloc(sizeof(BINT));
	BINT ttmp,tttmp;
	bint_mul(ttmp, x, y);
	bint_div(tttmp, (*tmp), ttmp, m);
	delbint(ttmp);
	delbint(tttmp);
	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
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
	// out 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq = (out == x || out == m) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// out값을 바꾸어도 x,y값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else  tmp = (BINT*)malloc(sizeof(BINT));
	BINT ttmp;
	bint_sqr(ttmp, x);
	bint_div(ttmp, (*tmp), ttmp, m);
	delbint(ttmp);

	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
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
