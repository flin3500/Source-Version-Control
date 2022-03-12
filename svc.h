#ifndef svc_h
#define svc_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>


typedef struct resolution {
    // NOTE: DO NOT MODIFY THIS STRUCT
    char *file_name;
    char *resolved_file;
} resolution;

typedef struct helperThing{
	char* HEAD;
	char** all_file;
	int all_num;
	char** change_file;
	int change_num;
	char** add_file;
	int add_num;
	char** rm_file;
	int rm_num;
	char** mod_file;
	int mod_num;
	struct commitThing **all_commit;
	char* commit_id;
	char** merge_list;
	struct branchThing* cur_branch;
	struct branchThing** all_branch;
	int branch_num;
	int if_merge;
	int commit_num;
	int prev;
	int merge_num;
} helperThing;

typedef struct branchThing{
	char* branch_name;
	struct commitThing* point;
}branchThing;

typedef struct commitThing{
	char* commit_id;
	char* commit_message;
	char* commit_branch;
	char** prev_commits;
	char** file;
	char** add;
	char** rm;
	char** mod;
	char** change;
	char** all;
	int** all_hash;
	int prev_c;
	int add_c;
	int rm_c;
	int mod_c;
	int change_c;
	int all_c;
} commitThing;

void *svc_init(void);

void cleanup(helperThing *helper);

int hash_file(helperThing *helper, char *file_path);

char *svc_commit(helperThing *helper, char *message);

commitThing *get_commit(helperThing *helper, char *commit_id);

char **get_prev_commits(helperThing *helper, commitThing *commit, int *n_prev);

void print_commit(helperThing *helper, char *commit_id);

int svc_branch(helperThing *helper, char *branch_name);

int svc_checkout(helperThing *helper, char *branch_name);

char **list_branches(helperThing *helper, int *n_branches);

int svc_add(helperThing *helper, char *file_name);

int svc_rm(helperThing *helper, char *file_name);

int svc_reset(helperThing *helper, char *commit_id);

char *svc_merge(helperThing *helper, char *branch_name, resolution *resolutions, int n_resolutions);

#endif
