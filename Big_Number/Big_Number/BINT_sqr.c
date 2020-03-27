#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : uword_2mul
// 기  능 :
// d * b^2 + c * b + out <= out + 2*x*y + c + d * b , d < 2
// 인  자 : P_UWORD out, const UWORD x,const UWORD y , P_UWORD c, P_UWORD d
// 리턴값 : 없음
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
void uword_2mul(P_UWORD out, const UWORD x, const UWORD y, P_UWORD c, P_UWORD d)
{
	// 중간값 저장 임시공간
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
// 함수명 : bint_unsigned_sqr
// 기  능 : out = x * x
// 인  자 : BINT out, const BINT x
// 리턴값 : 없음
// 제한 사항 : x->len > 0
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_sqr(BINT out, const BINT x)
{
	// for index i,j
	SINT i, j;
	// x의 길이
	SINT n = x->len;
	// 자리올림 c,d
	UWORD c, d;
	// 제한사항 체크
	assert(x->len > 0);
	// Step 1: output 동적할당 및 초기화 (결과값의 최대길이 : 2n+1)
	makebint_zero(out, 2 * n + 1);
	// Step 2: For i from 0 to (n-1) do the following :
	for (i = 0; i < n; ++i)
	{
		// c,d 초기화
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
// 함수명 : bint_sqr
// 기  능 : out = x * x
// 인  자 : BINT out, const BINT x
// 리턴값 : 없음
// 제한 사항 : x->len > 0
//
////////////////////////////////////////////////////////////////////////
void bint_sqr(BINT out, const BINT x)
{
	// out 이 x와 가르키는 곳이 같은지 체크
	SINT eq = (out == x) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp;
	// out값을 바꾸어도 x값의 변화가 없음
	if (eq == FALSE) tmp = out;
	// out값을 바꾸면 x값의 변화가 생기므로 동적할당
	else tmp = (BINT*)malloc(sizeof(BINT));

	// 제한사항 체크
	assert(x->len > 0); assert(tmp != NULL);
	// x == 0인 경우
	if (x->sig == 0)
	{
		// out = 0
		makebint_zero((*tmp), 1);
	}
	// x != 0인 경우
	else
	{
		// tmp = x * x
		bint_unsigned_sqr((*tmp), x);
		// 결과값 부호는 항상 양수
		(*tmp)->sig = POS_SIG;
	}

	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
	if (eq == TRUE)
	{
		delbint(out);
		bint_copy(out, (*tmp));
		delbint((*tmp));
		free(tmp);
	}
}