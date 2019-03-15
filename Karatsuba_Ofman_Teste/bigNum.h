/**************************************************************************
**    algoritmo baseado no bignumbers do livro Art of The Contest        **
**    e Aritmética de Alta Precisão, Túlio Toffolo – www.toffolo.com.br  **
**    Marco Antônio Carvalho – marco.opt@gmail.com                       **
**************************************************************************/
#ifndef BIGNUM_H
#define BIGNUM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXDIGITOS 10000  /* comprimento máximo do nmero */
#define POSITIVO 1
#define NEGATIVO -1

typedef struct{
	char numero[MAXDIGITOS];
	//char *numero;
	int bitSinal;
	int proxDigit;
}BigNum;

//BigNum->numero = (char*)malloc(MAXDIGITOS * sizeof(char));

/** imprime **/
void imprime_BigNum(BigNum *n)
{
	int i;
	if(n->bitSinal == NEGATIVO) printf("- ");
	for(i=n->proxDigit; i>=0; --i)
		printf("%c", '0' + n->numero[i]);

	printf("\n");
}


void str_to_BigNum(char *s, BigNum *n)
{
	int i, j=0;
	n->proxDigit = -1;

	for(i=0; i<MAXDIGITOS; ++i)
        n->numero[i] = (char)0;

    if(s[0] == '-')
    {
        n->bitSinal = NEGATIVO;

        for(i=strlen(s)-1; i>=1; --i)
        {
             n->proxDigit++;
             n->numero[j++] = s[i]-'0';
        }
        n->proxDigit = strlen(s)-2;
    }
    else
    {
        n->bitSinal = POSITIVO;
        for(i=strlen(s)-1; i>=0; --i)
        {
            n->proxDigit++;
            n->numero[j++] = s[i]-'0';
        }
        n->proxDigit = strlen(s)-1;
    }
}


/** Converte de int para BigNum **/
void int_to_BigNum(int a, BigNum *n)
{
	int i, tmp;

	if(a>=0)
		n->bitSinal = POSITIVO;
	else
		n->bitSinal = NEGATIVO;

	for(i=0; i<MAXDIGITOS; ++i)
		n->numero[i] = (char)0;

	n->proxDigit = -1;

	//tmp = a*(-1);
	tmp = abs(a);

	while(tmp > 0)
	{
		n->proxDigit++;
		n->numero[n->proxDigit] = (tmp%10);
		tmp/=10;
	}

	if(a == 0)
		n->proxDigit = 0;

}

/** inicializa **/
void inicializa_BigNum(BigNum *n)
{
	int_to_BigNum(0, n);
}

/** Adição **/
void soma_BigNum(BigNum *a, BigNum *b, BigNum *c)
{
	int i, vaiUm;

	inicializa_BigNum(c);

	if(a->bitSinal == b->bitSinal)
	{
		c->bitSinal = a->bitSinal;
	}
	else if(a->bitSinal == NEGATIVO)
	{
		a->bitSinal = POSITIVO;
		subtracao_BigNum(b, a, c);
		a->bitSinal = NEGATIVO;
		return;
	}
	else
	{
		b->bitSinal = POSITIVO;
		subtracao_BigNum(a, b, c);
		b->bitSinal = NEGATIVO;
		return;
	}

	c->proxDigit = max(a->proxDigit, b->proxDigit) + 1;
	vaiUm = 0;

	for(i=0; i<=c->proxDigit; ++i)
	{
		c->numero[i] = (char)(vaiUm + a->numero[i] + b->numero[i])%10;
		vaiUm = (vaiUm + a->numero[i] + b->numero[i])/10;
	}

	ajusta_zero(c);

}

/** Bubtração **/
void subtracao_BigNum(BigNum *a, BigNum *b, BigNum *c)
{
	int i, v, empresta;

	inicializa_BigNum(c);

	if(a->bitSinal == NEGATIVO || b->bitSinal == NEGATIVO)
	{
		b->bitSinal *= -1;
		soma_BigNum(a, b, c);
		b->bitSinal *= -1;
		return;
	}
	if((compara_BigNUm(a,b) == POSITIVO))
	{
		subtracao_BigNum(b, a, c);
		c->bitSinal = NEGATIVO;
		return;
	}
	c->proxDigit = max(a->proxDigit, b->proxDigit);
	empresta = 0;

	for(i=0; i<= c->proxDigit; ++i)
	{
		v = (a->numero[i] - empresta - b->numero[i]);
		if(a->numero[i] > 0)
			empresta = 0;
		if(v < 0)
		{
			v = v+10;
			empresta = 1;
		}
		c->numero[i] = (char)v%10;
	}
	ajusta_zero(c);
}


/** Ajusta zero **/
void ajusta_zero(BigNum *n)
{
	while((n->proxDigit > 0) && (n->numero[n->proxDigit]==0))
	{
		n->proxDigit--;

		if( (n->proxDigit == 0) && (n->numero[0] == 0) )
			n->bitSinal = POSITIVO;	/* para evitar "-0" */
	}
}

/** Compara BigNum **/
int compara_BigNUm(BigNum *a, BigNum *b)
{
	int i;

	/** Compara os sinais **/
	if( (a->bitSinal == NEGATIVO) && (b->bitSinal == POSITIVO) )
		return (POSITIVO);
	if( (a->bitSinal == POSITIVO) && (b->bitSinal == NEGATIVO) )
		return (NEGATIVO);

	/** Maior Digito **/
	if(b->proxDigit > a->proxDigit)
		return (POSITIVO * a->bitSinal);
	if(a->proxDigit > b->proxDigit)
		return (NEGATIVO * a->bitSinal);

	/** Compara o digito mais significativo **/
	for(i=a->proxDigit; i>=0; --i)
	{
		if(a->numero[i] > b->numero[i])
			return (NEGATIVO * a->bitSinal);
		if(b->numero[i] > a->numero[i])
			return (POSITIVO * a->bitSinal);
	}
	return 0;
}

int max(int a, int b)
{
    if(a > b) return(a); else return(b);
}

/** multiplica n por 10^d **/
void digit_shift(BigNum *n, int d)
{
    int i;
    if(n->proxDigit == 0 && n->numero[0] == 0) return;

    for(i=n->proxDigit; i>=0; --i)
        n->numero[i+d] = n->numero[i];

    for(i=0; i<d; ++i)
        n->numero[i] = 0;

    n->proxDigit+=d;
}


/** multiplica BigNum **/
void multiplica_BigNum(BigNum *a, BigNum *b, BigNum *c)
{
    BigNum linha;
    BigNum tmp;
    int i, j;

    inicializa_BigNum(c);

    linha = *a;

    for(i=0; i<= b->proxDigit; ++i){
        for(j=1; j<=b->numero[i]; ++j){
            soma_BigNum(c, &linha, &tmp);
            *c = tmp;
        }
        digit_shift(&linha, 1);
    }
    c->bitSinal = a->bitSinal*b->bitSinal;
    ajusta_zero(c);
}


/** divisão BigNum **/
void divide_BigNum(BigNum *a, BigNum *b, BigNum *c)
{
    BigNum linha;
    BigNum tmp;
    int i, j, aSinal, bSinal;

    inicializa_BigNum(c);

    c->bitSinal = a->bitSinal * b->bitSinal;
    aSinal = a->bitSinal;
    bSinal = b->bitSinal;
    a->bitSinal = POSITIVO;
    b->bitSinal = POSITIVO;

    inicializa_BigNum(&linha);
    inicializa_BigNum(&tmp);

    c->proxDigit = a->proxDigit;

    for(i=a->proxDigit; i>=0; --i)
    {
        digit_shift(&linha, 1);
        linha.numero[0] = a->numero[i];
        c->numero[i] = 0;

        while(compara_BigNUm(&linha, b)!=POSITIVO)
        {
            c->numero[i]++;
            subtracao_BigNum(&linha, b, &tmp);
            linha = tmp;
        }
    }
    ajusta_zero(c);
    a->bitSinal = aSinal;
    b->bitSinal = bSinal;

}

/** resto BigNum **/
void resto_BigNum(BigNum *a, BigNum *b, BigNum *c)
{
    BigNum tmp1, tmp2;

    inicializa_BigNum(c);
    inicializa_BigNum(&tmp1);
    inicializa_BigNum(&tmp2);

    divide_BigNum(a, b, &tmp1);         /* R = a-b*(a/b) calc do resto */
    multiplica_BigNum(b, &tmp1, &tmp2);
    subtracao_BigNum(a, &tmp2, c);


}

/** ler Entrada **/
int ler_entrada(char *x, char *y)
{
    int Size;
    scanf ( " %256[^\n]", x);
    scanf ( " %256[^\n]", y);

    if( strlen(x) <= strlen(y))
        Size = strlen(x);
    else
        Size = strlen(y);

    return Size;
}
int fat(int n)
{
    int i, f=1;
    for(i=n; i>1; --i)
        f*= i;
    return f;
}

#endif // BIGNUM_H
