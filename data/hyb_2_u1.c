/* Reference-C harness for the Yeniaras-Cenk 5-way (U1) hybrid multiplier.
   The multiplication algorithm (V1/U1 five-way split, Karatsuba, schoolbook)
   is by Yeniaras and Cenk (see refs. in the paper); only the I/O test
   harness follows the open-source Peng et al. sntrup761 reference.
   Used to count coefficient operations, not for timing. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>
#include <stdint.h>



/*************************************************
* Arguments:   struct complex a[]:        array of F_3  coefficients  for the input polynomial a(x)
*              struct complex b[]:        array of F_3  coefficients for the input polynomial b(x)
*              struct complex c[]:        array of F_3  coefficients for the multiplication polynomial c(x)=a(x).b(x)    
*              int n=761                  number of coefficients for input polynomials a(x) and b(x) 
**************************************************/

struct complex{

	int re;
	int im;
};


void copy(struct complex *source, struct complex *destination, int from, int to){

	for(int i=from, j=0; i<to; ++i, ++j)
		destination[j] = source[i];
}

void copy_same_loc(struct complex *source, struct complex *destination, int from, int to){

	for(int i=from; i<to; ++i)
		destination[i] = source[i];
}

void sum(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re + b[i].re;
		target[i].im = a[i].im + b[i].im;
	}
}

void sum_neg_neg(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = - a[i].re - b[i].re;
		target[i].im = - a[i].im - b[i].im;
	}
}

void sum_multiply_w(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = - a[i].im - b[i].im;
		target[i].im = a[i].re + b[i].re;
	}
}

void difference(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re - b[i].re;
		target[i].im = a[i].im - b[i].im;
	}
}

void multiplication(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		int ac = a[i].re * b[i].re;
		int bd = a[i].im * b[i].im;
		target[i].re = ac - bd;
		target[i].im = (a[i].re + a[i].im)*(b[i].re + b[i].im) - ac - bd;
	}
}

void negation(struct complex *a, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = -a[i].re;
		target[i].im = -a[i].im;
	}
}


void sum_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re + b[i].re;

	}
}

void sum_neg_neg_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = - a[i].re - b[i].re;

	}
}

void sum_re_im_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re + b[i].im;

	}
}

void sum_re_neg_im_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re - b[i].im;

	}
}

void sum_im_im_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].im + b[i].im;

	}
}

void sum_im_neg_im_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].im - b[i].im;

	}
}

void difference_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re - b[i].re;

	}
}

void multiplication_real(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re * b[i].re;

	}
}

void negation_real(struct complex *a, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = -a[i].re;

	}
}

void create_term(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re - b[i].im;
		target[i].im = a[i].im + b[i].re;
	}
}

void create_term_neg(struct complex *a, struct complex *b, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re + b[i].im;
		target[i].im = a[i].im - b[i].re;
	}
}

void phase_change(struct complex *a, struct complex *target, int n){

	for(int i=0; i<n; ++i){
		target[i].re = a[i].re;
		target[i].im = -a[i].im;		
	}
}

void sb_comba_real(struct complex a[], struct complex b[], struct complex c[], int n){

	int result;

	for(int i=0; i<n; ++i){
		result = 0;
		for(int j=0; j<=i; ++j)
			result += a[j].re*b[i-j].re;
		c[i].re = result;
	}

	for(int i=n; i<2*n-1; ++i){
		result = 0;
		for(int j=i-n+1; j<n; ++j)
			result += a[j].re*b[i-j].re;
		c[i].re = result;
	}

}


void sb_comba_complex(struct complex a[], struct complex b[], struct complex c[], int n){
	
	int re;
	int im;

	for(int i=0; i<n; ++i){
		re = im = 0;
		for(int j=0; j<=i; ++j){
			re += a[j].re * b[i-j].re - a[j].im * b[i-j].im;
			im += a[j].re * b[i-j].im + a[j].im * b[i-j].re;
		}
		c[i].re = re;
		c[i].im = im;
	}

	for(int i=n; i<2*n-1; ++i){
		re = im = 0;
		for(int j=i-n+1; j<n; ++j){
			re += a[j].re * b[i-j].re - a[j].im * b[i-j].im;
			im += a[j].re * b[i-j].im + a[j].im * b[i-j].re;
		}
		c[i].re = re;
		c[i].im = im;
	}

}


void ub_complex_29(struct complex a[], struct complex b[], struct complex c[], int n){
	
	#define UB_COMPLEX_MAX_AB_SIZE 15 // ceil(n/2)
	#define UB_COMPLEX_MAX_P_SIZE 29 // n


	int n0 = (n+1)/2;
	int n1 = n-n0;

	struct complex *a0 = a;
	struct complex *a1 = a+n0;
	struct complex *b0 = b;
	struct complex *b1 = b+n0;

	struct complex sum_a0_a1[UB_COMPLEX_MAX_AB_SIZE];
	struct complex sum_b0_b1[UB_COMPLEX_MAX_AB_SIZE];

	// assumed n is odd.
	sum(a0, a1, sum_a0_a1, n1);
	sum_a0_a1[n0-1] = a0[n0-1];
	sum(b0, b1, sum_b0_b1, n1);
	sum_b0_b1[n0-1] = b0[n0-1];


	struct complex p1[UB_COMPLEX_MAX_P_SIZE];
	struct complex p2[UB_COMPLEX_MAX_P_SIZE];
	struct complex p3[UB_COMPLEX_MAX_P_SIZE];
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));

	sb_comba_complex(a0, b0, p1, n0);
	sb_comba_complex(sum_a0_a1, sum_b0_b1, p2, n0);
	sb_comba_complex(a1, b1, p3, n1);


	sum(c, p1, c, 2*n0-1);
	difference(c+n0, p3, c+n0, 2*n1-1);

	for(int i=n+n1-2; i>=0 ; --i){
		c[i+n0].re -= c[i].re;
		c[i+n0].im -= c[i].im;
	}

	sum(c+n0, p2, c+n0, 2*n0-1);
	
}


void ub_real_29(struct complex a[], struct complex b[], struct complex c[], int n){
	
	#define UB_REAL_MAX_AB_SIZE 15 // ceil(n/2)
	#define UB_REAL_MAX_P_SIZE 29 // n

	/* base case */
	if(n == 1){
		multiplication_real(a, b, c, n);
		return;
	}

	int n0 = (n+1)/2;
	int n1 = n-n0;

	struct complex *a0 = a;
	struct complex *a1 = a+n0;
	struct complex *b0 = b;
	struct complex *b1 = b+n0;

	struct complex sum_a0_a1[UB_REAL_MAX_AB_SIZE];
	struct complex sum_b0_b1[UB_REAL_MAX_AB_SIZE];

	// assumed n is odd.
	sum(a0, a1, sum_a0_a1, n1);
	sum_a0_a1[n0-1] = a0[n0-1];
	sum(b0, b1, sum_b0_b1, n1);
	sum_b0_b1[n0-1] = b0[n0-1];


	struct complex p1[UB_REAL_MAX_P_SIZE];
	struct complex p2[UB_REAL_MAX_P_SIZE];
	struct complex p3[UB_REAL_MAX_P_SIZE];
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));

	sb_comba_real(a0, b0, p1, n0);
	sb_comba_real(sum_a0_a1, sum_b0_b1, p2, n0);
	sb_comba_real(a1, b1, p3, n1);


	sum_real(c, p1, c, 2*n0-1);
	difference_real(c+n0, p3, c+n0, 2*n1-1);

	for(int i=n+n1-2; i>=0 ; --i){
		c[i+n0].re -= c[i].re;
		c[i+n0].im -= c[i].im;
	}

	sum_real(c+n0, p2, c+n0, 2*n0-1);
	
}


void ub_complex_31(struct complex a[], struct complex b[], struct complex c[], int n){
	
	#define UB_COMPLEX_MAX_AB_SIZE 16 // ceil(n/2)
	#define UB_COMPLEX_MAX_P_SIZE 31 // n


	int n0 = (n+1)/2;
	int n1 = n-n0;

	struct complex *a0 = a;
	struct complex *a1 = a+n0;
	struct complex *b0 = b;
	struct complex *b1 = b+n0;

	struct complex sum_a0_a1[UB_COMPLEX_MAX_AB_SIZE];
	struct complex sum_b0_b1[UB_COMPLEX_MAX_AB_SIZE];

	// assumed n is odd.
	sum(a0, a1, sum_a0_a1, n1);
	sum_a0_a1[n0-1] = a0[n0-1];
	sum(b0, b1, sum_b0_b1, n1);
	sum_b0_b1[n0-1] = b0[n0-1];


	struct complex p1[UB_COMPLEX_MAX_P_SIZE];
	struct complex p2[UB_COMPLEX_MAX_P_SIZE];
	struct complex p3[UB_COMPLEX_MAX_P_SIZE];
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));

	sb_comba_complex(a0, b0, p1, n0);
	sb_comba_complex(sum_a0_a1, sum_b0_b1, p2, n0);
	sb_comba_complex(a1, b1, p3, n1);


	sum(c, p1, c, 2*n0-1);
	difference(c+n0, p3, c+n0, 2*n1-1);

	for(int i=n+n1-2; i>=0 ; --i){
		c[i+n0].re -= c[i].re;
		c[i+n0].im -= c[i].im;
	}

	sum(c+n0, p2, c+n0, 2*n0-1);
	
}



void ub_real_31(struct complex a[], struct complex b[], struct complex c[], int n){
	
	#define UB_REAL_MAX_AB_SIZE 16 // ceil(n/2)
	#define UB_REAL_MAX_P_SIZE 31 // n

	/* base case */
	if(n == 1){
		multiplication_real(a, b, c, n);
		return;
	}

	int n0 = (n+1)/2;
	int n1 = n-n0;

	struct complex *a0 = a;
	struct complex *a1 = a+n0;
	struct complex *b0 = b;
	struct complex *b1 = b+n0;

	struct complex sum_a0_a1[UB_REAL_MAX_AB_SIZE];
	struct complex sum_b0_b1[UB_REAL_MAX_AB_SIZE];

	// assumed n is odd.
	sum(a0, a1, sum_a0_a1, n1);
	sum_a0_a1[n0-1] = a0[n0-1];
	sum(b0, b1, sum_b0_b1, n1);
	sum_b0_b1[n0-1] = b0[n0-1];


	struct complex p1[UB_REAL_MAX_P_SIZE];
	struct complex p2[UB_REAL_MAX_P_SIZE];
	struct complex p3[UB_REAL_MAX_P_SIZE];
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));

	sb_comba_real(a0, b0, p1, n0);
	sb_comba_real(sum_a0_a1, sum_b0_b1, p2, n0);
	sb_comba_real(a1, b1, p3, n1);


	sum_real(c, p1, c, 2*n0-1);
	difference_real(c+n0, p3, c+n0, 2*n1-1);

	for(int i=n+n1-2; i>=0 ; --i){
		c[i+n0].re -= c[i].re;
		c[i+n0].im -= c[i].im;
	}

	sum_real(c+n0, p2, c+n0, 2*n0-1);
	
}



void ue_v12_real(struct complex a[], struct complex b[], struct complex c[], int n){


   // UE_V12 for k=2
	#define UE_V1_REAL_MAX_AB_SIZE 31 // (n+2)/5    
	#define UE_V1_REAL_MAX_P_SIZE 61 // 2*(n+2)/5-1
	
	#define UE_V1_REAL_MAX_AB_SIZE_LOW 29 // (n-4*2)/5
	#define UE_V1_REAL_MAX_P_SIZE_LOW  57 // 2*(n-4*2)/5-1
	/* Assumed n = 5^t-3 for some non-negative integer t */
	int n5_high = (n+2)/5;
	int n5_low = (n-8)/5; 
	int p_size = 2*n5_high-1;
	int p_size_low = 2*n5_low-1;


	struct complex *a0 = a;
	struct complex *a1 = a+n5_high;
	struct complex *a2 = a+2*n5_high;
	struct complex *a3 = a+3*n5_high;
	struct complex *a4 = a+4*n5_high;
	struct complex *b0 = b;
	struct complex *b1 = b+n5_high;
	struct complex *b2 = b+2*n5_high;
	struct complex *b3 = b+3*n5_high;
	struct complex *b4 = b+4*n5_high;

	struct complex ra0[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra1[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra2[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra3[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra4[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra5[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra6[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra7[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra8[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra9[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra10[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra11[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra12[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra13[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra14[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra15[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra16[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra17[UE_V1_REAL_MAX_AB_SIZE];

	struct complex rb0[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb1[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb2[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb3[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb4[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb5[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb6[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb7[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb8[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb9[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb10[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb11[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb12[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb13[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb14[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb15[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb16[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb17[UE_V1_REAL_MAX_AB_SIZE];


	sum(a1, a3, ra1, n5_high);

	difference(a0, a4, ra2, n5_low);
	copy_same_loc(a0, ra2, n5_low, n5_high);
	
        sum(a0, a4, ra3, n5_low);
	copy_same_loc(a0, ra3, n5_low, n5_high);
    
	difference(a1, a3, ra4, n5_high);
	difference(ra4, a2, ra5, n5_high);
	sum_neg_neg(a2, ra4, ra6, n5_high);
	difference(ra3, a2, ra7, n5_high);
	sum(ra1, ra2, ra8, n5_high);
	difference(ra2, ra1, ra9, n5_high);
	sum(ra1, ra3, ra10, n5_high);
	sum(ra10, a2, ra11, n5_high);
	create_term(ra7, ra4, ra12, n5_high);
	create_term_neg(ra7, ra4, ra13, n5_high);
	create_term(ra8, ra5, ra14, n5_high);
	create_term_neg(ra8, ra5, ra15, n5_high);
	create_term(ra9, ra6, ra16, n5_high);
	create_term_neg(ra9, ra6, ra17, n5_high);

	sum(b1, b3, rb1, n5_high);

	difference(b0, b4, rb2, n5_low);
	copy_same_loc(b0, rb2, n5_low, n5_high);

	sum(b0, b4, rb3, n5_low);
	copy_same_loc(b0, rb3, n5_low, n5_high);
	
	difference(b1, b3, rb4, n5_high);
	difference(rb4, b2, rb5, n5_high);
	sum_neg_neg(b2, rb4, rb6, n5_high);
	difference(rb3, b2, rb7, n5_high);
	sum(rb1, rb2, rb8, n5_high);
	difference(rb2, rb1, rb9, n5_high);
	sum(rb1, rb3, rb10, n5_high);
	sum(rb10, b2, rb11, n5_high);
	create_term(rb7, rb4, rb12, n5_high);
	create_term_neg(rb7, rb4, rb13, n5_high);
	create_term(rb8, rb5, rb14, n5_high);
	create_term_neg(rb8, rb5, rb15, n5_high);
	create_term(rb9, rb6, rb16, n5_high);
	create_term_neg(rb9, rb6, rb17, n5_high);


	struct complex p0[UE_V1_REAL_MAX_P_SIZE];
	struct complex p1[UE_V1_REAL_MAX_P_SIZE];
	struct complex p2[UE_V1_REAL_MAX_P_SIZE];
	struct complex p3[UE_V1_REAL_MAX_P_SIZE];
	struct complex p4[UE_V1_REAL_MAX_P_SIZE];
	struct complex p5[UE_V1_REAL_MAX_P_SIZE];
	struct complex p6[UE_V1_REAL_MAX_P_SIZE];
	struct complex p7[UE_V1_REAL_MAX_P_SIZE];
	struct complex p8[UE_V1_REAL_MAX_P_SIZE_LOW];
	memset(p0, 0, sizeof(p0));
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));
	memset(p4, 0, sizeof(p4));
	memset(p5, 0, sizeof(p5));
	memset(p6, 0, sizeof(p6));
	memset(p7, 0, sizeof(p7));
	memset(p8, 0, sizeof(p8));


	ub_real_31(a0, b0, p0, n5_high);
	ub_real_31(ra11, rb11, p1, n5_high);
	ub_complex_31(ra12, rb12, p2, n5_high);
	phase_change(p2, p3, p_size);
	ub_complex_31(ra14, rb14, p4, n5_high);
	phase_change(p4, p5, p_size);
        ub_complex_31(ra16, rb16, p6, n5_high);
	phase_change(p6, p7, p_size);
	
	ub_real_29(a4, b4, p8, n5_low);

	struct complex u1[UE_V1_REAL_MAX_P_SIZE];
	struct complex u2[UE_V1_REAL_MAX_P_SIZE];
	struct complex u3[UE_V1_REAL_MAX_P_SIZE];
	struct complex u4[UE_V1_REAL_MAX_P_SIZE];
	struct complex u5[UE_V1_REAL_MAX_P_SIZE];
	struct complex u6[UE_V1_REAL_MAX_P_SIZE];
	struct complex u7[UE_V1_REAL_MAX_P_SIZE];
	struct complex u8[UE_V1_REAL_MAX_P_SIZE];
	struct complex u9[UE_V1_REAL_MAX_P_SIZE];
	struct complex u10[UE_V1_REAL_MAX_P_SIZE];
	struct complex u11[UE_V1_REAL_MAX_P_SIZE];
	struct complex u12[UE_V1_REAL_MAX_P_SIZE];
	struct complex u13[UE_V1_REAL_MAX_P_SIZE];
	struct complex u14[UE_V1_REAL_MAX_P_SIZE]; 

	sum_real(p0, p8, u1, p_size_low);   
	copy_same_loc(p0, u1, p_size_low, p_size);  
	
	difference_real(p1, u1, u2, p_size);
	difference_real(p2, p6, u3, p_size);
	difference_real(u3, p4, u4, p_size);
	difference_real(p2, p4, u5, p_size);
	sum_real(p6, p4, u6, p_size);
	sum_im_neg_im_real(p2, p4, u7, p_size);
	sum_re_im_real(u7, p6, u8, p_size);
	sum_im_im_real(p4, p6, u9, p_size);
	sum_im_im_real(p2, p4, u10, p_size);
	sum_re_neg_im_real(u10, p6, u11, p_size);
	sum_real(u2, u3, u12, p_size);
	sum_real(u1, u4, u13, p_size);
	sum_real(u2, u5, u14, p_size);


	struct complex c1[UE_V1_REAL_MAX_P_SIZE];
	struct complex c2[UE_V1_REAL_MAX_P_SIZE];
	struct complex c3[UE_V1_REAL_MAX_P_SIZE];
	struct complex c4[UE_V1_REAL_MAX_P_SIZE];
	struct complex c5[UE_V1_REAL_MAX_P_SIZE];
	struct complex c6[UE_V1_REAL_MAX_P_SIZE];
	struct complex c7[UE_V1_REAL_MAX_P_SIZE];

	struct complex *c0 = p0;

	sum_real(u12, u8, c1, p_size);
	difference_real(u13, u9, c2, p_size);
	difference_real(u12, u8, c3, p_size);
	sum_real(u1, u6, c4, p_size);
	sum_real(u14, u11, c5, p_size);
	sum_real(u13, u9, c6, p_size);
	difference_real(u14, u11, c7, p_size);

	struct complex *c8 = p8;

	sum_real(c, c0, c, p_size);
	sum_real(c+n5_high, c1, c+n5_high, p_size);
	sum_real(c+2*n5_high, c2, c+2*n5_high, p_size);
	sum_real(c+3*n5_high, c3, c+3*n5_high, p_size);
	sum_real(c+4*n5_high, c4, c+4*n5_high, p_size);
	sum_real(c+5*n5_high, c5, c+5*n5_high, p_size);
	sum_real(c+6*n5_high, c6, c+6*n5_high, p_size);
	sum_real(c+7*n5_high, c7, c+7*n5_high, p_size);
	sum_real(c+8*n5_high, c8, c+8*n5_high, p_size-4);



}


void ue_v12_complex(struct complex a[], struct complex b[], struct complex c[], int n){

// UE_V12 for k=2
	#define UE_V1_COMPLEX_MAX_AB_SIZE 31 // (n+2)/5
	#define UE_V1_COMPLEX_MAX_P_SIZE 61 // 2*(n+2)/5-1
	
	#define UE_V1_COMPLEX_MAX_AB_SIZE_LOW  29 // (n-4*2)/5
	#define UE_V1_COMPLEX_MAX_P_SIZE_LOW  57 // 2*(n-4*2)/5-1

	/* Assumed n = 5^t-3 for some non-negative integer t */
	int n5_high = (n+2)/5;
	int n5_low = (n-8)/5; 
	int p_size = 2*n5_high-1;
	int p_size_low = 2*n5_low-1;


	struct complex *a0 = a;
	struct complex *a1 = a+n5_high;
	struct complex *a2 = a+2*n5_high;
	struct complex *a3 = a+3*n5_high;
	struct complex *a4 = a+4*n5_high;
	struct complex *b0 = b;
	struct complex *b1 = b+n5_high;
	struct complex *b2 = b+2*n5_high;
	struct complex *b3 = b+3*n5_high;
	struct complex *b4 = b+4*n5_high;

	struct complex ra0[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra1[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra2[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra3[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra4[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra5[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra6[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra7[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra8[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra9[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra10[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra11[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra12[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra13[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra14[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra15[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra16[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra17[UE_V1_COMPLEX_MAX_AB_SIZE];

	struct complex rb0[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb1[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb2[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb3[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb4[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb5[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb6[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb7[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb8[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb9[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb10[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb11[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb12[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb13[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb14[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb15[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb16[UE_V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb17[UE_V1_COMPLEX_MAX_AB_SIZE];


	sum(a1, a3, ra1, n5_high);

	difference(a0, a4, ra2, n5_low);
	copy_same_loc(a0, ra2, n5_low, n5_high);

	sum(a0, a4, ra3, n5_low);
	copy_same_loc(a0, ra3, n5_low, n5_high);

	difference(a1, a3, ra4, n5_high);
	difference(ra4, a2, ra5, n5_high);
	sum_neg_neg(a2, ra4, ra6, n5_high);
	difference(ra3, a2, ra7, n5_high);
	sum(ra1, ra2, ra8, n5_high);
	difference(ra2, ra1, ra9, n5_high);
	sum(ra1, ra3, ra10, n5_high);
	sum(ra10, a2, ra11, n5_high);
	create_term(ra7, ra4, ra12, n5_high);
	create_term_neg(ra7, ra4, ra13, n5_high);
	create_term(ra8, ra5, ra14, n5_high);
	create_term_neg(ra8, ra5, ra15, n5_high);
	create_term(ra9, ra6, ra16, n5_high);
	create_term_neg(ra9, ra6, ra17, n5_high);


	sum(b1, b3, rb1, n5_high);

	difference(b0, b4, rb2, n5_low);
	copy_same_loc(b0, rb2, n5_low, n5_high);

	sum(b0, b4, rb3, n5_low);
	copy_same_loc(b0, rb3, n5_low, n5_high);

	difference(b1, b3, rb4, n5_high);
	difference(rb4, b2, rb5, n5_high);
	sum_neg_neg(b2, rb4, rb6, n5_high);
	difference(rb3, b2, rb7, n5_high);
	sum(rb1, rb2, rb8, n5_high);
	difference(rb2, rb1, rb9, n5_high);
	sum(rb1, rb3, rb10, n5_high);
	sum(rb10, b2, rb11, n5_high);
	create_term(rb7, rb4, rb12, n5_high);
	create_term_neg(rb7, rb4, rb13, n5_high);
	create_term(rb8, rb5, rb14, n5_high);
	create_term_neg(rb8, rb5, rb15, n5_high);
	create_term(rb9, rb6, rb16, n5_high);
	create_term_neg(rb9, rb6, rb17, n5_high);


	struct complex p0[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p1[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p2[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p3[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p4[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p5[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p6[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p7[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex p8[UE_V1_COMPLEX_MAX_P_SIZE_LOW]; 
	memset(p0, 0, sizeof(p0));
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));
	memset(p4, 0, sizeof(p4));
	memset(p5, 0, sizeof(p5));
	memset(p6, 0, sizeof(p6));
	memset(p7, 0, sizeof(p7));
	memset(p8, 0, sizeof(p8));


	ub_complex_31(a0, b0, p0, n5_high);  
	ub_complex_31(ra11, rb11, p1, n5_high);
	ub_complex_31(ra12, rb12, p2, n5_high);
	ub_complex_31(ra13, rb13, p3, n5_high);
	ub_complex_31(ra14, rb14, p4, n5_high);
	ub_complex_31(ra15, rb15, p5, n5_high);
	ub_complex_31(ra16, rb16, p6, n5_high);
	ub_complex_31(ra17, rb17, p7, n5_high);
	
	ub_complex_29(a4, b4, p8, n5_low);

	struct complex u1[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u2[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u3[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u4[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u5[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u6[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u7[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u8[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u9[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u10[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u11[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u12[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u13[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u14[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u15[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u16[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u17[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u18[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u19[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u20[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex u21[UE_V1_COMPLEX_MAX_P_SIZE];

	difference(p1, p0, u1, p_size); 
	sum(p2, p3, u2, p_size);
	difference(p2, p3, u3, p_size);
	sum(p6, p7, u4, p_size);
	difference(p6, p7, u5, p_size);
	sum(p4, p5, u6, p_size);
	difference(p4, p5, u7, p_size);
	sum(u4, u6, u8, p_size);
	difference(u1, u2, u9, p_size);
	sum(u9, u4, u10, p_size);
	
	difference(u10, p8, u11, p_size); 
	copy_same_loc(u10, u11, p_size_low, p_size); 
	
	difference(u5, u7, u12, p_size);
	sum(u3, u12, u13, p_size);
	difference(p0, u2, u14, p_size);
	sum(u14, u8, u15, p_size);
	sum(u15, p8, u16, p_size);   
	copy_same_loc(u15, u16, p_size_low, p_size); 

	sum_neg_neg(u7, u5, u17, p_size);
	sum(u9, u6, u18, p_size);
	difference(u18, p8, u19, p_size);   
	copy_same_loc(u18, u19, p_size_low, p_size);
	
	difference(u3, u12, u20, p_size);
	difference(p0, u8, u21, p_size);


	struct complex c1[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex c2[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex c3[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex c4[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex c5[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex c6[UE_V1_COMPLEX_MAX_P_SIZE];
	struct complex c7[UE_V1_COMPLEX_MAX_P_SIZE];

	struct complex *c0 = p0;

	create_term(u11, u13, c1, p_size);
	create_term(u16, u17, c2, p_size);
	create_term_neg(u11, u13, c3, p_size);
	sum(u21, p8, c4, p_size);     
	copy_same_loc(u21, c4, p_size_low, p_size); 
	create_term(u19, u20, c5, p_size);
	create_term_neg(u16, u17, c6, p_size);
	create_term_neg(u19, u20, c7, p_size);

	struct complex *c8 = p8;

	sum(c, c0, c, p_size);
	sum(c+n5_high, c1, c+n5_high, p_size);
	sum(c+2*n5_high, c2, c+2*n5_high, p_size);
	sum(c+3*n5_high, c3, c+3*n5_high, p_size);
	sum(c+4*n5_high, c4, c+4*n5_high, p_size);
	
	sum(c+5*n5_high, c5, c+5*n5_high, p_size);
	sum(c+6*n5_high, c6, c+6*n5_high, p_size);
	sum(c+7*n5_high, c7, c+7*n5_high, p_size);
	sum(c+8*n5_high, c8, c+8*n5_high, p_size-4);

}

void v1_complex(struct complex a[], struct complex b[], struct complex c[], int n){

	#define V1_COMPLEX_MAX_AB_SIZE 6 // n/5
	#define V1_COMPLEX_MAX_P_SIZE 11 // 2*n/5-1


	/* Assumed n = 5^k for some non-negative integer k */
	int n5 = n/5;
	int p_size = 2*n5-1;


	struct complex *a0 = a;
	struct complex *a1 = a+n5;
	struct complex *a2 = a+2*n5;
	struct complex *a3 = a+3*n5;
	struct complex *a4 = a+4*n5;
	struct complex *b0 = b;
	struct complex *b1 = b+n5;
	struct complex *b2 = b+2*n5;
	struct complex *b3 = b+3*n5;
	struct complex *b4 = b+4*n5;

	struct complex ra0[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra1[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra2[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra3[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra4[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra5[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra6[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra7[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra8[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra9[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra10[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra11[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra12[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra13[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra14[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra15[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra16[V1_COMPLEX_MAX_AB_SIZE];
	struct complex ra17[V1_COMPLEX_MAX_AB_SIZE];

	struct complex rb0[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb1[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb2[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb3[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb4[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb5[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb6[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb7[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb8[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb9[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb10[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb11[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb12[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb13[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb14[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb15[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb16[V1_COMPLEX_MAX_AB_SIZE];
	struct complex rb17[V1_COMPLEX_MAX_AB_SIZE];


	sum(a1, a3, ra1, n5);
	difference(a0, a4, ra2, n5);
	sum(a0, a4, ra3, n5);
	difference(a1, a3, ra4, n5);
	difference(ra4, a2, ra5, n5);
	sum_neg_neg(a2, ra4, ra6, n5);
	difference(ra3, a2, ra7, n5);
	sum(ra1, ra2, ra8, n5);
	difference(ra2, ra1, ra9, n5);
	sum(ra1, ra3, ra10, n5);
	sum(ra10, a2, ra11, n5);
	create_term(ra7, ra4, ra12, n5);
	create_term_neg(ra7, ra4, ra13, n5);
	create_term(ra8, ra5, ra14, n5);
	create_term_neg(ra8, ra5, ra15, n5);
	create_term(ra9, ra6, ra16, n5);
	create_term_neg(ra9, ra6, ra17, n5);

	sum(b1, b3, rb1, n5);
	difference(b0, b4, rb2, n5);
	sum(b0, b4, rb3, n5);
	difference(b1, b3, rb4, n5);
	difference(rb4, b2, rb5, n5);
	sum_neg_neg(b2, rb4, rb6, n5);
	difference(rb3, b2, rb7, n5);
	sum(rb1, rb2, rb8, n5);
	difference(rb2, rb1, rb9, n5);
	sum(rb1, rb3, rb10, n5);
	sum(rb10, b2, rb11, n5);
	create_term(rb7, rb4, rb12, n5);
	create_term_neg(rb7, rb4, rb13, n5);
	create_term(rb8, rb5, rb14, n5);
	create_term_neg(rb8, rb5, rb15, n5);
	create_term(rb9, rb6, rb16, n5);
	create_term_neg(rb9, rb6, rb17, n5);


	struct complex p0[V1_COMPLEX_MAX_P_SIZE];
	struct complex p1[V1_COMPLEX_MAX_P_SIZE];
	struct complex p2[V1_COMPLEX_MAX_P_SIZE];
	struct complex p3[V1_COMPLEX_MAX_P_SIZE];
	struct complex p4[V1_COMPLEX_MAX_P_SIZE];
	struct complex p5[V1_COMPLEX_MAX_P_SIZE];
	struct complex p6[V1_COMPLEX_MAX_P_SIZE];
	struct complex p7[V1_COMPLEX_MAX_P_SIZE];
	struct complex p8[V1_COMPLEX_MAX_P_SIZE];
	memset(p0, 0, sizeof(p0));
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));
	memset(p4, 0, sizeof(p4));
	memset(p5, 0, sizeof(p5));
	memset(p6, 0, sizeof(p6));
	memset(p7, 0, sizeof(p7));
	memset(p8, 0, sizeof(p8));


	sb_comba_complex(a0, b0, p0, n5);
	sb_comba_complex(ra11, rb11, p1, n5);
	sb_comba_complex(ra12, rb12, p2, n5);
	sb_comba_complex(ra13, rb13, p3, n5);
	sb_comba_complex(ra14, rb14, p4, n5);
	sb_comba_complex(ra15, rb15, p5, n5);
	sb_comba_complex(ra16, rb16, p6, n5);
	sb_comba_complex(ra17, rb17, p7, n5);
	sb_comba_complex(a4, b4, p8, n5);

	struct complex u1[V1_COMPLEX_MAX_P_SIZE];
	struct complex u2[V1_COMPLEX_MAX_P_SIZE];
	struct complex u3[V1_COMPLEX_MAX_P_SIZE];
	struct complex u4[V1_COMPLEX_MAX_P_SIZE];
	struct complex u5[V1_COMPLEX_MAX_P_SIZE];
	struct complex u6[V1_COMPLEX_MAX_P_SIZE];
	struct complex u7[V1_COMPLEX_MAX_P_SIZE];
	struct complex u8[V1_COMPLEX_MAX_P_SIZE];
	struct complex u9[V1_COMPLEX_MAX_P_SIZE];
	struct complex u10[V1_COMPLEX_MAX_P_SIZE];
	struct complex u11[V1_COMPLEX_MAX_P_SIZE];
	struct complex u12[V1_COMPLEX_MAX_P_SIZE];
	struct complex u13[V1_COMPLEX_MAX_P_SIZE];
	struct complex u14[V1_COMPLEX_MAX_P_SIZE];
	struct complex u15[V1_COMPLEX_MAX_P_SIZE];
	struct complex u16[V1_COMPLEX_MAX_P_SIZE];
	struct complex u17[V1_COMPLEX_MAX_P_SIZE];
	struct complex u18[V1_COMPLEX_MAX_P_SIZE];
	struct complex u19[V1_COMPLEX_MAX_P_SIZE];
	struct complex u20[V1_COMPLEX_MAX_P_SIZE];
	struct complex u21[V1_COMPLEX_MAX_P_SIZE];

	difference(p1, p0, u1, p_size);
	sum(p2, p3, u2, p_size);
	difference(p2, p3, u3, p_size);
	sum(p6, p7, u4, p_size);
	difference(p6, p7, u5, p_size);
	sum(p4, p5, u6, p_size);
	difference(p4, p5, u7, p_size);
	sum(u4, u6, u8, p_size);
	difference(u1, u2, u9, p_size);
	sum(u9, u4, u10, p_size);
	difference(u10, p8, u11, p_size);
	difference(u5, u7, u12, p_size);
	sum(u3, u12, u13, p_size);
	difference(p0, u2, u14, p_size);
	sum(u14, u8, u15, p_size);
	sum(u15, p8, u16, p_size);
	sum_neg_neg(u7, u5, u17, p_size);
	sum(u9, u6, u18, p_size);
	difference(u18, p8, u19, p_size);
	difference(u3, u12, u20, p_size);
	difference(p0, u8, u21, p_size);


	struct complex c1[V1_COMPLEX_MAX_P_SIZE];
	struct complex c2[V1_COMPLEX_MAX_P_SIZE];
	struct complex c3[V1_COMPLEX_MAX_P_SIZE];
	struct complex c4[V1_COMPLEX_MAX_P_SIZE];
	struct complex c5[V1_COMPLEX_MAX_P_SIZE];
	struct complex c6[V1_COMPLEX_MAX_P_SIZE];
	struct complex c7[V1_COMPLEX_MAX_P_SIZE];

	struct complex *c0 = p0;

	create_term(u11, u13, c1, p_size);
	create_term(u16, u17, c2, p_size);
	create_term_neg(u11, u13, c3, p_size);
	sum(u21, p8, c4, p_size);
	create_term(u19, u20, c5, p_size);
	create_term_neg(u16, u17, c6, p_size);
	create_term_neg(u19, u20, c7, p_size);

	struct complex *c8 = p8;

	sum(c, c0, c, p_size);
	sum(c+n5, c1, c+n5, p_size);
	sum(c+2*n5, c2, c+2*n5, p_size);
	sum(c+3*n5, c3, c+3*n5, p_size);
	sum(c+4*n5, c4, c+4*n5, p_size);
	sum(c+5*n5, c5, c+5*n5, p_size);
	sum(c+6*n5, c6, c+6*n5, p_size);
	sum(c+7*n5, c7, c+7*n5, p_size);
	sum(c+8*n5, c8, c+8*n5, p_size);

}


void ka2_real(struct complex a[], struct complex b[], struct complex c[], int n){
	
	#define KA2_REAL_MAX_AB_SIZE 15 // n/2
	#define KA2_REAL_MAX_P_SIZE 29 // n-1


	int n2 = n/2;

	struct complex *a0 = a;
	struct complex *a1 = a+n2;
	struct complex *b0 = b;
	struct complex *b1 = b+n2;

	struct complex sum_a0_a1[KA2_REAL_MAX_AB_SIZE];
	struct complex sum_b0_b1[KA2_REAL_MAX_AB_SIZE];

	sum(a0, a1, sum_a0_a1, n2);
	sum(b0, b1, sum_b0_b1, n2);

	struct complex p1[KA2_REAL_MAX_P_SIZE];
	struct complex p2[KA2_REAL_MAX_P_SIZE];
	struct complex p3[KA2_REAL_MAX_P_SIZE];
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));

	sb_comba_real(a0, b0, p1, n2);
	sb_comba_real(sum_a0_a1, sum_b0_b1, p2, n2);
	sb_comba_real(a1, b1, p3, n2);


	sum_real(c, p1, c, n-1);
	difference_real(c+n2, p3, c+n2, n-1);

	for(int i=n+n2-2; i>=0 ; --i){
		c[i+n2].re -= c[i].re;
	}

	sum_real(c+n2, p2, c+n2, n-1);

}



void ue_v11_real(struct complex a[], struct complex b[], struct complex c[], int n){


   // UE_V11 for k=1
	#define UE_V1_REAL_MAX_AB_SIZE  30 // (n+1)/5    
	#define UE_V1_REAL_MAX_P_SIZE  59 // 2*(n+1)/5-1
	
	#define UE_V1_REAL_MAX_AB_SIZE_LOW 29 // (n-4*1)/5
	#define UE_V1_REAL_MAX_P_SIZE_LOW  57 // 2*(n-4*1)/5-1

	/* Assumed n = 5^t-3 for some non-negative integer t */
	int n5_high = (n+1)/5;
	int n5_low = (n-4)/5; 
	int p_size = 2*n5_high-1;
	int p_size_low = 2*n5_low-1;


	struct complex *a0 = a;
	struct complex *a1 = a+n5_high;
	struct complex *a2 = a+2*n5_high;
	struct complex *a3 = a+3*n5_high;
	struct complex *a4 = a+4*n5_high;
	struct complex *b0 = b;
	struct complex *b1 = b+n5_high;
	struct complex *b2 = b+2*n5_high;
	struct complex *b3 = b+3*n5_high;
	struct complex *b4 = b+4*n5_high;

	struct complex ra0[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra1[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra2[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra3[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra4[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra5[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra6[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra7[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra8[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra9[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra10[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra11[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra12[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra13[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra14[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra15[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra16[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra17[UE_V1_REAL_MAX_AB_SIZE];

	struct complex rb0[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb1[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb2[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb3[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb4[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb5[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb6[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb7[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb8[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb9[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb10[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb11[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb12[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb13[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb14[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb15[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb16[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb17[UE_V1_REAL_MAX_AB_SIZE];


	sum(a1, a3, ra1, n5_high);

	difference(a0, a4, ra2, n5_low);
	copy_same_loc(a0, ra2, n5_low, n5_high);
	
        sum(a0, a4, ra3, n5_low);
	copy_same_loc(a0, ra3, n5_low, n5_high);
    
	difference(a1, a3, ra4, n5_high);
	difference(ra4, a2, ra5, n5_high);
	sum_neg_neg(a2, ra4, ra6, n5_high);
	difference(ra3, a2, ra7, n5_high);
	sum(ra1, ra2, ra8, n5_high);
	difference(ra2, ra1, ra9, n5_high);
	sum(ra1, ra3, ra10, n5_high);
	sum(ra10, a2, ra11, n5_high);
	create_term(ra7, ra4, ra12, n5_high);
	create_term_neg(ra7, ra4, ra13, n5_high);
	create_term(ra8, ra5, ra14, n5_high);
	create_term_neg(ra8, ra5, ra15, n5_high);
	create_term(ra9, ra6, ra16, n5_high);
	create_term_neg(ra9, ra6, ra17, n5_high);

	sum(b1, b3, rb1, n5_high);

	difference(b0, b4, rb2, n5_low);
	copy_same_loc(b0, rb2, n5_low, n5_high);

	sum(b0, b4, rb3, n5_low);
	copy_same_loc(b0, rb3, n5_low, n5_high);
	
	difference(b1, b3, rb4, n5_high);
	difference(rb4, b2, rb5, n5_high);
	sum_neg_neg(b2, rb4, rb6, n5_high);
	difference(rb3, b2, rb7, n5_high);
	sum(rb1, rb2, rb8, n5_high);
	difference(rb2, rb1, rb9, n5_high);
	sum(rb1, rb3, rb10, n5_high);
	sum(rb10, b2, rb11, n5_high);
	create_term(rb7, rb4, rb12, n5_high);
	create_term_neg(rb7, rb4, rb13, n5_high);
	create_term(rb8, rb5, rb14, n5_high);
	create_term_neg(rb8, rb5, rb15, n5_high);
	create_term(rb9, rb6, rb16, n5_high);
	create_term_neg(rb9, rb6, rb17, n5_high);


	struct complex p0[UE_V1_REAL_MAX_P_SIZE];
	struct complex p1[UE_V1_REAL_MAX_P_SIZE];
	struct complex p2[UE_V1_REAL_MAX_P_SIZE];
	struct complex p3[UE_V1_REAL_MAX_P_SIZE];
	struct complex p4[UE_V1_REAL_MAX_P_SIZE];
	struct complex p5[UE_V1_REAL_MAX_P_SIZE];
	struct complex p6[UE_V1_REAL_MAX_P_SIZE];
	struct complex p7[UE_V1_REAL_MAX_P_SIZE];
	struct complex p8[UE_V1_REAL_MAX_P_SIZE_LOW];
	memset(p0, 0, sizeof(p0));
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));
	memset(p4, 0, sizeof(p4));
	memset(p5, 0, sizeof(p5));
	memset(p6, 0, sizeof(p6));
	memset(p7, 0, sizeof(p7));
	memset(p8, 0, sizeof(p8));


	ka2_real(a0, b0, p0, n5_high);
	ka2_real(ra11, rb11, p1, n5_high);
	v1_complex(ra12, rb12, p2, n5_high);
	phase_change(p2, p3, p_size);
	v1_complex(ra14, rb14, p4, n5_high);
	phase_change(p4, p5, p_size);
        v1_complex(ra16, rb16, p6, n5_high);
	phase_change(p6, p7, p_size);
	
	ub_real_29(a4, b4, p8, n5_low);

	struct complex u1[UE_V1_REAL_MAX_P_SIZE];
	struct complex u2[UE_V1_REAL_MAX_P_SIZE];
	struct complex u3[UE_V1_REAL_MAX_P_SIZE];
	struct complex u4[UE_V1_REAL_MAX_P_SIZE];
	struct complex u5[UE_V1_REAL_MAX_P_SIZE];
	struct complex u6[UE_V1_REAL_MAX_P_SIZE];
	struct complex u7[UE_V1_REAL_MAX_P_SIZE];
	struct complex u8[UE_V1_REAL_MAX_P_SIZE];
	struct complex u9[UE_V1_REAL_MAX_P_SIZE];
	struct complex u10[UE_V1_REAL_MAX_P_SIZE];
	struct complex u11[UE_V1_REAL_MAX_P_SIZE];
	struct complex u12[UE_V1_REAL_MAX_P_SIZE];
	struct complex u13[UE_V1_REAL_MAX_P_SIZE];
	struct complex u14[UE_V1_REAL_MAX_P_SIZE]; 

	sum_real(p0, p8, u1, p_size_low);   
	copy_same_loc(p0, u1, p_size_low, p_size);  
	
	difference_real(p1, u1, u2, p_size);
	difference_real(p2, p6, u3, p_size);
	difference_real(u3, p4, u4, p_size);
	difference_real(p2, p4, u5, p_size);
	sum_real(p6, p4, u6, p_size);
	sum_im_neg_im_real(p2, p4, u7, p_size);
	sum_re_im_real(u7, p6, u8, p_size);
	sum_im_im_real(p4, p6, u9, p_size);
	sum_im_im_real(p2, p4, u10, p_size);
	sum_re_neg_im_real(u10, p6, u11, p_size);
	sum_real(u2, u3, u12, p_size);
	sum_real(u1, u4, u13, p_size);
	sum_real(u2, u5, u14, p_size);


	struct complex c1[UE_V1_REAL_MAX_P_SIZE];
	struct complex c2[UE_V1_REAL_MAX_P_SIZE];
	struct complex c3[UE_V1_REAL_MAX_P_SIZE];
	struct complex c4[UE_V1_REAL_MAX_P_SIZE];
	struct complex c5[UE_V1_REAL_MAX_P_SIZE];
	struct complex c6[UE_V1_REAL_MAX_P_SIZE];
	struct complex c7[UE_V1_REAL_MAX_P_SIZE];

	struct complex *c0 = p0;

	sum_real(u12, u8, c1, p_size);
	difference_real(u13, u9, c2, p_size);
	difference_real(u12, u8, c3, p_size);
	sum_real(u1, u6, c4, p_size);
	sum_real(u14, u11, c5, p_size);
	sum_real(u13, u9, c6, p_size);
	difference_real(u14, u11, c7, p_size);

	struct complex *c8 = p8;

	sum_real(c, c0, c, p_size);
	sum_real(c+n5_high, c1, c+n5_high, p_size);
	sum_real(c+2*n5_high, c2, c+2*n5_high, p_size);
	sum_real(c+3*n5_high, c3, c+3*n5_high, p_size);
	sum_real(c+4*n5_high, c4, c+4*n5_high, p_size);
	sum_real(c+5*n5_high, c5, c+5*n5_high, p_size);
	sum_real(c+6*n5_high, c6, c+6*n5_high, p_size);
	sum_real(c+7*n5_high, c7, c+7*n5_high, p_size);
	sum_real(c+8*n5_high, c8, c+8*n5_high, p_size-2);

}



void ue_v14_real(struct complex a[], struct complex b[], struct complex c[], int n){


   // UE_V14 for k=4
	#define UE_V1_REAL_MAX_AB_SIZE 153 // (n+4)/5    
	#define UE_V1_REAL_MAX_P_SIZE 305 // 2*(n+4)/5-1
	
	#define UE_V1_REAL_MAX_AB_SIZE_LOW 149 // (n-4*4)/5
	#define UE_V1_REAL_MAX_P_SIZE_LOW  297 // 2*(n-4*4)/5-1

	/* Assumed n = 5^t-3 for some non-negative integer t */
	int n5_high = (n+4)/5;
	int n5_low = (n-16)/5; 
	int p_size = 2*n5_high-1;
	int p_size_low = 2*n5_low-1;


	struct complex *a0 = a;
	struct complex *a1 = a+n5_high;
	struct complex *a2 = a+2*n5_high;
	struct complex *a3 = a+3*n5_high;
	struct complex *a4 = a+4*n5_high;
	struct complex *b0 = b;
	struct complex *b1 = b+n5_high;
	struct complex *b2 = b+2*n5_high;
	struct complex *b3 = b+3*n5_high;
	struct complex *b4 = b+4*n5_high;

	struct complex ra0[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra1[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra2[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra3[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra4[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra5[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra6[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra7[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra8[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra9[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra10[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra11[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra12[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra13[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra14[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra15[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra16[UE_V1_REAL_MAX_AB_SIZE];
	struct complex ra17[UE_V1_REAL_MAX_AB_SIZE];

	struct complex rb0[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb1[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb2[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb3[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb4[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb5[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb6[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb7[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb8[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb9[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb10[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb11[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb12[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb13[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb14[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb15[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb16[UE_V1_REAL_MAX_AB_SIZE];
	struct complex rb17[UE_V1_REAL_MAX_AB_SIZE];


	sum(a1, a3, ra1, n5_high);

	difference(a0, a4, ra2, n5_low);
	copy_same_loc(a0, ra2, n5_low, n5_high);
	
        sum(a0, a4, ra3, n5_low);
	copy_same_loc(a0, ra3, n5_low, n5_high);
    
	difference(a1, a3, ra4, n5_high);
	difference(ra4, a2, ra5, n5_high);
	sum_neg_neg(a2, ra4, ra6, n5_high);
	difference(ra3, a2, ra7, n5_high);
	sum(ra1, ra2, ra8, n5_high);
	difference(ra2, ra1, ra9, n5_high);
	sum(ra1, ra3, ra10, n5_high);
	sum(ra10, a2, ra11, n5_high);
	create_term(ra7, ra4, ra12, n5_high);
	create_term_neg(ra7, ra4, ra13, n5_high);
	create_term(ra8, ra5, ra14, n5_high);
	create_term_neg(ra8, ra5, ra15, n5_high);
	create_term(ra9, ra6, ra16, n5_high);
	create_term_neg(ra9, ra6, ra17, n5_high);

	sum(b1, b3, rb1, n5_high);

	difference(b0, b4, rb2, n5_low);
	copy_same_loc(b0, rb2, n5_low, n5_high);

	sum(b0, b4, rb3, n5_low);
	copy_same_loc(b0, rb3, n5_low, n5_high);
	
	difference(b1, b3, rb4, n5_high);
	difference(rb4, b2, rb5, n5_high);
	sum_neg_neg(b2, rb4, rb6, n5_high);
	difference(rb3, b2, rb7, n5_high);
	sum(rb1, rb2, rb8, n5_high);
	difference(rb2, rb1, rb9, n5_high);
	sum(rb1, rb3, rb10, n5_high);
	sum(rb10, b2, rb11, n5_high);
	create_term(rb7, rb4, rb12, n5_high);
	create_term_neg(rb7, rb4, rb13, n5_high);
	create_term(rb8, rb5, rb14, n5_high);
	create_term_neg(rb8, rb5, rb15, n5_high);
	create_term(rb9, rb6, rb16, n5_high);
	create_term_neg(rb9, rb6, rb17, n5_high);


	struct complex p0[UE_V1_REAL_MAX_P_SIZE];
	struct complex p1[UE_V1_REAL_MAX_P_SIZE];
	struct complex p2[UE_V1_REAL_MAX_P_SIZE];
	struct complex p3[UE_V1_REAL_MAX_P_SIZE];
	struct complex p4[UE_V1_REAL_MAX_P_SIZE];
	struct complex p5[UE_V1_REAL_MAX_P_SIZE];
	struct complex p6[UE_V1_REAL_MAX_P_SIZE];
	struct complex p7[UE_V1_REAL_MAX_P_SIZE];
	struct complex p8[UE_V1_REAL_MAX_P_SIZE_LOW];
	memset(p0, 0, sizeof(p0));
	memset(p1, 0, sizeof(p1));
	memset(p2, 0, sizeof(p2));
	memset(p3, 0, sizeof(p3));
	memset(p4, 0, sizeof(p4));
	memset(p5, 0, sizeof(p5));
	memset(p6, 0, sizeof(p6));
	memset(p7, 0, sizeof(p7));
	memset(p8, 0, sizeof(p8));


	ue_v12_real(a0, b0, p0, n5_high);
	ue_v12_real(ra11, rb11, p1, n5_high);
	ue_v12_complex(ra12, rb12, p2, n5_high);
	phase_change(p2, p3, p_size);
	ue_v12_complex(ra14, rb14, p4, n5_high);
	phase_change(p4, p5, p_size);
        ue_v12_complex(ra16, rb16, p6, n5_high);
	phase_change(p6, p7, p_size);
	
	ue_v11_real(a4, b4, p8, n5_low);

	struct complex u1[UE_V1_REAL_MAX_P_SIZE];
	struct complex u2[UE_V1_REAL_MAX_P_SIZE];
	struct complex u3[UE_V1_REAL_MAX_P_SIZE];
	struct complex u4[UE_V1_REAL_MAX_P_SIZE];
	struct complex u5[UE_V1_REAL_MAX_P_SIZE];
	struct complex u6[UE_V1_REAL_MAX_P_SIZE];
	struct complex u7[UE_V1_REAL_MAX_P_SIZE];
	struct complex u8[UE_V1_REAL_MAX_P_SIZE];
	struct complex u9[UE_V1_REAL_MAX_P_SIZE];
	struct complex u10[UE_V1_REAL_MAX_P_SIZE];
	struct complex u11[UE_V1_REAL_MAX_P_SIZE];
	struct complex u12[UE_V1_REAL_MAX_P_SIZE];
	struct complex u13[UE_V1_REAL_MAX_P_SIZE];
	struct complex u14[UE_V1_REAL_MAX_P_SIZE]; 

	sum_real(p0, p8, u1, p_size_low);   
	copy_same_loc(p0, u1, p_size_low, p_size);  
	
	difference_real(p1, u1, u2, p_size);
	difference_real(p2, p6, u3, p_size);
	difference_real(u3, p4, u4, p_size);
	difference_real(p2, p4, u5, p_size);
	sum_real(p6, p4, u6, p_size);
	sum_im_neg_im_real(p2, p4, u7, p_size);
	sum_re_im_real(u7, p6, u8, p_size);
	sum_im_im_real(p4, p6, u9, p_size);
	sum_im_im_real(p2, p4, u10, p_size);
	sum_re_neg_im_real(u10, p6, u11, p_size);
	sum_real(u2, u3, u12, p_size);
	sum_real(u1, u4, u13, p_size);
	sum_real(u2, u5, u14, p_size);


	struct complex c1[UE_V1_REAL_MAX_P_SIZE];
	struct complex c2[UE_V1_REAL_MAX_P_SIZE];
	struct complex c3[UE_V1_REAL_MAX_P_SIZE];
	struct complex c4[UE_V1_REAL_MAX_P_SIZE];
	struct complex c5[UE_V1_REAL_MAX_P_SIZE];
	struct complex c6[UE_V1_REAL_MAX_P_SIZE];
	struct complex c7[UE_V1_REAL_MAX_P_SIZE];

	struct complex *c0 = p0;

	sum_real(u12, u8, c1, p_size);
	difference_real(u13, u9, c2, p_size);
	difference_real(u12, u8, c3, p_size);
	sum_real(u1, u6, c4, p_size);
	sum_real(u14, u11, c5, p_size);
	sum_real(u13, u9, c6, p_size);
	difference_real(u14, u11, c7, p_size);

	struct complex *c8 = p8;

	sum_real(c, c0, c, p_size);
	sum_real(c+n5_high, c1, c+n5_high, p_size);
	sum_real(c+2*n5_high, c2, c+2*n5_high, p_size);
	sum_real(c+3*n5_high, c3, c+3*n5_high, p_size);
	sum_real(c+4*n5_high, c4, c+4*n5_high, p_size);
	sum_real(c+5*n5_high, c5, c+5*n5_high, p_size);
	sum_real(c+6*n5_high, c6, c+6*n5_high, p_size);
	sum_real(c+7*n5_high, c7, c+7*n5_high, p_size);
	sum_real(c+8*n5_high, c8, c+8*n5_high, p_size-8);

}




int main(){


	struct complex *a = malloc(761*sizeof(struct complex));
	struct complex *b = malloc(761*sizeof(struct complex));
	struct complex *c = calloc((2*761-1), sizeof(struct complex));

        int i;
        
//reading a and b from the file inp761test
   FILE *myFile;
    myFile = fopen("inp761test", "r");
    if (myFile == NULL){
        printf("Error Reading File\n");
        exit (0);}

    for (i = 0; i < 761; i++){
        fscanf(myFile, "%d", &a[i].re );
        a[i].im = 0;
    }
    
    for (i = 0; i < 761; i++){
        fscanf(myFile, "%d", &b[i].re );
        b[i].im = 0;
    }
    fclose(myFile);
    

        unsigned long long   LastCycleCount = _rdtsc();
    	clock_t start = clock ();	     
   	for (i=0;i<99999;i++){
            ue_v14_real(a, b, c, 761);
            memset(c, 0, sizeof(struct complex)*(2*761-1));
    	   }
    	ue_v14_real(a, b, c, 761);
	unsigned long long   EndCycleCount = _rdtsc();
        unsigned long long   CyclesElapsed = EndCycleCount - LastCycleCount;
        CyclesElapsed = CyclesElapsed/100000 ;
        double Timelapsed=(clock()-start)/(double) CLOCKS_PER_SEC;
        Timelapsed=Timelapsed/100000;
        
//writing on c_ue_v1_761 file	
	FILE *outFile;
    outFile = fopen("c_ue_v1_761", "w");
    if (outFile == NULL)
	{
    	printf("Cannot Open File\n");
        exit (0);
    }
    
   for (i = 0; i < 2*761-1; i++)
        fprintf(outFile, "%d ", ((c[i].re)%3+3)%3 );
        
	
//printing on console
  for( i=0; i<2*761-1; i++)
       printf("%d ", ((c[i].re)%3+3)%3);
  printf("\n\n\n");
  printf("Cycles: %llu\n", CyclesElapsed);
  printf("Multiplication Time:");
  printf( "%f",Timelapsed);
  printf("sec");
  printf("\n \n ");

	free(a);
	free(b);
	free(c);

	return 0;
}
