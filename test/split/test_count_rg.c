/* The MIT License
 *
 * Copyright (c) 2014 Genome Research Limited.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Contact: Martin Pollard <mp15@sanger.ac.uk> */
/* Created by Martin Pollard on 11/03/2014. */

#include "../../bam_split.c"
#include "../test.h"
#include <unistd.h>

void setup_test_1(bam_hdr_t** hdr_in)
{
	*hdr_in = bam_hdr_init();
	const char *test1 =
	"@HD\tVN:1.4\n"
	"@SQ\tSN:blah\n"
	"@RG\tID:fish\n";
	(*hdr_in)->text = strdup(test1);
	(*hdr_in)->l_text = strlen(test1);
}

int main(int argc, char**argv)
{
	// test state
	const int NUM_TESTS = 1;
	int verbose = 0;
	int success = 0;
	int failure = 0;
	
	int getopt_char;
	while ((getopt_char = getopt(argc, argv, "v")) != -1) {
		switch (getopt_char) {
			case 'v':
				++verbose;
				break;
			default:
				printf(
					   "usage: test_count_rg [-v]\n\n"
					   " -v verbose output\n"
					   );
				break;
		}
	}
	

	// Setup stderr redirect
	size_t len = 0;
	char* res = NULL;
	FILE* orig_stderr = fdopen(dup(STDERR_FILENO), "a"); // Save stderr
	char* tempfname = (optind < argc)? argv[optind] : "test_count_rg.tmp";
	FILE* check = NULL;
	
	// setup
	if (verbose) printf("BEGIN test 1\n");  // TID test
	bam_hdr_t* hdr1;
	size_t count;
	char** output;
	setup_test_1(&hdr1);
	if (verbose > 1) {
		printf("hdr1\n");
		dump_hdr(hdr1);
	}
	if (verbose) printf("RUN test 1\n");
	
	// test
	xfreopen(tempfname, "w", stderr); // Redirect stderr to pipe
	bool result_1 = count_RG(hdr1, &count, &output);
	fclose(stderr);
	
	if (verbose) printf("END RUN test 1\n");
	if (verbose > 1) {
		printf("b\n");
		dump_hdr(hdr1);
	}
	
	// check result
	len = 0;
	check = fopen(tempfname, "r");
	if (result_1 && count == 1 && !strcmp(output[0], "fish")
		&& (getline(&res, &len, check) == -1)
		&& (feof(check) || (res && !strcmp("",res)))) {
		++success;
	} else {
		++failure;
		if (verbose) printf("FAIL test 1\n");
	}
	fclose(check);
	
	// teardown
	bam_hdr_destroy(hdr1);
	if (verbose) printf("END test 1\n");

	// Cleanup
	free(res);
	remove(tempfname);
	
	if (NUM_TESTS == success) {
		return 0;
	} else {
		fprintf(orig_stderr, "%d failures %d successes\n", failure, success);
		return 1;
	}
}
