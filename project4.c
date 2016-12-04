#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>

struct point {
	int x;
	int y;
};

int distance(struct point * p1, struct point * p2){
	return (int) round(sqrt( pow(p1->x - p2->x,2) + pow(p1->y - p2->y, 2) ));
}

int get_num_lines(FILE * fd){
	int lines = 0;
	char ch;
	while(!feof(fd)){
		ch = fgetc(fd);
		if(ch == '\n'){
			lines++;
		}
	}
	return lines;
}


void set_point_array(FILE * fd, struct point * points, int numlines){
	char line[256];
	char * pch;
	memset(line, '\0', sizeof(line));
	int i = 0;
    while (fgets(line, sizeof(line), fd)) {
	    pch = strtok(line, " \r\n");
		pch = strtok(NULL, " \r\n");
		points[i].x = atoi(pch);
		pch = strtok(NULL, " \r\n");
		points[i].y = atoi(pch);
		memset(line, '\0', sizeof(line));
		//printf("x: %d, y: %d\n", points[i].x, points[i].y);
		i++;
    }
}

struct point * allocate_points(int numlines){
	struct point * points = malloc(numlines * sizeof(struct point));
	return points;
}

void delete_points(struct point * points){
	free(points);
}

int * tsp_approximation(struct point * points, struct point * to_visit, int numlines, int start){
	int * path = malloc(numlines * sizeof(int));
	int current = start;
	path[0] = current;
	// choose the path from start
	int current_dist = 0;
	int min_dist;
	int smallest_so_far;
	for(int i = 1; i < numlines; i++){
		min_dist = INT_MAX;
		for(int j = 0; j < numlines; j++){
			if(to_visit[j].x != -1 && current != j){
				current_dist = distance(&points[j], &points[current]);
				if(current_dist < min_dist){
					min_dist = current_dist;
					smallest_so_far = j;
				}
			}
		}
		path[i] = smallest_so_far;
		to_visit[current].x = -1;
		to_visit[current].y = -1;
		current = smallest_so_far;
	}
	return path;
}


void tsp_two_opt(struct point *points, int *currentPath, int numlines) {
	for(int i = 0; i < numlines; i++) {
		int p = i + 1;
		int* temp = malloc((numlines) * sizeof(int));
		for(int b = 0; b < numlines; b++)
			temp[b] = currentPath[b];
		for(int r = p + 1; r < numlines; r++) {

			int best_distance = 0;
			for(int j = 0; j < numlines - 1; j++) {
				best_distance += distance(&points[temp[j]], &points[temp[j+1]]);		
			}
			int anotherTemp = temp[i];
			temp[i] = temp[r];
			temp[r] = anotherTemp;
			
			int current_distance = 0;
			for(int j = 0; j < numlines - 1; j++) {
				current_distance += distance(&points[temp[j]], &points[temp[j+1]]);		
			}
			if(current_distance < best_distance) {
				for(int j = 0; j < numlines; j++)
					currentPath[j] = temp[j];
			}
		}
		free(temp);
	}
	
}

int get_length(struct point * points, int * path, int numlines){
	int current_length = 0;
	for (int i = 0; i < numlines-1; i++){
		current_length += distance(&points[path[i]], &points[path[i+1]]);
	}
	current_length += distance(&points[path[numlines-1]], &points[path[0]]);
	return current_length;
}

int main(int argc, char * argv[]){
	if(argc < 2){
		fprintf(stderr, "Invalid usage: project4.c <input-file>\n");
		exit(1);
	}
	struct timeval tval_before, tval_after, tval_result;
	gettimeofday(&tval_before, NULL);
	FILE * fd = fopen(argv[1], "r");
	if(fd < 0){
		fprintf(stderr, "Error opening input file\n");
		exit(1);
	}
	int numlines = get_num_lines(fd);
	fclose(fd);
	struct point * points = allocate_points(numlines);
	FILE * fp = fopen(argv[1], "r");
	if(fp < 0){
		fprintf(stderr, "Error opening input file\n");
		exit(1);
	}
	set_point_array(fp, points, numlines);
	fclose(fp);
	struct point * to_visit = allocate_points(numlines);
	for(int i = 0; i < numlines; i++){
		to_visit[i].x = points[i].x;
		to_visit[i].y = points[i].y;
	}
	int * path = tsp_approximation(points, to_visit, numlines, 0);
	int path_length = get_length(points, path, numlines);
	char new_file[100];
	memset(new_file,0, sizeof(new_file));
	strcpy(new_file, argv[1]);
	char * end = new_file + strlen(new_file);
	end += sprintf(end, "%s", ".tour");
	FILE* out_fd = fopen(new_file, "w");
	fprintf(out_fd, "%d\n", path_length);
	for(int i = 0; i < numlines; i++){
		fprintf(out_fd, "%d\n", path[i]);
	}
	fclose(out_fd);
	free(path);
	delete_points(to_visit);
	delete_points(points);
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);
	printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}
