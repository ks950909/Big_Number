#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : make_leftshift_val
// 기  능 : make new x,y(normalization) and return how many shift
// 인  자 : UWORD num(y의 최상위 숫자) , const BINT x, const BINT y,BINt nx, BINT ny
// 리턴값 : SINT (# of leftshift)
// 제한 사항 : x->len > 0 , y->len > 0
//
////////////////////////////////////////////////////////////////////////
SINT make_leftshift_val(UWORD num, const BINT x, const BINT y, BINT nx, BINT ny)
{
	// 결과값
	SINT leftshift_val = 0;
	// 제한사항 체크
	assert(x->len > 0); assert(y->len > 0);
	// y의 최상위 숫자를 LMSB_ONE이상으로 만들음
	while (num < LMSB_ONE)
	{
		num <<= 1;
		// 그때 필요한 leftshift갯수를 저장
		leftshift_val++;
	}
	// 아까 구한 leftshift 만큼 x를 leftshift시켜 nx에 저장
	bint_leftshift(nx, x, leftshift_val);
	// 아까 구한 leftshift 만큼 y를 leftshift시켜 ny에 저장
	bint_leftshift(ny, y, leftshift_val);

	// 결과값 리턴
	return leftshift_val;
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bisection
// 기  능 : (x[i]*b^2 + x[i-1]*b+x[i-2])/(y[t]*b+y[t-1]) 을 찾는 함수
// 인  자 : const BINT x, const BINT y  (x/y)
// 리턴값 : UWORD 결과값
// 제한 사항 : bint_unsigned_div_step3_1에서만 쓰자
//
////////////////////////////////////////////////////////////////////////
UWORD bisection(const BINT x, const BINT y)
{
	BINT out, tmp;
	// 최소 최대 설정
	UWORD lo = 0, hi = MASK_WW;
	UWORD output;
	SINT compare_tmp;
	// out에 사이즈 1할당
	makebint(out, 1); out->sig = POS_SIG;
	// 이분탐색
	while (lo + 1 < hi)
	{
		// out->dat[0] = (lo + hi) / 2
		out->dat[0] = (lo + hi) >> 1;
		if (out->dat[0] < lo) out->dat[0] |= LMSB_ONE;

		// tmp = out * y 한 후 tmp 와 x의 크기 비교
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
		// tmp > x 인 경우
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
// 함수명 : bint_unsigned_div_step2
// 기  능 :
// 인  자 : BINT out1,BINT newx, const BINT newy , const SINT nmt
// 리턴값 : 없음
// 제한 사항 : bint_unsigned_div에서만 쓰기
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div_step2(BINT out1, BINT newx, const BINT newy, const SINT nmt)
{
	BINT tmp;
	SINT compare_tmp;
	// tmp = y*b^(n-t)만들기
	bint_leftshift(tmp, newy, BITSZ_WW * (nmt));
	// 두 값 비교한 값 저장
	compare_tmp = bint_unsigned_compare(newx, tmp);
	// newx >= tmp 인 경우
	if (compare_tmp >= 0)
	{
		// q[n-t] += 1
		out1->dat[nmt]++;
		// x -= y*b^(n-t)
		bint_sub(newx, newx, tmp);
	}
	// 사용한 메모리 해제
	delbint(tmp);
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_div_step3_1
// 기  능 :
// 인  자 : const BINT newx,const BINT newy, BINT out1, BINT tmp,BINT yt, CONST SINT idx,CONST SINT t
// 리턴값 : 없음
// 제한 사항 : bint_unsigned_div에서만 쓰기
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
// 함수명 : bint_unsigned_div_step3_3
// 기  능 :
// 인  자 : BINT newx, const BINT newy, BINt out1, const SINT idx, const SINT t
// 리턴값 : 없음
// 제한 사항 : bint_unsigned_div_step3_3에서만 쓰기
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

	// x와 tmpy2비교
	compare_tmp = bint_unsigned_compare(newx, tmpy2);
	// x < tmpy2
	if (compare_tmp < 0)
	{
		// q[i-t-1]-=1 , tmpy2 의미에 맞게 tmpy1 빼기
		out1->dat[idx - t - 1]--;
		bint_sub(tmpy2, tmpy2, tmpy1);
	}

	// x -= tmpy2
	bint_sub(newx, newx, tmpy2);

	// 메모리 해제
	delbint(tmpy1);
	delbint(tmpy2);
	delbint(tmpy3);
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_div_makesubarr
// 기  능 : out1는 처음 할당 후 프로그램 끝에서 할당 예정,tmp, yt는 고정길이로 사용 예정이므로 미리 메모리 할당
// 인  자 : BINT out1, BINt tmp, BINT yt,BINt tmpy3, const BINt newy,const SINT n cosnt SINT t
// 리턴값 : 없음
// 제한 사항 : bint_unsigned_div에서만 쓰기
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div_makesubarr(BINT out1, BINT tmp, BINT yt, const BINT newy, const SINT n, const SINT t)
{
	// out1 : 길이 n-t+1, 0으로 초기화
	makebint_zero(out1, n - t + 1);
	// 앞으로 양수로 사용하므로 미리 POS_SIG 줌
	out1->sig = POS_SIG;

	// tmp : 길이 3짜리 x[i]*b^2 + x[i-1]*b + x[i-2]에 사용 , 이후로 값이 계속 변하므로 이후에 설정
	makebint(tmp, 3);
	tmp->sig = POS_SIG;
	// yt : 길이 2짜리 y[t] * b + y[t-1]에 사용 , 이후로 값이 변하지 않으므로 지금 설정 ( -1로 인덱스가 떨어지는 경우 -1대입)
	makebint(yt, 2);
	yt->dat[0] = t == 0 ? 0u : newy->dat[t - 1];
	yt->dat[1] = newy->dat[t];
	yt->sig = POS_SIG;
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_unsigned_div_free
// 기  능 : 각 BINT 메모리 해제
// 인  자 : BINT newx, BINT newy, BInt tmp, BINT yt
// 리턴값 : 없음
// 제한 사항 : bint_unsigned_div에서만 쓰기
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
// 함수명 : bint_unsigned_div
// 기  능 : |x| = |y| * out1 + out2 , |y| > |out2|
// 인  자 : BINT out1,BINT out2, const BINT x, const BINT y
// 리턴값 : 없음
// 제한 사항 : x,y 길이 > 0,
//
////////////////////////////////////////////////////////////////////////
void bint_unsigned_div(BINT out1, BINT out2, const BINT x, const BINT y)
{
	// 길이
	SINT n, t;
	// for index i
	SINT i;
	// normalize 결과 저장용
	SINT leftshift_val;
	// 두수의 대소비교 결과 저장용
	SINT compare_tmp;
	// 중간 과정 계산
	BINT newx, newy, tmp, yt;
	// y의 최상위 숫자
	UWORD ytmp = y->dat[y->len - 1];

	// 제한사항 체크
	assert(x->len > 0); assert(y->len > 0);

	// |y| 와 |x|의 크기비교
	compare_tmp = bint_unsigned_compare(x, y);
	// |y| > |x| 라면 out1 = 0 , out2 = x 설정후 리턴
	if (compare_tmp < 0)
	{
		makebint_zero(out1, 1);
		bint_copy(out2, x);
		return;
	}

	// normalization
	leftshift_val = make_leftshift_val(ytmp, x, y, newx, newy);

	// n,t 설정
	n = newx->len - 1; t = newy->len - 1;

	// 동적 할당
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

	// 메모리 해제
	bint_unsigned_div_free(newx, newy, tmp, yt);
}

////////////////////////////////////////////////////////////////////////
//
// 함수명 : bint_div
// 기  능 :
// x = out1 * y + out2 ( out2 < |y|)
// 인  자 : BINT out1,BINT out2, const BINT x, const BINT y
// 리턴값 : 없음
// 제한 사항 : x->len > 0 , y->len > 0, out1 != out2
//
////////////////////////////////////////////////////////////////////////
void bint_div(BINT out1, BINT out2, const BINT x, const BINT y)
{
	// out1 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq1 = (out1 == x || out1 == y) ? TRUE : FALSE;
	// out2 이 x,y와 가르키는 곳이 같은지 체크
	SINT eq2 = (out2 == x || out2 == y) ? TRUE : FALSE;
	// 주소값이 같을때를 대비
	BINT* tmp1, * tmp2;
	// 1저장용
	BINT one;

	// out1값을 바꾸어도 x,y값의 변화가 없음
	if (eq1 == FALSE) tmp1 = out1;
	// out1값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else tmp1 = (BINT*)malloc(sizeof(BINT));
	// out2값을 바꾸어도 x,y값의 변화가 없음
	if (eq2 == FALSE) tmp2 = out2;
	// out2값을 바꾸면 x,y값의 변화가 생기므로 동적할당
	else tmp2 = (BINT*)malloc(sizeof(BINT));

	// 제한 사항 체크
	assert(x->len > 0); assert(y->len > 0); assert(out1 != out2); assert(tmp1 != NULL); assert(tmp2 != NULL);

	// y = 0 , 사실상 오류
	if (y->sig == ZERO_SIG)
	{
		makebint_zero((*tmp1), 1);
		bint_copy((*tmp2), x);
	}
	// x = 0 , 몫,나머지 모두 0
	else if (x->sig == ZERO_SIG)
	{
		makebint_zero((*tmp1), 1);
		makebint_zero((*tmp2), 1);
	}
	// x > 0
	else if (x->sig == POS_SIG)
	{
		bint_unsigned_div((*tmp1), (*tmp2), x, y);
		// 나머지는 항상 r 로 같음
		// 몫
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
			// 나머지
			// y > 0 : y - r , y < 0 : -y - r
			// make -r
			(*tmp2)->sig = NEG_SIG;
			// y > 0 : +y
			if (y->sig == POS_SIG) bint_add((*tmp2), (*tmp2), y);
			// y < 0 : -y
			else bint_sub((*tmp2), (*tmp2), y);
			// 몫
			// y > 0 : -1 - q , y < 0 : q + 1
			// q를 먼저 설정
			if (x->sig != y->sig && (*tmp1)->sig == POS_SIG) (*tmp1)->sig = NEG_SIG;
			// 1을 만들고
			uword2bint(one, 1u);
			// y > 0 : -1 - q
			if (y->sig == POS_SIG)
				bint_sub((*tmp1), (*tmp1), one);
			// y < 0 : q + 1
			else
				bint_add((*tmp1), (*tmp1), one);
			// 사용한 one 메모리 해제
			delbint(one);
		}
	}

	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
	if (eq1 == TRUE)
	{
		delbint(out1);
		bint_copy(out1, (*tmp1));
		delbint((*tmp1));
		free(tmp1);
	}
	// 같은 곳을 가르킨다면 복사 후 사용한 메모리 해제
	if (eq2 == TRUE)
	{
		delbint(out2);
		bint_copy(out2, (*tmp2));
		delbint((*tmp2));
		free(tmp2);
	}
}