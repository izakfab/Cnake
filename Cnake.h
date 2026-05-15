#ifndef CNAKE_H

#define CNAKE_H

typedef struct _snake_body {
	int x;
	int y;
	char *oblika;
	struct _snake_body *next;
} snake_body;

typedef struct _apple {
	int x;
	int y;
} apple;



#endif
