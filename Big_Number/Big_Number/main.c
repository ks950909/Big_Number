#include "BINT.h"
#include <stdlib.h>
#include <string.h>
#include <time.h> /* srand(time(NULL)) */



UWORD makerandom()
{
	UWORD output = 0u;
	output |= (rand() & 0xff);
	output <<= 8;
	output |= (rand() & 0xff);
	output <<= 8;
	output |= (rand() & 0xff);
	output <<= 8;
	output |= (rand() & 0xff);
	return output;
}

void makerandomBINT(BINT x, SINT maxlength)
{
	SINT i;
	x->len = rand() % maxlength + 1;
	makebint(x, x->len);
	x->sig = 1;
	for (i = 0; i < x->len; ++i)
	{
		x->dat[i] = makerandom();
	}
	bint_unsigned_makelen(x);
	i = rand() % 2001;
	if (x->sig == ZERO_SIG || i == 0)
	{
		x->len = 1;
		x->dat[0] = 0u;
		x->sig = ZERO_SIG;
	}
	else x->sig = i <= 1000 ? POS_SIG : NEG_SIG;
}
void makerandomBINT_fix(BINT x, SINT length)
{
	SINT i;
	x->len = length;
	makebint(x, x->len);
	x->sig = 1;
	for (i = 0; i < x->len; ++i)
	{
		x->dat[i] = makerandom();
	}
	bint_unsigned_makelen(x);
	x->sig = POS_SIG;
}
void file2bint(const char filename[], const char filemode[], BINT n)
{
	FILE* f;
	f = fopen(filename, filemode);
	if (f == NULL)
	{
		fprintf(stderr, filename);
		fprintf(stderr, " is not open\n");
		exit(1);
	}
	if (filemode == "r")
		fscan(f, n);
	else
		fprint(f, n);
	fclose(f);
}
void inpython(int argc, char** argv)
{
	BINT a, b, c, d;
	file2bint("a.txt", "r", a);
	file2bint("b.txt", "r", b);
	makebint_zero(d, 1);
	if (argc > 1 && strncmp(argv[1], "add", 4) == 0)
		bint_add(c, a, b);
	else if (argc > 1 && strncmp(argv[1], "sub", 4) == 0)
		bint_sub(c, a, b);
	else if (argc > 1 && strncmp(argv[1], "mul", 4) == 0)
		bint_mul(c, a, b);
	else if (argc > 1 && strncmp(argv[1], "sqr", 4) == 0)
		bint_sqr(c, a);
	else if (argc > 1 && strncmp(argv[1], "div", 4) == 0)
		bint_div(c, d, a, b);
	else
		exit(1);
	//print("c",c);
	file2bint("c.txt", "w", c);
	file2bint("d.txt", "w", d);
	//system("pause");
	delbint(a);
	delbint(b);
	delbint(c);
	delbint(d);
}

void check_plusminus()
{
	BINT a, b, c;
	int cnt = 0;
	int maxlength = 300;
	// a+b = c ,c-= b, c-=a, c ?= 0

	while (cnt < 10000000)
	{
		if (((cnt++) & 0xffff) == 0)
			printf("Case %d\n", cnt);

		makerandomBINT(a, maxlength);
		makerandomBINT(b, maxlength);

		// c = a + b
		bint_add(c, a, b);
		// c -= a
		bint_sub(c, c, a);
		// c -= b
		bint_sub(c, c, b);

		if (c->sig != 0)
		{
			puts("Error");
			print("a", a);
			print("b", b);
			print("c", c);
			break;
		}

		delbint(a);
		delbint(b);
		delbint(c);
	}

	if (cnt == 10000000) puts("All testcase Good!");
}
void check_mulsqr()
{
	BINT a, b, apb, amb, sapb, samb, sapbmsamb, fab, f, ab;
	SINT cnt = 0;
	SINT maxlength = 30;

	//(a+b) ^ 2 - (a-b) ^ 2 ?= 4ab
	uword2bint(f, 4u);
	while (cnt < 10000000)
	{
		if (((cnt++) & 0xffff) == 0)
			printf("cnt : %d\n", cnt);

		makerandomBINT(a, maxlength);
		makerandomBINT(b, maxlength);

		// apb = a+b
		bint_add(apb, a, b);
		// amb = a-b
		bint_sub(amb, a, b);
		// sapb = (a+b)^2
		bint_sqr(sapb, apb);
		// samb = (a-b)^2
		bint_sqr(samb, amb);
		// sapbmsamb = (a+b)^2-(a-b)^2
		bint_sub(sapbmsamb, sapb, samb);
		// ab = a*b
		bint_mul(ab, a, b);
		// fab = 4*a*b
		bint_mul(fab, ab, f);
		if (bint_compare(fab, sapbmsamb) != 0)
		{
			print("a", a);
			print("b", b);
			print("a+b", apb);
			print("a-b", amb);
			print("(a+b)^2", sapb);
			print("(a-b)^2", samb);
			print("(a+b)^2-(a-b)^2", sapbmsamb);
			print("ab", ab);
			print("fab", fab);
			fprintf(stderr, "error");
			break;
		}
		delbint(a);
		delbint(b);
		delbint(apb);
		delbint(amb);
		delbint(sapb);
		delbint(samb);
		delbint(sapbmsamb);
		delbint(ab);
		delbint(fab);
	}
	delbint(f);

	if (cnt == 10000000) puts("All testcase Good!");
}

void check_div()
{
	BINT a, b, q, r;
	BINT qb, qbpr;
	SINT cnt = 0;
	SINT maxlength = 30;

	//a = q * b + r , (0 <= r < |b| )
	while (cnt < 10000000)
	{
		if (((cnt++) & 0xffff) == 0)
			printf("cnt : %d\n", cnt);

		makerandomBINT(a, maxlength);
		makerandomBINT(b, maxlength);

		if (b->sig == 0)continue;

		// q = a / b , r = a % b
		bint_div(q, r, a, b);
		// qb = q * b
		bint_mul(qb, q, b);
		// qbpr = q * b + r
		bint_add(qbpr, qb, r);

		if (bint_compare(a, qbpr) != 0 || bint_unsigned_compare(b, r) <= 0)
		{
			print("a", a);
			print("b", b);
			print("q", q);
			print("r", r);
			print("qb", qb);
			print("qb+r", qbpr);
			fprintf(stderr, "error");
			break;
		}
		delbint(a);
		delbint(b);
		delbint(q);
		delbint(r);
		delbint(qb);
		delbint(qbpr);
	}
	if (cnt == 10000000) puts("All testcase Good!");
}
void onetest()
{
	//one test
	BINT M,Rinv;
	UWORD M_arr[33] = {0x616B52B7 ,0xBEC18FFD ,0x28D3166A ,0xEEE3B067 ,
                  0x0827ABBF ,0x1FE6BFFC ,0x279DECDD ,0x3B5FF0DA ,
                  0x7945EA76 ,0xB09FF817 ,0x3F4C599A ,0x57256C9A ,
                  0x377C1B96 ,0x07EFCC36 ,0x47E66E7F ,0x2D55CC39 ,
                  0xC7E2E48C ,0x7AD9F24A ,0x5B2E13FE ,0x6C47CA03 ,
                  0xCD72D203 ,0x631ED5DB ,0x18471F09 ,0xC0A6D4E4 ,
                  0x31CB4500 ,0x019D3848 ,0xFE4E80FB ,0xF95F7D71 ,
                  0xE36C0B5F ,0xB8DDCD1C ,0x0BEFDD5A ,0x9083F615 };
	UWORD Rinv_arr[33] = {0xa1083f8c,0x8c802575,0x21e8c263,0xbbd59864,
		0xfbe160f5,0x8a79b9af,0x266ef4ef,0x2d1a7038,
		0xcd31960,0x25d0a3cb,0x17590436,0x7145cb91,
		0xf0649e66,0x90364411,0xbc3494e7,0x6c6fc100,
		0x49e8e947,0x9beb5b08,0x5d9f4a1f,0xdedba528,
		0xa979c6db,0x1f5b8a5c,0x7918d4e8,0xc8c0fd00,
		0x454e1f3e,0xa43b6b27,0x5121a3f1,0x9fd68dda,
		0xb4ac4b38,0x1fbd55c4,0x7c5a5165,0x55056db2};
	UWORD mp = 0x71cbd4f9;
	UWORD b_arr[11] = { 0xfffffff,0x00000001 };
	uwordarr2bint(M, M_arr, 32, POS_SIG);
	uwordarr2bint(Rinv, Rinv_arr, 32, POS_SIG);
	BINT tmp1, tmp2, tmp3, R,T,out;
	makebint(T, 64);
	for (int i = 1; i < 64; i++)
	{
		T->dat[i] = 0;
	}
	T->dat[0] = 1;
	T->sig = POS_SIG;

	uword2bint(R, 0x1);
	bint_leftshift(R, R, 32 * 32);
	bint_mul(tmp1,R, Rinv);
	bint_div(tmp2, tmp3, tmp1, M);
	bint_montgomery_reduction(out, M, T, mp);

	print("tmp3", tmp3);

	print("M", M);
	printf("mp %08x\n", -UWORD_inv(M_arr[0]));
	print("out", out);
}
void check_mulinv()
{
	UWORD x, y;
	SINT cnt=0;
	while (TRUE)
	{
		if ((cnt++ & 0xffff) == 0)
			printf("%d\n", cnt);
		x = makerandom();
		if ((x & 1)== 0)continue;
		//printf("%x : \n", x);
		y = UWORD_inv(x);
		//printf("%x\n", y);
		if (x * y != 1)
		{
			printf("%x %x : %x\n", x, y, x * y);
			break;
		}
	}
}
void check_montgomery_reduction()
{
	BINT m,R,T,a,b,tmp1,tmp2,mr,tmp3,tmp4;
	SINT n;
	SINT cnt = 0;
	SINT maxlength = 3;

	//a = q * b + r , (0 <= r < |b| )
	while (cnt < 10000000)
	{
		if (((cnt++) & 0xffff) == 0)
			printf("cnt : %d\n", cnt);
		//printf("cnt : %d\n", cnt);

		makerandomBINT(m, maxlength);
		n = m->len;

		if (m->sig == 0)continue;

		m->sig = POS_SIG;
		if ((m->dat[0] & 1u) == 0)m->dat[0]++;

		uword2bint(R, 1u);
		bint_leftshift(R, R, BITSZ_WW * n);

		makerandomBINT(T, 2*n);
		makebint(T, 2 * n);
		for (SINT i = 0; i < 2*n; i++)
		{
			T->dat[i] = makerandom();
		}
		T->sig = POS_SIG;
		bint_mul(mr, m, R);

		if (bint_compare(T, mr) >= 0)continue;

		UWORD minv = -UWORD_inv(m->dat[0]);

		bint_montgomery_reduction(a, m, T, minv);
		/*
		printf("minv : %08x\n", minv);
		print("m", m);
		print("T", T);
		printf("n : %d\n", n);
		print("a", a);
		*/

		bint_mul(b, a, R);
		bint_div(tmp1, tmp2, b, m);
		bint_div(tmp3, tmp4, T, m);

		//print("tmp2", tmp2);
		//print("tmp4", tmp4);

		if (bint_compare(tmp2,tmp4) != ZERO_SIG )
		{
			print("m", m);
			print("T", T);
			print("R", R);
			print("output", a);
			print("output * r", b);
			print("output * r / m", tmp1);
			print("output * r % m", tmp2);
			print("T / m", tmp3);
			print("T % m", tmp4);
			break;
		}
		delbint(a);
		delbint(b);
		delbint(m);
		delbint(T);
		delbint(tmp1);
		delbint(tmp2);
		delbint(tmp3);
		delbint(tmp4);
	}
	if (cnt == 10000000) puts("All testcase Good!");

}

void check_montgomery_multiplication()
{
	BINT m,R,x,y,a,b,tmp1,tmp2,mr,tmp3,tmp4,xy;
	SINT n;
	SINT cnt = 0;
	SINT maxlength = 3;

	while (cnt < 10000000)
	{
		if (((cnt++) & 0xffff) == 0)
			printf("cnt : %d\n", cnt);
		//printf("cnt : %d\n", cnt);

		makerandomBINT(m, maxlength);
		n = m->len;

		if (m->sig == 0)continue;

		m->sig = POS_SIG;
		if ((m->dat[0] & 1u) == 0)m->dat[0]++;

		uword2bint(R, 1u);
		bint_leftshift(R, R, BITSZ_WW * n);

		makebint(x, n);
		makebint(y, n);
		for (SINT i = 0; i < n; i++)
		{
			x->dat[i] = makerandom();
			y->dat[i] = makerandom();
		}
		x->sig = POS_SIG;
		y->sig = POS_SIG;
		bint_mul(mr, m, R);

		//if (bint_compare(T, mr) >= 0)continue;

		UWORD minv = -UWORD_inv(m->dat[0]);

		bint_montgomery_multiplication(a, m, x, y, minv);
		/*
		printf("minv : %08x\n", minv);
		print("m", m);
		print("T", T);
		printf("n : %d\n", n);
		print("a", a);
		*/

		bint_mul(b, a, R);
		bint_mul(xy, x, y);
		bint_div(tmp1, tmp2, b, m);
		bint_div(tmp3, tmp4, xy, m);

		//print("tmp2", tmp2);
		//print("tmp4", tmp4);

		if (bint_compare(tmp2,tmp4) != ZERO_SIG )
		{
			print("m", m);
			print("x", x);
			print("y", y);
			print("R", R);
			print("xy", xy);
			print("output", a);
			print("output * r", b);
			print("output * r / m", tmp1);
			print("output * r % m", tmp2);
			print("xy / m", tmp3);
			print("xy % m", tmp4);
			break;
		}
		delbint(a);
		delbint(b);
		delbint(m);
		delbint(x);
		delbint(y);
		delbint(xy);
		delbint(tmp1);
		delbint(tmp2);
		delbint(tmp3);
		delbint(tmp4);
	}
	if (cnt == 10000000) puts("All testcase Good!");

}

clock_t elapsed;
float sec;
 
#define START_WATCH \
{\
 elapsed = -clock(); \
}\
 
#define STOP_WATCH \
{\
 elapsed += clock();\
 sec = (float)elapsed/CLOCKS_PER_SEC;\
}\
 
#define PRINT_TIME(qstr) \
{\
 printf("\n[%s: %.10f s]\n",qstr,sec);\
}\


void check_time_div_montgomeryreduction()
{
	BINT *T_bint_arr;
	const SINT testsize = 10000;
	T_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));

	BINT M,Rinv,R;
	UWORD M_arr[33] = {0x616B52B7 ,0xBEC18FFD ,0x28D3166A ,0xEEE3B067 ,
                  0x0827ABBF ,0x1FE6BFFC ,0x279DECDD ,0x3B5FF0DA ,
                  0x7945EA76 ,0xB09FF817 ,0x3F4C599A ,0x57256C9A ,
                  0x377C1B96 ,0x07EFCC36 ,0x47E66E7F ,0x2D55CC39 ,
                  0xC7E2E48C ,0x7AD9F24A ,0x5B2E13FE ,0x6C47CA03 ,
                  0xCD72D203 ,0x631ED5DB ,0x18471F09 ,0xC0A6D4E4 ,
                  0x31CB4500 ,0x019D3848 ,0xFE4E80FB ,0xF95F7D71 ,
                  0xE36C0B5F ,0xB8DDCD1C ,0x0BEFDD5A ,0x9083F615 };
	UWORD Rinv_arr[33] = {0xa1083f8c,0x8c802575,0x21e8c263,0xbbd59864,
		0xfbe160f5,0x8a79b9af,0x266ef4ef,0x2d1a7038,
		0xcd31960,0x25d0a3cb,0x17590436,0x7145cb91,
		0xf0649e66,0x90364411,0xbc3494e7,0x6c6fc100,
		0x49e8e947,0x9beb5b08,0x5d9f4a1f,0xdedba528,
		0xa979c6db,0x1f5b8a5c,0x7918d4e8,0xc8c0fd00,
		0x454e1f3e,0xa43b6b27,0x5121a3f1,0x9fd68dda,
		0xb4ac4b38,0x1fbd55c4,0x7c5a5165,0x55056db2};
	UWORD mp = 0x71cbd4f9;

	uwordarr2bint(M, M_arr, 32, POS_SIG);
	uwordarr2bint(Rinv, Rinv_arr, 32, POS_SIG);
	uword2bint(R, 0x1);
	bint_leftshift(R, R, 32 * 32);
	SINT n = 32;
	BINT mr;
	bint_mul(mr, M, R);

	for (int i = 0; i < testsize; i++)
	{
		TORF flag = TRUE;
		while (flag)
		{
			makerandomBINT_fix(T_bint_arr[i], 2*n);
			if (bint_compare(T_bint_arr[i],mr)< 0)
			{
				flag = FALSE;
			}
		}
	}
	puts("make fin");
	// div
	BINT out,out1,out2;

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_classical_modular_multiplication(out, M, T_bint_arr[i], Rinv);
	}
	STOP_WATCH

	PRINT_TIME("div")

	// montgomery reduction

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_montgomery_reduction(out, M, T_bint_arr[i], mp);
	}

	STOP_WATCH

	PRINT_TIME("montgomery reduction")

	// montgomery multiplication

		/*

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
	}

	STOP_WATCH

	PRINT_TIME("montgomery multiplication")
	*/

}

void check_time_div_montgomeryreduction_montgomerymultiplicatoin()
{
	BINT *x_bint_arr;
	BINT *y_bint_arr;
	BINT *out_bint_arr;
	BINT *out1_bint_arr;
	BINT *out2_bint_arr;
	const SINT testsize = 10000;
	x_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	y_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	out_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	out1_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	out2_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));

	BINT M,Rinv,R;
	UWORD M_arr[33] = {0x616B52B7 ,0xBEC18FFD ,0x28D3166A ,0xEEE3B067 ,
                  0x0827ABBF ,0x1FE6BFFC ,0x279DECDD ,0x3B5FF0DA ,
                  0x7945EA76 ,0xB09FF817 ,0x3F4C599A ,0x57256C9A ,
                  0x377C1B96 ,0x07EFCC36 ,0x47E66E7F ,0x2D55CC39 ,
                  0xC7E2E48C ,0x7AD9F24A ,0x5B2E13FE ,0x6C47CA03 ,
                  0xCD72D203 ,0x631ED5DB ,0x18471F09 ,0xC0A6D4E4 ,
                  0x31CB4500 ,0x019D3848 ,0xFE4E80FB ,0xF95F7D71 ,
                  0xE36C0B5F ,0xB8DDCD1C ,0x0BEFDD5A ,0x9083F615 };
	UWORD Rinv_arr[33] = {0xa1083f8c,0x8c802575,0x21e8c263,0xbbd59864,
		0xfbe160f5,0x8a79b9af,0x266ef4ef,0x2d1a7038,
		0xcd31960,0x25d0a3cb,0x17590436,0x7145cb91,
		0xf0649e66,0x90364411,0xbc3494e7,0x6c6fc100,
		0x49e8e947,0x9beb5b08,0x5d9f4a1f,0xdedba528,
		0xa979c6db,0x1f5b8a5c,0x7918d4e8,0xc8c0fd00,
		0x454e1f3e,0xa43b6b27,0x5121a3f1,0x9fd68dda,
		0xb4ac4b38,0x1fbd55c4,0x7c5a5165,0x55056db2};
	UWORD mp = 0x71cbd4f9;

	uwordarr2bint(M, M_arr, 32, POS_SIG);
	uwordarr2bint(Rinv, Rinv_arr, 32, POS_SIG);
	uword2bint(R, 0x1);
	bint_leftshift(R, R, 32 * 32);
	SINT n = 32;
	BINT mr;
	bint_mul(mr, M, R);

	for (int i = 0; i < testsize; i++)
	{
		TORF flag = TRUE;
		while (flag)
		{
			makerandomBINT_fix(x_bint_arr[i], n);
			if (bint_compare(x_bint_arr[i],M)< 0)
			{
				flag = FALSE;
			}
		}
		flag = TRUE;
		while (flag)
		{
			makerandomBINT_fix(y_bint_arr[i], n);
			if (bint_compare(y_bint_arr[i],M)< 0)
			{
				flag = FALSE;
			}
		}
	}
	puts("make fin");
	BINT out,out1,out2;
	// div

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_mul(out, x_bint_arr[i], y_bint_arr[i]);
		bint_classical_modular_multiplication(out_bint_arr[i], M, out, Rinv);
	}
	STOP_WATCH

	PRINT_TIME("div")

	// montgomery reduction

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_mul(out, x_bint_arr[i], y_bint_arr[i]);
		bint_montgomery_reduction(out1_bint_arr[i], M, out, mp);
	}

	STOP_WATCH

	PRINT_TIME("montgomery reduction")

	// montgomery multiplication


	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_montgomery_multiplication(out2_bint_arr[i], M, x_bint_arr[i], y_bint_arr[i], mp);
	}

	STOP_WATCH

	PRINT_TIME("montgomery multiplication")

	for (int i = 0; i < testsize; i++)
	{
		if (bint_compare(out_bint_arr[i], out1_bint_arr[i]) != 0 || bint_compare(out_bint_arr[i],out2_bint_arr[i]) != 0)
		{
			printf("%d error", i);
			print("", out_bint_arr[i]);
			print("", out1_bint_arr[i]);
			print("", out2_bint_arr[i]);
			return;
		}
	}
	printf("All test same results\n");
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL)); /* use rand */
	//check_montgomery_reduction();
	//check_montgomery_multiplication();
	//check_mulsqr();
	//check_mulinv();
	//check_time_div_montgomeryreduction();
	check_time_div_montgomeryreduction_montgomerymultiplicatoin();
	//onetest();
	return 0;
}