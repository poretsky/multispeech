
typedef struct {
	char *left_context;
	regexp *lc;
	char *target;
	char *right_context;
	regexp *rc;
	char *output;
} RULE;

#define SAME 0x0001

void rule_exec(int type,int nrules, RULE *rule, char *input);
