#include "BINT.h"
#include <stdlib.h>
#include <string.h>
#include <time.h> /* srand(time(NULL)) */

// memory
#include <crtdbg.h>

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
	SINT maxlength = 3;

	//a = q * b + r , (0 <= r < |b| )
	while (cnt < 10000000)
	{
		if (((cnt++) & 0xffff) == 0)
			printf("cnt : %d\n", cnt);

		makerandomBINT(a, maxlength);
		makerandomBINT(b, maxlength);
		a->sig = POS_SIG;
		b->sig = POS_SIG;

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
	//_CrtSetBreakAlloc(88);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//one test
	UWORD a_arr[33] = {0xB6F11BC2,0x4A685E41,0x399B04CB,0x67EDB8EF,0x7788CC47,0xEBF07FAC,0x8DDE7BA6,0x0E4BA226,0xEECB0A78,0x9B1C4673,0xDDFFE8F3,0x71464389,0xF52F7670,0x04F0A6C3,0xA1938AEE,0xD9042D63,0xA469EF8E,0x0DB1CD0A,0x9270E315,0xCEEA2F41,0x556D986D,0xE0E52FF4,0xAAAAB424,0x199C5CF2,0xA172D1EB,0xE64F7B6E,0x2096FE19,0xC4000481,0x61EC3C26,0xC7F6A3A6,0x6FCA0230,0x173BB73C };
	UWORD e_arr[33] = {0x10E72CB2,0x0F5CB59F,0xF2F37224,0x6E9D5EE1,0xF4141B7F,0xD86F37E6,0xB2F4AFB0,0x052DB1DD,0x1E3EA6C8,0x3F757059,0xFD23F07D,0xE7AACCD3,0x68D86FA9,0xEF735126,0x4C1578C8,0x5E3A3F12,0xEB98567E,0xCFAF0AB9,0x59A0C60F,0xF242DA24,0xFF737CE6,0x09AC0CBF,0x1BD99D0F,0x6C67B0D9,0x3E9E42BD,0x06310A26,0xCD13FCC4,0xD68D113D,0x818E4A1F,0x6F42F31A,0xE7E5256C,0x79DA8E9C};
	UWORD m_arr[33] = {0xAD8FB979,0x6E1DD707,0x4F1B40E7,0x9CB148B0,0xDF516983,0x034B8840,0xB99C7AFA,0x1B07C7A9,0x8F68AE1F,0xEF44693E,0xE7A4B267,0x9E248535,0x9847286C,0xDB0D692C,0x50C8276B,0x6D2B13A2,0x11A4B8C5,0xAFECB979,0xAFDC5A82,0x1182427A,0x547480C9,0x9664CBD1,0xFCF50E69,0x54CD248D,0xF9EBDF53,0xF73B5021,0x7068421E,0x3FF208E6,0xDA807982,0x7D773897,0x85CB39DD,0x1A1680D3};
	UWORD x_arr[64] = {0x54afba80,0x33647940,0x197709dd,0xcd0f51dd,0x1f9c44c4,0xcad22bfe,0x215ab259,0xc913d0b8,0x28cb2a9c,0x404157c4,0xe0f76cd,0x20579f04,0xd5fc4ada,0xd45b0fef,0x3274b141,0x6c16693c,0x96e00a7f,0x734a4bfc,0x6bfc5822,0xfb5a4eff,0xdf52a1d4,0x6d462700,0xb9ba344c,0x19648f93,0xa0d673c,0xb8675bd7,0x690fff87,0x3a7e3318,0x22dfacf7,0xed7325dc,0x370faf32,0x7d30eca9,0x5b0a4fc2,0xafeded36,0x21de0adf,0x7ef065bd,0x95962a6a,0x3a34e95,0x46b4f9c5,0x5c8295ce,0xa2e08559,0xb5887fc0,0x1fdd64b7,0xcc0e496,0xcba70517,0xd1234729,0xbb4d3eb};
	UWORD y_arr[64] = {0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x2ac53910,0x90f44b18,0xeaf00b1b,0xefbd1e29,0x54b62f4e,0xea8d6039,0x2bbfe2bd,0xf0c3972d,0x0572dcd4,0xe81dd5c1,0xe0977885,0x3e5338ba,0x386fbcde,0x6e6c3503,0x16a75499,0x495185e6,0xb3354ebd,0xffffffff,0x6a9c3a30,0xb3b0f517,0x859bb631,0x12ddb936,0x19558d64,0x0bfebd51,0x1bdd92c2,0x7f6c0629,0x1aa9c3bf,0x280fe1e6,0xa2812245,0xf6f5fb69,0x09764c1c,0x1ba7e4eb,0x0bb4d3eb };

	UWORD xx_arr[64] = {0x0a95f750,0xa66c8f28,0xa32ee13b,0x99a1ea3b,0xc3f38898,0x395a457f,0x042b564b,0x99227a17,0x85196553,0xa8082af8,0x81c1eed9,0x440af3e0,0xfabf895b,0x3a8b61fd,0x864e9628,0xac3802f5,0xc7d921b2,0x89393c7d,0x0b5cbce2,0x4be12f7d,0xe56f7e17,0xde36e437,0x989e2352,0x9b70640c,0xbafe13fb,0x85055857,0x51730bf3,0x44143c0c,0xb67cec49,0xd96ff56b,0x847a73ee,0xdda0b504,0xa8ffe9d6,0x8eabb3d8,0xafe4b48b,0x4c5be30f,0xfcc6c718,0xcb1bcc63,0x157c5147,0xe12132b0,0x8e05a5a9,0x46cbc9c4,0x447b01bb,0x2cd5f75d,0xf11ce7a1,0x73031d6a,0x8ae09c8d,0x2c58b53b,0xea624edf,0xe52f7f4c,0x5538e601,0x1e9520ac,0x88c36f7a,0x622365fa,0x2dae7168,0xf2a8dde1,0x7bbd61fe,0x51a5be8f,0xaae3552b,0x77b845c1,0xa8d895e3,0x00214277,0xe3c35748,0x01c193ec};
	UWORD yy_arr[64] = {0xad8fb979,0x6e1dd707,0x4f1b40e7,0x9cb148b0,0xdf516983,0x034b8840,0xb99c7afa,0x1b07c7a9,0x8f68ae1f,0xef44693e,0xe7a4b267,0x9e248535,0x9847286c,0xdb0d692c,0x50c8276b,0x6d2b13a2,0x11a4b8c5,0xafecb979,0xafdc5a82,0x1182427a,0x547480c9,0x9664cbd1,0xfcf50e69,0x54cd248d,0xf9ebdf53,0xf73b5021,0x7068421e,0x3ff208e6,0xda807982,0x7d773897,0x85cb39dd,0x1a1680d3};
	BINT x,y,a,e,m,kary,kary_modify,xx,yy;
	uwordarr2bint(a, a_arr, 32, POS_SIG);
	uwordarr2bint(e, e_arr, 32, POS_SIG);
	uwordarr2bint(m, m_arr, 32, POS_SIG);
	uwordarr2bint(x, x_arr, 64, POS_SIG);
	uwordarr2bint(y, y_arr, 64, POS_SIG);
	uwordarr2bint(xx, xx_arr, 64, POS_SIG);
	uwordarr2bint(yy, yy_arr, 32, POS_SIG);

	//bint_div(kary,kary_modify,xx, yy);

	bint_ltor_exponentiation(kary, a, e, m,  NULL, NULL);
	bint_ltor_kary_exponentiation(kary_modify, a, e, m, 8, NULL, NULL);
	bint_rtol_exponentiation(xx, a, e, m, NULL, NULL);
	print("kary", kary);
	print("kary_modify", kary_modify);
	print("rtol",xx );
	//bint_sqr(a,  e);
	delbint(a);
	delbint(e);
	delbint(m);
	//delbint(kary);
	//delbint(kary_modify);
	//_CrtDumpMemoryLeaks();
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
void check_time_sqrmontred_montgomerymultiplication()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
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
		BOOL flag = TRUE;
		while (flag)
		{
			makerandomBINT_fix(T_bint_arr[i], n);
			if (bint_compare(T_bint_arr[i],mr)< 0)
			{
				flag = FALSE;
			}
		}
	}
	puts("make fin");
	BINT out,tmp;

	// montgomery reduction

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_sqr(tmp, T_bint_arr[i]);
		bint_montgomery_reduction(out, M, tmp, mp);
	}

	STOP_WATCH

	PRINT_TIME("montgomery reduction")

	// montgomery multiplication


	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_montgomery_multiplication(out, M, T_bint_arr[i], T_bint_arr[i], mp);
	}

	STOP_WATCH

	PRINT_TIME("montgomery multiplication")

	_CrtDumpMemoryLeaks();
}
void check_exp()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtDumpMemoryLeaks();
	BINT a, e,rtol,ltor,kary,kary_modify,sliding,montgo,m;
	BINT rm,r2m,R,R2,tmp;
	SINT cnt = 0;
	SINT maxlength = 32;
	SINT length = 32;
	SINT smalltest = 11;
	SINT k;
	//uword2bint(e, smalltest);

	uword2bint(R, 0x1);
	uword2bint(R2, 0x1);
	R->sig = POS_SIG;
	bint_leftshift(R, R, BITSZ_WW * length);
	bint_leftshift(R2, R2, 2*BITSZ_WW * length);

	while (cnt < 10000)
	{
		printf("%d\n", cnt++);

		// make a,m,e,k
		makerandomBINT_fix(a, length);
		makerandomBINT_fix(m, length);
		makerandomBINT_fix(e, length);
		e->sig = POS_SIG;
		a->sig = POS_SIG;
		m->sig = POS_SIG;
		if ((m->dat[0] & 1) == 0) m->dat[0]++;
		k = rand() % 8 + 1;

		// use montgomery exponentiation
		bint_div(tmp, rm, R, m);
		delbint(tmp);
		bint_div(tmp, r2m, R2, m);
		delbint(tmp);
		MP_montgomery = -UWORD_inv(m->dat[0]);

		// exponentiation
		bint_rtol_exponentiation(rtol, a, e, m, NULL, NULL);
		bint_ltor_exponentiation(ltor, a, e, m, NULL, NULL);
		bint_ltor_kary_exponentiation(kary, a, e, m,k, NULL, NULL);
		bint_ltor_kary_modify_exponentiation(kary_modify, a, e, m,k, NULL, NULL);
		bint_slidingwindow_exponentiation(sliding, a, e, m, k, NULL, NULL);
		bint_montogomery_exponentiation(montgo, a, e, m, MP_montgomery, rm, r2m,NULL);
		
		if (bint_compare(rtol, ltor) != 0 || bint_compare(rtol,kary) != 0 || bint_compare(rtol,kary_modify) != 0 || bint_compare(rtol,sliding) != 0 || bint_compare(rtol,montgo) != 0)
		{
			puts("error");
			printf("k : %d\n", k);
			print("a", a);
			print("e", e);
			print("m", m);
			print("rtol", rtol);
			print("ltor", ltor);
			print("kary", kary);
			print("kary_modify", kary_modify);
			print("sliding window", sliding);
			print("montgomery", montgo);
			break;
		}
		if (cnt == 10000) printf("All test Good \n");
		delbint(a);
		delbint(m);
		delbint(e);
		delbint(rtol);
		delbint(ltor);
		delbint(kary);
		delbint(kary_modify);
		delbint(sliding);
		delbint(montgo);
		delbint(rm);
		delbint(r2m);

		//_CrtDumpMemoryLeaks();
	}
	delbint(rm);
	delbint(r2m);
	delbint(R);
	delbint(R2);
}
void exp_time()
{
	// make constant 
	const SINT testsize = 100;
	const SINT length = 32;
	// use at montgomery
	BINT rm,r2m,R,R2,tmp;
	// use at kary,kary_modify,slidingwindow
	SINT k = 6;
	// make g
	BINT *g_bint_arr;
	g_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make e
	BINT* e_bint_arr;
	e_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make m
	BINT* m_bint_arr;
	m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make rm
	BINT* rm_bint_arr;
	rm_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make r2m
	BINT* r2m_bint_arr;
	r2m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make mp
	UWORD* mp_arr;
	mp_arr = (UWORD*)malloc(testsize * sizeof(UWORD));
	// make outarr
	BINT** out_bint_arr;
	out_bint_arr = (BINT**)malloc(testsize * sizeof(BINT*));
	for (int i = 0; i < testsize; i++)
	{
		out_bint_arr[i] = (BINT*)malloc(6 * sizeof(BINT));
	}

	// use at montgomery
	uword2bint(R, 0x1);
	uword2bint(R2, 0x1);
	R->sig = POS_SIG;
	bint_leftshift(R, R, BITSZ_WW * length);
	bint_leftshift(R2, R2, 2*BITSZ_WW * length);

	for (int i = 0; i < testsize; i++)
	{
		makerandomBINT_fix(g_bint_arr[i], length);
		makerandomBINT_fix(e_bint_arr[i], length);
		makerandomBINT_fix(m_bint_arr[i], length);
		e_bint_arr[i]->sig = POS_SIG;
		g_bint_arr[i]->sig = POS_SIG;
		m_bint_arr[i]->sig = POS_SIG;
		if ((m_bint_arr[i]->dat[0] & 1) == 0) m_bint_arr[i]->dat[0]++;
		k = rand() % 8 + 1;

		// use montgomery exponentiation
		bint_div(tmp, rm_bint_arr[i], R, m_bint_arr[i]);
		delbint(tmp);
		bint_div(tmp, r2m_bint_arr[i], R2, m_bint_arr[i]);
		delbint(tmp);
		mp_arr[i] = -UWORD_inv(m_bint_arr[i]->dat[0]);
	}
	puts("make data");
	
	// rtol
	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_rtol_exponentiation(out_bint_arr[i][0], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], NULL, NULL);
	}

	STOP_WATCH

	PRINT_TIME("rtol")


	// ltor
	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_ltor_exponentiation(out_bint_arr[i][1], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], NULL, NULL);
	}

	STOP_WATCH

	PRINT_TIME("ltor")


	// kary
	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_ltor_kary_exponentiation(out_bint_arr[i][2], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], k, NULL, NULL);
	}

	STOP_WATCH

	PRINT_TIME("kary")

	// kary_modify
	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_ltor_kary_modify_exponentiation(out_bint_arr[i][3], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], k, NULL, NULL);
	}

	STOP_WATCH

	PRINT_TIME("kary_modify")

	//sliding window
	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_slidingwindow_exponentiation(out_bint_arr[i][4], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], k, NULL, NULL);
	}

	STOP_WATCH

	PRINT_TIME("sliding window")

	//montgomery exponentiation
	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_montogomery_exponentiation(out_bint_arr[i][5], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], mp_arr[i], rm_bint_arr[i], r2m_bint_arr[i], NULL);
	}

	STOP_WATCH

	PRINT_TIME("montgomery")

	for (int i = 0; i < testsize; i++)
	{
		for (int j = 1; j < 6; j++)
		{
			if (bint_compare(out_bint_arr[i][0], out_bint_arr[i][j]) != 0)
			{
				puts("error");
				for (int k = 0; k < 6; k++)
				{
					print("", out_bint_arr[i][k]);
				}
				return;
			}
		}
	}
	puts("no Error");

	for (int i = 0; i < testsize; i++)
	{
		delbint(g_bint_arr[i]);
		delbint(e_bint_arr[i]);
		delbint(m_bint_arr[i]);
		for(int j = 0 ; j < 6 ; j++)
		delbint(out_bint_arr[i][j]);
		delbint(rm_bint_arr[i]);
		delbint(r2m_bint_arr[i]);
	}
	free(g_bint_arr);
	free(e_bint_arr);
	free(m_bint_arr);
	free(rm_bint_arr);
	free(r2m_bint_arr);
	free(mp_arr);
	for (int i = 0; i < testsize; i++)
		free(out_bint_arr[i]);
	free(out_bint_arr);
}
void kary_time()
{
	// make constant 
	const SINT testsize = 100;
	const SINT length = 32;
	const SINT maxk = 12;
	// make g
	BINT *g_bint_arr;
	g_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make e
	BINT* e_bint_arr;
	e_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make m
	BINT* m_bint_arr;
	m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));

	// make outarr
	BINT** out_bint_arr;
	out_bint_arr = (BINT**)malloc(testsize * sizeof(BINT*));
	for (int i = 0; i < testsize; i++)
	{
		out_bint_arr[i] = (BINT*)malloc((maxk+1) * sizeof(BINT));
	}


	for (int i = 0; i < testsize; i++)
	{
		makerandomBINT_fix(g_bint_arr[i], length);
		makerandomBINT_fix(e_bint_arr[i], length);
		makerandomBINT_fix(m_bint_arr[i], length);
		e_bint_arr[i]->sig = POS_SIG;
		g_bint_arr[i]->sig = POS_SIG;
		m_bint_arr[i]->sig = POS_SIG;
		if ((m_bint_arr[i]->dat[0] & 1) == 0) m_bint_arr[i]->dat[0]++;
	}
	puts("make data");
	
	for (int k = 1; k <= maxk; k++)
	{
		char ss[20];

		START_WATCH

		for (int i = 0; i < testsize; i++)
		{
			bint_ltor_kary_exponentiation(out_bint_arr[i][k], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i],k, NULL, NULL);
		}

		STOP_WATCH

		sprintf(ss, "kary k = %d", k);
		PRINT_TIME(ss);
	}

	for (int i = 0; i < testsize; i++)
	{
		for (int j = 2; j <= maxk; j++)
		{
			if (bint_compare(out_bint_arr[i][1], out_bint_arr[i][j]) != 0)
			{
				puts("error");
				for (int k = 1; k <= maxk; k++)
				{
					print("", out_bint_arr[i][k]);
				}
				return;
			}
		}
	}
	puts("no Error");
	for (int i = 0; i < testsize; i++)
	{
		delbint(g_bint_arr[i]);
		delbint(e_bint_arr[i]);
		delbint(m_bint_arr[i]);
		for(int j = 1 ; j <= maxk ; j++)
		delbint(out_bint_arr[i][j]);
	}
	free(g_bint_arr);
	free(e_bint_arr);
	free(m_bint_arr);
	for (int i = 0; i < testsize; i++)
		free(out_bint_arr[i]);
	free(out_bint_arr);
}


void kary_modify_time()
{
	// make constant 
	const SINT testsize = 100;
	const SINT length = 32;
	const SINT maxk = 12;
	// make g
	BINT *g_bint_arr;
	g_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make e
	BINT* e_bint_arr;
	e_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make m
	BINT* m_bint_arr;
	m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));

	// make outarr
	BINT** out_bint_arr;
	out_bint_arr = (BINT**)malloc(testsize * sizeof(BINT*));
	for (int i = 0; i < testsize; i++)
	{
		out_bint_arr[i] = (BINT*)malloc((maxk+1) * sizeof(BINT));
	}


	for (int i = 0; i < testsize; i++)
	{
		makerandomBINT_fix(g_bint_arr[i], length);
		makerandomBINT_fix(e_bint_arr[i], length);
		makerandomBINT_fix(m_bint_arr[i], length);
		e_bint_arr[i]->sig = POS_SIG;
		g_bint_arr[i]->sig = POS_SIG;
		m_bint_arr[i]->sig = POS_SIG;
		if ((m_bint_arr[i]->dat[0] & 1) == 0) m_bint_arr[i]->dat[0]++;
	}
	puts("make data");
	
	for (int k = 1; k <= maxk; k++)
	{
		char ss[20];

		START_WATCH

		for (int i = 0; i < testsize; i++)
		{
			bint_ltor_kary_modify_exponentiation(out_bint_arr[i][k], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i],k, NULL, NULL);
		}

		STOP_WATCH

		sprintf(ss, "kary modify k = %d", k);
		PRINT_TIME(ss);
	}

	for (int i = 0; i < testsize; i++)
	{
		for (int j = 2; j <= maxk; j++)
		{
			if (bint_compare(out_bint_arr[i][1], out_bint_arr[i][j]) != 0)
			{
				puts("error");
				for (int k = 1; k <= maxk; k++)
				{
					print("", out_bint_arr[i][k]);
				}
				return;
			}
		}
	}
	puts("no Error");
	for (int i = 0; i < testsize; i++)
	{
		delbint(g_bint_arr[i]);
		delbint(e_bint_arr[i]);
		delbint(m_bint_arr[i]);
		for(int j = 1 ; j <= maxk ; j++)
		delbint(out_bint_arr[i][j]);
	}
	free(g_bint_arr);
	free(e_bint_arr);
	free(m_bint_arr);
	for (int i = 0; i < testsize; i++)
		free(out_bint_arr[i]);
	free(out_bint_arr);
}

void slidingwindow_time()
{
	// make constant 
	const SINT testsize = 100;
	const SINT length = 32;
	const SINT maxk = 12;
	// make g
	BINT *g_bint_arr;
	g_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make e
	BINT* e_bint_arr;
	e_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make m
	BINT* m_bint_arr;
	m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));

	// make outarr
	BINT** out_bint_arr;
	out_bint_arr = (BINT**)malloc(testsize * sizeof(BINT*));
	for (int i = 0; i < testsize; i++)
	{
		out_bint_arr[i] = (BINT*)malloc((maxk+1) * sizeof(BINT));
	}


	for (int i = 0; i < testsize; i++)
	{
		makerandomBINT_fix(g_bint_arr[i], length);
		makerandomBINT_fix(e_bint_arr[i], length);
		makerandomBINT_fix(m_bint_arr[i], length);
		e_bint_arr[i]->sig = POS_SIG;
		g_bint_arr[i]->sig = POS_SIG;
		m_bint_arr[i]->sig = POS_SIG;
		if ((m_bint_arr[i]->dat[0] & 1) == 0) m_bint_arr[i]->dat[0]++;
	}
	puts("make data");
	
	for (int k = 1; k <= maxk; k++)
	{
		char ss[20];

		START_WATCH

		for (int i = 0; i < testsize; i++)
		{
			bint_slidingwindow_exponentiation(out_bint_arr[i][k], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i],k, NULL, NULL);
		}

		STOP_WATCH

		sprintf(ss, "slidingwindow k = %d", k);
		PRINT_TIME(ss);
	}

	for (int i = 0; i < testsize; i++)
	{
		for (int j = 2; j <= maxk; j++)
		{
			if (bint_compare(out_bint_arr[i][1], out_bint_arr[i][j]) != 0)
			{
				puts("error");
				for (int k = 1; k <= maxk; k++)
				{
					print("", out_bint_arr[i][k]);
				}
				return;
			}
		}
	}
	puts("no Error");
	for (int i = 0; i < testsize; i++)
	{
		delbint(g_bint_arr[i]);
		delbint(e_bint_arr[i]);
		delbint(m_bint_arr[i]);
		for(int j = 1 ; j <= maxk ; j++)
		delbint(out_bint_arr[i][j]);
	}
	free(g_bint_arr);
	free(e_bint_arr);
	free(m_bint_arr);
	for (int i = 0; i < testsize; i++)
		free(out_bint_arr[i]);
	free(out_bint_arr);
}
void montgomery_time()
{
	// make constant 
	const SINT testsize = 100;
	const SINT length = 32;
	// make g
	BINT *g_bint_arr;
	g_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make e
	BINT* e_bint_arr;
	e_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make m
	BINT* m_bint_arr;
	m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));

	// make outarr
	BINT** out_bint_arr;
	out_bint_arr = (BINT**)malloc(testsize * sizeof(BINT*));
	for (int i = 0; i < testsize; i++)
	{
		out_bint_arr[i] = (BINT*)malloc((3) * sizeof(BINT));
	}

	// use at montgomery
	BINT rm,r2m,R,R2,tmp;
	// make rm
	BINT* rm_bint_arr;
	rm_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make r2m
	BINT* r2m_bint_arr;
	r2m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
	// make mp
	UWORD* mp_arr;
	mp_arr = (UWORD*)malloc(testsize * sizeof(UWORD));

	// use at montgomery
	uword2bint(R, 0x1);
	uword2bint(R2, 0x1);
	R->sig = POS_SIG;
	bint_leftshift(R, R, BITSZ_WW * length);
	bint_leftshift(R2, R2, 2*BITSZ_WW * length);

	for (int i = 0; i < testsize; i++)
	{
		makerandomBINT_fix(g_bint_arr[i], length);
		makerandomBINT_fix(e_bint_arr[i], length);
		makerandomBINT_fix(m_bint_arr[i], length);
		e_bint_arr[i]->sig = POS_SIG;
		g_bint_arr[i]->sig = POS_SIG;
		m_bint_arr[i]->sig = POS_SIG;
		if ((m_bint_arr[i]->dat[0] & 1) == 0) m_bint_arr[i]->dat[0]++;

		// use montgomery exponentiation
		bint_div(tmp, rm_bint_arr[i], R, m_bint_arr[i]);
		delbint(tmp);
		bint_div(tmp, r2m_bint_arr[i], R2, m_bint_arr[i]);
		delbint(tmp);
		mp_arr[i] = -UWORD_inv(m_bint_arr[i]->dat[0]);

	}
	puts("make data");
	
	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_montogomery_exponentiation(out_bint_arr[i][0], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], mp_arr[i], rm_bint_arr[i], r2m_bint_arr[i], bint_montmul_redmul);
	}

	STOP_WATCH

	PRINT_TIME("montgomery reduction + multiplication");

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_montogomery_exponentiation(out_bint_arr[i][1], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], mp_arr[i], rm_bint_arr[i], r2m_bint_arr[i], bint_montmul_redmulsqr);
	}

	STOP_WATCH

	PRINT_TIME("montgomery reduction + multiplication + squaring");

	START_WATCH

	for (int i = 0; i < testsize; i++)
	{
		bint_montogomery_exponentiation(out_bint_arr[i][2], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], mp_arr[i], rm_bint_arr[i], r2m_bint_arr[i], bint_montmul_montmul);
	}

	STOP_WATCH

	PRINT_TIME("montgomery multiplication");

	for (int i = 0; i < testsize; i++)
	{
		for (int j = 1; j < 3; j++)
		{
			if (bint_compare(out_bint_arr[i][0], out_bint_arr[i][j]) != 0)
			{
				puts("error");
				for (int k = 0; k < 3; k++)
				{
					print("", out_bint_arr[i][k]);
				}
				return;
			}
		}
	}
	puts("no Error");
	for (int i = 0; i < testsize; i++)
	{
		delbint(g_bint_arr[i]);
		delbint(e_bint_arr[i]);
		delbint(m_bint_arr[i]);
		for(int j = 0 ; j < 3 ; j++)
		delbint(out_bint_arr[i][j]);
	}
	free(g_bint_arr);
	free(e_bint_arr);
	free(m_bint_arr);
	for (int i = 0; i < testsize; i++)
		free(out_bint_arr[i]);
	free(out_bint_arr);
}

void wordsize_time()
{

	// make constant 
	const SINT testsize = 100;
	SINT k = 6;
	SINT length = 30;
	SINT lengthe = 30;
	for (length = 30; length <= 90; length += 30) 
	{
		for (lengthe = 30; lengthe <= 100; lengthe += 5)
		{

			// make g
			BINT* g_bint_arr;
			g_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
			// make e
			BINT* e_bint_arr;
			e_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
			// make m
			BINT* m_bint_arr;
			m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));

			// make outarr
			BINT** out_bint_arr;
			out_bint_arr = (BINT**)malloc(testsize * sizeof(BINT*));
			for (int i = 0; i < testsize; i++)
			{
				out_bint_arr[i] = (BINT*)malloc((3) * sizeof(BINT));
			}

			// use at montgomery
			BINT rm, r2m, R, R2, tmp;
			// make rm
			BINT* rm_bint_arr;
			rm_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
			// make r2m
			BINT* r2m_bint_arr;
			r2m_bint_arr = (BINT*)malloc(testsize * sizeof(BINT));
			// make mp
			UWORD* mp_arr;
			mp_arr = (UWORD*)malloc(testsize * sizeof(UWORD));

			// use at montgomery
			uword2bint(R, 0x1);
			uword2bint(R2, 0x1);
			R->sig = POS_SIG;
			bint_leftshift(R, R, BITSZ_WW * length);
			bint_leftshift(R2, R2, 2 * BITSZ_WW * length);

			for (int i = 0; i < testsize; i++)
			{
				makerandomBINT_fix(g_bint_arr[i], length);
				makerandomBINT_fix(e_bint_arr[i], lengthe);
				makerandomBINT_fix(m_bint_arr[i], length);
				e_bint_arr[i]->sig = POS_SIG;
				g_bint_arr[i]->sig = POS_SIG;
				m_bint_arr[i]->sig = POS_SIG;
				if ((m_bint_arr[i]->dat[0] & 1) == 0) m_bint_arr[i]->dat[0]++;

				// use montgomery exponentiation
				bint_div(tmp, rm_bint_arr[i], R, m_bint_arr[i]);
				delbint(tmp);
				bint_div(tmp, r2m_bint_arr[i], R2, m_bint_arr[i]);
				delbint(tmp);
				mp_arr[i] = -UWORD_inv(m_bint_arr[i]->dat[0]);

			}
			puts("==================================================================");
			printf("g,m length = %d , e length = %d\n", length, lengthe);

			START_WATCH

				for (int i = 0; i < testsize; i++)
				{
					bint_ltor_kary_modify_exponentiation(out_bint_arr[i][0], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], k, NULL, NULL);
				}

			STOP_WATCH

				PRINT_TIME("kary_modify");
			START_WATCH

				for (int i = 0; i < testsize; i++)
				{
					bint_slidingwindow_exponentiation(out_bint_arr[i][1], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], k, NULL, NULL);
				}

			STOP_WATCH

				PRINT_TIME("slidingwindow");

			START_WATCH

				for (int i = 0; i < testsize; i++)
				{
					bint_montogomery_exponentiation(out_bint_arr[i][2], g_bint_arr[i], e_bint_arr[i], m_bint_arr[i], mp_arr[i], rm_bint_arr[i], r2m_bint_arr[i], NULL);
				}

			STOP_WATCH

				PRINT_TIME("montgomery ");

			for (int i = 0; i < testsize; i++)
			{
				delbint(g_bint_arr[i]);
				delbint(e_bint_arr[i]);
				delbint(m_bint_arr[i]);
				for (int j = 0; j < 3; j++)
					delbint(out_bint_arr[i][j]);
				delbint(rm_bint_arr[i]);
				delbint(r2m_bint_arr[i]);
			}
			free(g_bint_arr);
			free(e_bint_arr);
			free(m_bint_arr);
			free(rm_bint_arr);
			free(r2m_bint_arr);
			free(mp_arr);
			for (int i = 0; i < testsize; i++)
				free(out_bint_arr[i]);
			free(out_bint_arr);
		}
	}
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL)); /* use rand */

	//check_montgomery_reduction();
	//check_montgomery_multiplication();
	//check_mulsqr();
	//check_mulinv();
	//check_time_div_montgomeryreduction();
	//check_time_div_montgomeryreduction_montgomerymultiplicatoin();
	//onetest();
	//check_time_sqrmontred_montgomerymultiplication();
	//check_div();

	//check_exp();
	//exp_time();
	//kary_time();
	//kary_modify_time();
	//slidingwindow_time();
	//montgomery_time();
	wordsize_time();

	return 0;
}