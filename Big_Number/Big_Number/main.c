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
	BINT a, b, c, d;
	UWORD a_arr[11] = { 0xf,0x01 };
	UWORD b_arr[11] = { 0xfffffff,0x00000001 };
	a->len = 1; b->len = 1;
	uwordarr2bint(a, a_arr, a->len, -1);
	uwordarr2bint(b, b_arr, b->len, 1);
	//bint_unsigned_makelen(a);
	//bint_unsigned_makelen(b);

	bint_div(c, d, a, b);

	print("a", a);
	print("b", b);

	puts("");
	puts("a = b * c + d");
	puts("");

	print("c", c);
	puts("");
	print("d", d);
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
		//print("m", m);

		uword2bint(R, 1u);
		bint_leftshift(R, R, BITSZ_WW * n);
		//print("R", R);
		//printf("n : %d\n", n);

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

void check_time_div_montogomeryreduction()
{
	BINT T_bint_arr[1000];
	BINT m_bint_arr[1000];
	for (int i = 0; i < 1000; i++)
	{
	}
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL)); /* use rand */
	check_montgomery_reduction();
	//check_montgomery_multiplication();
	//check_mulsqr();
	//check_mulinv();
	return 0;
}