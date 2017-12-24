#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Feel free to edit the line length 
// and create an outragious triangle
#define MAXLINE 512
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"

struct mst_node {
	int sum;
	unsigned char left;
	unsigned char right;
};

int triangle_height;
int **triangle_ptr;
struct mst_node *triangle_mst;

/* Reads in a triangle formatted correctly from fp. Sets the global variables 
 * triangle_height and triangle_ptr appropriately.
 */
void read_triangle(FILE *fp){
    char cur_char;
    char read_buf[MAXLINE];
    char *int_str;
    int i, j;
    int num_lines = 0;
    int nonempty_flag = 0;

    // Compute the line count
    while ((cur_char = getc(fp)) != EOF){

        if (nonempty_flag == 0){
            ++nonempty_flag;
        }

        if (cur_char == '\n'){
            ++num_lines;
        }
    }
    if (nonempty_flag){
        ++num_lines;
    }
    triangle_height = num_lines;

    // Construct the array...
    triangle_ptr = calloc(triangle_height, sizeof(int *));
    i = 0;

    while (i < triangle_height){

        // Note the i + 1 is due to the layout of the 
        // expected triangle
        triangle_ptr[i] = calloc(i + 1, sizeof(int));
        ++i;
    }
    fseek(fp, 0, SEEK_SET);
    i = 0;
    j = 0;

    // Read in triangle values
    while (fgets(read_buf, MAXLINE, fp) != NULL){
        
        int_str = strtok(read_buf, " ");
        while (int_str != NULL){

        	triangle_ptr[i][j] = strtol(int_str, NULL, 10);
        	int_str = strtok(NULL, " ");
        	++j;
        }
        ++i;
        j = 0;
    }
    return;
}


/* Runs a simple variant of Prim's Minimum Spanning Tree algorithm.
 * Sets triangle_mst to an array signifying the maximum spanning tree 
 * as follows: Each triangle node gets 3 array elements. The first
 * signifies the maximal sum along the path reaching the node, the following
 * two represent the existence of an edge in the MST from this node to it's left
 * and right parents respectively.
 */
void prims_MST(){
	int i, j;
	
	// The calculation here is just Gauss' Trick
	int mst_list_size = (triangle_height * (triangle_height + 1)) / 2;
	triangle_mst = calloc(mst_list_size, sizeof(struct mst_node));

	int element_row_iterator = 0;
	i = 0;
	j = 0;

	while (element_row_iterator < mst_list_size){

		while (j < i + 1){

			if (i > 0){

				// Determine which parent will yield the maximal sum
				if (j == 0) {

					triangle_mst[element_row_iterator + j].right = 1;
					triangle_mst[element_row_iterator + j].sum = 
									triangle_mst[element_row_iterator - i + j].sum
									+ triangle_ptr[i][j];

				} else if (j == i){

					triangle_mst[element_row_iterator + j].left = 1;
					triangle_mst[element_row_iterator + j].sum = 
									triangle_mst[element_row_iterator - i + j - 1].sum
									+ triangle_ptr[i][j];

				} else if (triangle_mst[element_row_iterator - i + j].sum
						   > triangle_mst[element_row_iterator - i + j - 1].sum){
					
					triangle_mst[element_row_iterator + j].right = 1;
					triangle_mst[element_row_iterator + j].sum = 
									triangle_mst[element_row_iterator - i + j].sum
									+ triangle_ptr[i][j];

				} else {

					triangle_mst[element_row_iterator + j].left = 1;
					triangle_mst[element_row_iterator + j].sum = 
									triangle_mst[element_row_iterator - i + j - 1].sum
									+ triangle_ptr[i][j];
				}
			} else {
				triangle_mst[0].sum = **triangle_ptr;
			}
			++j;
		}
		++i;
		element_row_iterator = (i * (i + 1)) / 2;
		j = 0;
	}
	return;
}

int main(int argc, char **argv){

	if (argc > 1){

		FILE *fp;
		if ((fp = fopen(argv[1], "r")) == NULL){
			printf("Invalid Triangle file.\n");
			return 0;
		}
		read_triangle(fp);
		prims_MST();
		int j = 0;
		int i = 0;
		int cur_max_index = 0;
		int cur_max = triangle_mst[(triangle_height * (triangle_height - 1)) / 2].sum;
		int cur_max_contender;

		int *max_path[triangle_height];
		int *cur_path_node;

		// Determine the maximal sum
		while (j < triangle_height){

			cur_max_contender = triangle_mst[((triangle_height * (triangle_height - 1)) / 2) 
							  				  + j].sum;

			if (cur_max_contender > cur_max){

				cur_max_index = j;
				cur_max = triangle_mst[((triangle_height * (triangle_height - 1)) / 2) 
										+ cur_max_index].sum;
			}
			++j;
		}
		i = triangle_height - 1;
		j = cur_max_index;
		cur_path_node = triangle_ptr[i] + j;
		max_path[i] = cur_path_node;
		
		while (i > 0){

			if (triangle_mst[((i * (i + 1)) / 2) + j].left){
				j = j - 1;
			}
			--i;
			cur_path_node = triangle_ptr[i] + j;
			max_path[i] = cur_path_node;
		}
		i = 0;

		// Print off triangle with fancy terminal colours
		while (i < triangle_height){

			printf("%*s", 3 * (triangle_height - i - 1), "");
			j = 0;

			while (j <= i){

				if (j > 0){
					printf("   ");
				}
				if (triangle_ptr[i] + j == max_path[i]){
					printf("%s%3d", KRED, triangle_ptr[i][j]);
				
	 			} else {
					printf("%s%3d", KGRN, triangle_ptr[i][j]);
				}
				++j;
			}
			printf("\n");
			++i;
		}
		printf("%sThe maximal path sum is %d\n", KNRM, cur_max);
		return 1;

	} else {
		printf("Please specify a text file containing a correctly formatted triangle.\n");
		return 0;
	}
}