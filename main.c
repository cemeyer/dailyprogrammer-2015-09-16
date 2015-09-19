#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <akdtree.h>

#define	CV(a)	((const void *)a)

static int
pt_cmp(unsigned dim, const akd_userdata_t *a, const akd_userdata_t *b)
{
	const double *ad = CV(a), *bd = CV(b);

	if (dim) {
		ad++;
		bd++;
	}

	if (*ad > *bd)
		return (1);
	else if (*ad == *bd)
		return (0);
	else
		return (-1);
}

static double
pt_axis_sdist(const akd_userdata_t *a, const akd_userdata_t *b, unsigned dim)
{
	const double *ad = CV(a), *bd = CV(b);
	double d;

	if (dim) {
		ad++;
		bd++;
	}

	d = (*ad - *bd);
	return (d * d);
}

static double
pt_sdist(const akd_userdata_t *a, const akd_userdata_t *b)
{

	return (pt_axis_sdist(a, b, 0) + pt_axis_sdist(a, b, 1));
}

static size_t
parse_input(double **inp)
{
	unsigned N, i;

	scanf("%u\n", &N);
	*inp = malloc(sizeof(**inp) * 2 * N);
	if (*inp == NULL)
		err(1, "malloc");
	for (i = 0; i < N; i++)
		scanf("(%lg, %lg)\n", *inp + i*2, *inp + i*2 + 1);

	return (N);
}

#if 0
int
print_cb(unsigned level, const akd_userdata_t *datum)
{
	const double *dat;
	unsigned i;

	for (i = 0; i < level; i++)
		putchar('\t');

	dat = CV(datum);
	printf("%lg, %lg\n", dat[0], dat[1]);
	return (0);
}
#endif

int
main(int argc, char **argv)
{
	const void *vnn;
	double *inputs;
	struct akd_tree *kdtree;
	struct akd_param_block apb = {
		.ap_k = 2,
		.ap_size = sizeof(double) * 2,
		.ap_cmp = pt_cmp,
		.ap_flags = 0,
		._u = {
		._double = {
			.ap_squared_dist = pt_sdist,
			.ap_axis_squared_dist = pt_axis_sdist,
		}},
	};
	double bestd = 1./0., d;
	double bestpts[4];
	size_t len, i;
	int error;

	(void)argc;
	(void)argv;

	len = parse_input(&inputs);

	error = akd_create((void *)inputs, len, &apb, &kdtree);
	if (error)
		errx(1, "akd_create: %d", error);

#if 0
	error = akd_tree_walk(kdtree, print_cb, 0);
	if (error)
		errx(1, "akd_tree_walk: %d", error);
#endif

	for (i = 0; i < len; i++) {
		vnn = akd_find_nearest_ex(kdtree, CV(&inputs[i * 2]),
		    AKD_NOT_EQUAL);
		if (vnn == NULL)
			errx(1, "akd_find_nearest");

		d = pt_sdist(vnn, CV(&inputs[i * 2]));
		if (d == 0.) {
			const double *res = vnn;
			printf("fail:akd_find_nearest %lg,%lg => %lg,%lg\n",
			    inputs[i * 2], inputs[i * 2 + 1],
			    res[0], res[1]);
		}
		if (d < bestd && d != 0.) {
			bestd = d;
			memcpy(bestpts, vnn, sizeof(double) * 2);
			memcpy(&bestpts[2], &inputs[i * 2], sizeof(double) * 2);
		}
	}

	printf("(%g,%g) (%g,%g)\n", bestpts[0], bestpts[1], bestpts[2],
	    bestpts[3]);

	return (0);
}
