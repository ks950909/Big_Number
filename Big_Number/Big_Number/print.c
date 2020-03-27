#include "config.h"
#include "BINT.h"

////////////////////////////////////////////////////////////////////////
//
// 함수명 : print
// 기  능 : 빅넘버 출력
// 인  자 :
//    Input  -> BINT *a
// 리턴값 : 정상 0, 비정상 -1
// 제한 사항 : 없음
//
////////////////////////////////////////////////////////////////////////
SINT print(const char nstr[], const BINT n)
{
	SINT i;

	printf("%s : ", nstr);

	if (n->sig == ZERO_SIG) {
		printf("0\n");
		return 0;
	}

	printf("(len : %d) %s", n->len, (n->sig == POS_SIG) ? "+" : "-");

	for (i = n->len - 1; i >= 0; i--)
		printf(" %0.8X", n->dat[i]);
	puts("");

	return 0;
}

SINT fprint(FILE* f, const BINT n)
{
	SINT i;

	if (n->sig == ZERO_SIG) {
		fprintf(f, "1\n0\n0\n");
		return 0;
	}

	fprintf(f, "%d\n%s\n", n->len, (n->sig == POS_SIG) ? "+" : "-");

	for (i = n->len - 1; i >= 0; i--)
	{
		//fprintf(stdout, "%8X\n", n->dat[i]);
		fprintf(f, "%X\n", n->dat[i]);
	}

	return 0;
}

SINT fscan(FILE* f, BINT n)
{
	SINT i, m;
	char x;

	fscanf(f, "%d ", &m);
	fscanf(f, "%c ", &x);
	makebint(n, m);
	if (x == '0') n->sig = 0;
	else if (x == '+') n->sig = 1;
	else n->sig = -1;
	//printf("%d\n", n->sig);

	for (i = m - 1; i >= 0; --i)
	{
		fscanf(f, " %x", &n->dat[i]);
	}

	return 0;
}