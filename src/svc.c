#include "svc.h"



int contain(int num_in_list,char** list,char* need_to_find){
    for (int i = 0; i < num_in_list; i++)
    {
        if(strcmp(list[i],need_to_find)==0){
            return 1;
        }
    }
    return 0;
}

void check_manual(helperThing *helper,char** array,int array_length){
    for (int i = 0; i < array_length; i++)
    {
        if(hash_file(helper,array[i])==-2)
        {
            for (int j = i; j < array_length-1; j++)
            {
                strcpy(array[j],array[j+1]);
            }
            free(array[array_length-1]);
            array=(char **)realloc(array,(array_length-1)*sizeof(char*));
            array_length-=1;
        }
    }
}

void reset(helperThing *helper){
        for (int i = 0; i < helper->add_num; i++)
        {
            free(helper->add_file[i]);
        }
        free(helper->add_file);
        helper->add_file=NULL;

        for (int i = 0; i < helper->rm_num; i++)
        {
            free(helper->rm_file[i]);
        }
        free(helper->rm_file);
        helper->rm_file=NULL;

        for (int i = 0; i < helper->mod_num; i++)
        {
            free(helper->mod_file[i]);
        }
        free(helper->mod_file);
        helper->mod_file=NULL;

        for (int i = 0; i < helper->change_num; i++)
        {
            free(helper->change_file[i]);
        }
        free(helper->change_file);
        helper->change_file=NULL;

        helper->add_num=0;

        helper->add_file=(char **)malloc(sizeof(char*));

        helper->rm_num=0;

        helper->rm_file=(char **)malloc(sizeof(char*));

        helper->mod_num=0;

        helper->mod_file=(char **)malloc(sizeof(char*));

        helper->change_num=0;

        helper->change_file=(char **)malloc(sizeof(char*));
}

void reset_all(helperThing *helper){
    for (int i = 0; i < helper->all_num; i++)
    {
        free(helper->all_file[i]);
    }
    free(helper->all_file);

    helper->all_file=NULL;

    helper->all_num=0;

    helper->all_file=(char **)malloc(sizeof(char*));
}


void sort(char** array,int length){
    for (int i = 0; i < length-1; i++)
    {
        for (int j = 0; j < length-1; j++)
        {
            
            if (strcasecmp(array[j],array[j + 1]) > 0) {
                char *tmp= array[j];     
                array[j]= array[j + 1];          
                array[j + 1]= tmp;
            }

        }
    }
}

void check_modify(helperThing *helper){
    // find index od commit_id
    int j;
    for (j = 0; j < helper->commit_num; j++)
    {
        if (strcmp(helper->all_commit[j]->commit_id,helper->commit_id)==0)
        {
            break;
        }
    }

    for (int i = 0; i < helper->all_commit[j]->all_c; i++)
    {
        for (int k = 0; k < helper->all_num; k++)
        {
            if(strcmp(helper->all_file[k],helper->all_commit[j]->all[i])==0){
                if(hash_file(helper,helper->all_file[k])!=*helper->all_commit[j]->all_hash[i]){
                    // jusitify if already in change
                    if(!contain(helper->change_num,helper->change_file,helper->all_file[k])){
                        helper->change_file=(char**)realloc(helper->change_file,(helper->change_num+1)*sizeof(char*));
                        helper->change_file[helper->change_num]=(char *)malloc(sizeof(char)*strlen(helper->all_file[k])+2);
                        strcpy(helper->change_file[helper->change_num], helper->all_file[k]);
                        helper->change_num+=1;
                    }

                    // add to mod
                    if(!contain(helper->mod_num,helper->mod_file,helper->all_file[k])){
                        helper->mod_file=(char**)realloc(helper->mod_file,(helper->mod_num+1)*sizeof(char*));
                        helper->mod_file[helper->mod_num]=(char *)malloc(sizeof(char)*strlen(helper->all_file[k])+2);
                        strcpy(helper->mod_file[helper->mod_num],helper->all_file[k]);
                        helper->mod_num+=1;
                    }

                }
            }
        }
    }
}


char* read_and_write(char* filename){
    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen (filename, "rb" );
    if( !fp ) perror(filename),exit(1);

    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    /* allocate memory for entire content */
    buffer = calloc( 1, lSize+1 );
    if( !buffer ){
        fclose(fp);
        fputs("memory alloc fails",stderr);
        exit(1);
    }

    /* copy the file into the buffer */
    fread( buffer , lSize, 1 , fp);

    /* do your work here, buffer is a string contains the whole text */

    fclose(fp);
    return buffer;    
}

void set_commit(helperThing *helper,char *message){
        helper->all_commit=(struct commitThing **)realloc(helper->all_commit,(helper->commit_num+1)*sizeof(struct commitThing*));

        helper->all_commit[helper->commit_num]=(struct commitThing *)malloc(1*sizeof(struct commitThing));
        
        helper->cur_branch->point=helper->all_commit[helper->commit_num];
        // message deep copy
        helper->all_commit[helper->commit_num]->commit_message=(char *)malloc(strlen(message)*sizeof(char)+2);
        strcpy(helper->all_commit[helper->commit_num]->commit_message,message);

        // commit branch deep copy
        helper->all_commit[helper->commit_num]->commit_branch=(char *)malloc(strlen(helper->HEAD)*sizeof(char)+2);
        strcpy(helper->all_commit[helper->commit_num]->commit_branch,helper->HEAD);

        // add deep copy
        helper->all_commit[helper->commit_num]->add=(char **)malloc(helper->add_num*sizeof(char*));
        for (int i = 0; i < helper->add_num; i++)
        {
            helper->all_commit[helper->commit_num]->add[i]=(char *)malloc(strlen(helper->add_file[i])*sizeof(char)+2);
            strcpy(helper->all_commit[helper->commit_num]->add[i],helper->add_file[i]);
        }
        // rm deep copy
        helper->all_commit[helper->commit_num]->rm=(char **)malloc(helper->rm_num*sizeof(char*));
        for (int i = 0; i < helper->rm_num; i++)
        {
            helper->all_commit[helper->commit_num]->rm[i]=(char *)malloc(strlen(helper->rm_file[i])*sizeof(char)+2);
            strcpy(helper->all_commit[helper->commit_num]->rm[i],helper->rm_file[i]);
        }
        // mod deep copy
        helper->all_commit[helper->commit_num]->mod=(char **)malloc(helper->mod_num*sizeof(char*));
        for (int i = 0; i < helper->mod_num; i++)
        {
            helper->all_commit[helper->commit_num]->mod[i]=(char *)malloc(strlen(helper->mod_file[i])*sizeof(char)+2);
            strcpy(helper->all_commit[helper->commit_num]->mod[i],helper->mod_file[i]);
        }
        // change deep copy
        helper->all_commit[helper->commit_num]->change=(char **)malloc(helper->change_num*sizeof(char*));
        for (int i = 0; i < helper->change_num; i++)
        {
            helper->all_commit[helper->commit_num]->change[i]=(char *)malloc(strlen(helper->change_file[i])*sizeof(char)+2);
            strcpy(helper->all_commit[helper->commit_num]->change[i],helper->change_file[i]);
        }
        // all deep copy
        helper->all_commit[helper->commit_num]->all=(char **)malloc(helper->all_num*sizeof(char*));
        for (int i = 0; i < helper->all_num; i++)
        {
            helper->all_commit[helper->commit_num]->all[i]=(char *)malloc(strlen(helper->all_file[i])*sizeof(char)+2);
            strcpy(helper->all_commit[helper->commit_num]->all[i],helper->all_file[i]);
        }

        // all hash deep copy
        helper->all_commit[helper->commit_num]->all_hash=(int **)malloc(helper->all_num*sizeof(int*));
        for (int i = 0; i < helper->all_num; i++)
        {
            helper->all_commit[helper->commit_num]->all_hash[i]=(int *)malloc(10*sizeof(int));
            *helper->all_commit[helper->commit_num]->all_hash[i]=hash_file(helper,helper->all_file[i]);
        }

        // prev commit deep copy
        if(helper->if_merge){
            helper->all_commit[helper->commit_num]->prev_commits=(char **)malloc(helper->prev*sizeof(char*));
            for (int i = 0; i < helper->prev; i++)
            {
                helper->all_commit[helper->commit_num]->prev_commits[i]=(char *)malloc(strlen(helper->commit_id)*sizeof(char)+2);
                strcpy(helper->all_commit[helper->commit_num]->prev_commits[i],helper->merge_list[i]);
            }
        }else{
            helper->all_commit[helper->commit_num]->prev_commits=(char **)malloc(helper->prev*sizeof(char*));
            for (int i = 0; i < helper->prev; i++)
            {
                helper->all_commit[helper->commit_num]->prev_commits[i]=(char *)malloc(strlen(helper->commit_id)*sizeof(char)+2);
                strcpy(helper->all_commit[helper->commit_num]->prev_commits[i],helper->commit_id);
            }
        }
        //commit list
        helper->all_commit[helper->commit_num]->file=(char **)malloc(helper->all_num*sizeof(char*));
        for (int i = 0; i < helper->all_num; i++)
        {
            char* buffer=read_and_write(helper->all_file[i]);
            helper->all_commit[helper->commit_num]->file[i]=(char *)malloc((strlen(buffer))*sizeof(char)+2);
            strcpy(helper->all_commit[helper->commit_num]->file[i],buffer);
            free(buffer);
        }
        

        helper->all_commit[helper->commit_num]->add_c=helper->add_num;
        helper->all_commit[helper->commit_num]->rm_c=helper->rm_num;
        helper->all_commit[helper->commit_num]->mod_c=helper->mod_num;
        helper->all_commit[helper->commit_num]->change_c=helper->change_num;
        helper->all_commit[helper->commit_num]->all_c=helper->all_num;
        helper->all_commit[helper->commit_num]->prev_c=helper->prev;
}


int regex(char* expression,char* branch_name){
    regex_t regex;
    int i;

    //Compile regular expression
    i = regcomp(&regex, expression, 0);
    if (i) {
        fprintf(stderr, "can not regex\n");
        exit(1);
    }

    //Execute regular expression
    i = regexec(&regex, branch_name, 0, NULL, 0);
    if (!i) {
        //Free memory allocated to the pattern buffer by regcomp()
        regfree(&regex);
        return 1;
    }
    else{
        //Free memory allocated to the pattern buffer by regcomp()
        regfree(&regex);
        return 0;
    }   
}

int containbranch(int num_in_list,struct branchThing** list,char* need_to_find){
    for (int i = 0; i < num_in_list; i++)
    {
        if(strcmp(list[i]->branch_name,need_to_find)==0){
            return 1;
        }
    }
    return 0;
}

void *svc_init(void) {
    helperThing *helper=(helperThing *)malloc(sizeof(helperThing));
    //head
    helper->HEAD=(char*)malloc(50*sizeof(char));
    strcpy(helper->HEAD,"master");
    //branch
    helper->branch_num=0;
    helper->all_branch=(struct branchThing **)malloc((helper->branch_num+1)*sizeof(struct branchThing*));
    helper->all_branch[helper->branch_num]=(struct branchThing*)malloc(1*sizeof(struct branchThing));
    helper->all_branch[helper->branch_num]->branch_name=(char *)malloc(8*sizeof(char));
    strcpy(helper->all_branch[helper->branch_num]->branch_name,helper->HEAD);
    helper->all_branch[helper->branch_num]->point=NULL;
    //current branch
    helper->cur_branch=helper->all_branch[helper->branch_num];
    helper->branch_num+=1;
    
    //file
    helper->add_num=0;
    helper->add_file=(char **)malloc(sizeof(char*));

    helper->rm_num=0;
    helper->rm_file=(char **)malloc(sizeof(char*));

    helper->mod_num=0;
    helper->mod_file=(char **)malloc(sizeof(char*));

    helper->change_num=0;
    helper->change_file=(char **)malloc(sizeof(char*));

    helper->all_num=0;
    helper->all_file=(char **)malloc(sizeof(char*));
    //commit
    helper->commit_num=0;
    helper->all_commit=(struct commitThing **)malloc(sizeof(struct commitThing*));
    helper->commit_id=malloc(sizeof(char)*8);

    //merge
    helper->if_merge=0;
    helper->merge_list=(char **)malloc(sizeof(char*));
    helper->merge_num=0;

    return helper;
}

void cleanup(helperThing *helper) {

    for (int i = 0; i < helper->merge_num; i++)
    {
        free(helper->merge_list[i]);
    }
    free(helper->merge_list);

    for (int i = 0; i < helper->branch_num; i++)
    {
        free(helper->all_branch[i]->branch_name);
        free(helper->all_branch[i]);
    }
    free(helper->all_branch);

    for (int i = 0; i < helper->change_num; i++)
    {
        free(helper->change_file[i]);
    }
    free(helper->change_file);

    for (int i = 0; i < helper->all_num; i++)
    {
        free(helper->all_file[i]);
    }
    free(helper->all_file);

    for (int i = 0; i < helper->add_num; i++)
    {
        free(helper->add_file[i]);
    }
    free(helper->add_file);

    for (int i = 0; i < helper->rm_num; i++)
    {
        free(helper->rm_file[i]);
    }
    free(helper->rm_file);

    for (int i = 0; i < helper->mod_num; i++)
    {
        free(helper->mod_file[i]);
    }
    free(helper->mod_file);

    for (int i = 0; i < helper->commit_num; i++)
    {
        for (int j = 0; j < helper->all_commit[i]->add_c; j++)
        {
            free(helper->all_commit[i]->add[j]);
        }

        for (int j = 0; j < helper->all_commit[i]->rm_c; j++)
        {
            free(helper->all_commit[i]->rm[j]);
        }

        for (int j = 0; j < helper->all_commit[i]->mod_c; j++)
        {
            free(helper->all_commit[i]->mod[j]);
        }

        for (int j = 0; j < helper->all_commit[i]->change_c; j++)
        {
            free(helper->all_commit[i]->change[j]);
        }
        
        for (int j = 0; j < helper->all_commit[i]->all_c; j++)
        {
            free(helper->all_commit[i]->all[j]);
        }

        for (int j = 0; j < helper->all_commit[i]->all_c; j++)
        {
            free(helper->all_commit[i]->all_hash[j]);
        }
        for (int j = 0; j < helper->all_commit[i]->prev_c; j++)
        {
            free(helper->all_commit[i]->prev_commits[j]);
        }

        for (int j = 0; j < helper->all_commit[i]->all_c; j++)
        {
            free(helper->all_commit[i]->file[j]);
        }
        

        free(helper->all_commit[i]->file);
        free(helper->all_commit[i]->commit_id);
        free(helper->all_commit[i]->commit_message);
        free(helper->all_commit[i]->commit_branch);
        free(helper->all_commit[i]->add);
        free(helper->all_commit[i]->rm);
        free(helper->all_commit[i]->mod);
        free(helper->all_commit[i]->change);
        free(helper->all_commit[i]->all);
        free(helper->all_commit[i]->all_hash);
        free(helper->all_commit[i]->prev_commits);
        free(helper->all_commit[i]);
    }
    free(helper->all_commit);

    free(helper->commit_id);
    free(helper->HEAD);
    free(helper);
}

int hash_file(helperThing *helper, char *file_path) {
    if(!file_path){
        return -1;
    }
    FILE *fp=fopen(file_path,"rb");
    if(!fp){
        return -2;
    }
    int hash = 0;
    //add file name
    char *nn = file_path;
    while(*nn!='\0'){
        hash=(hash+(unsigned int)(*nn))%1000;
        nn++;
    }
    
    //add file content
    int c = getc(fp);
    while (c != EOF) {
        hash=(hash+c)%2000000000;
        c = getc(fp);
    }
    fclose(fp);
    
    return hash;
}

char *svc_commit(helperThing *helper, char *message) {
    if(!message||strcmp(message,"No changes")==0){return NULL;}
    
    int id=0;
    char *nn = message;
    while(*nn!='\0'){
        id=(id+(unsigned int)(*nn))%1000;
        nn++;
    }
    for (int i = 0; i < helper->all_num; i++)
    {
        if(hash_file(helper,helper->all_file[i])==-2){
            svc_rm(helper,helper->all_file[i]);
        }
    }
    
    //The first commit is a special one
    if(helper->commit_num==0){
        sort(helper->change_file,helper->change_num);
        for (int i = 0; i < helper->change_num;i++)
        {
            char* cur_file=helper->change_file[i];
            if(contain(helper->add_num,helper->add_file,cur_file)){
                id=id+376591;
                char *nn = cur_file;
                while(*nn!='\0'){
                    id=(id*(((unsigned int)(*nn))%37))%15485863+1;
                    nn++;
                }
            }   
        }
        helper->prev=0;
        set_commit(helper,message);
        sprintf(helper->commit_id,"%06x",id);
        // id deep copy
        helper->all_commit[helper->commit_num]->commit_id=(char *)malloc(8*sizeof(char));
        strcpy(helper->all_commit[helper->commit_num]->commit_id,helper->commit_id);
        // increase the num
        helper->commit_num+=1;
        reset(helper);
        return helper->commit_id;
    }
    

    check_modify(helper);

    sort(helper->change_file,helper->change_num);

    if(helper->change_num==0){return NULL;}

    for (int i = 0; i < helper->change_num; i++)
    {
        char* cur_file=helper->change_file[i];
        if(contain(helper->add_num,helper->add_file,cur_file)){
            
            id=id+376591;
        }
        if(contain(helper->rm_num,helper->rm_file,cur_file)){
            
            id=id+85973;
        }
        if(contain(helper->mod_num,helper->mod_file,cur_file)){

            id=id+9573681;
        }
        char *nn = cur_file;
        while(*nn!='\0'){
            id=(id*(((unsigned int)(*nn))%37))%15485863+1;
            nn++;
        }
    }
    if(helper->if_merge){
        helper->prev=2;
    }else{
        helper->prev=1;
    }
    set_commit(helper,message);
    sprintf(helper->commit_id,"%06x",id);
    // id deep copy
    helper->all_commit[helper->commit_num]->commit_id=(char *)malloc(8*sizeof(char));
    strcpy(helper->all_commit[helper->commit_num]->commit_id,helper->commit_id);
    // increase the num
    helper->commit_num+=1;
    reset(helper);
    return helper->commit_id;
}

commitThing *get_commit(helperThing *helper, char *commit_id) {
    if(!commit_id){return NULL;}
    for (int i = 0; i < helper->commit_num; i++)
    {
        if (strcmp(helper->all_commit[i]->commit_id,commit_id)==0)
        {
            return helper->all_commit[i];
        }
    }
    return NULL;
}

char **get_prev_commits(helperThing *helper, commitThing *commit, int *n_prev) {
    if(!n_prev){return NULL;}
    if (!commit){*n_prev=0;return NULL;}
    for (int i = 0; i < helper->commit_num; i++)
    {
        if (strcmp(helper->all_commit[i]->commit_id,commit->commit_id)==0&&i==0)
        {
            *n_prev=0;
            return NULL;
        }
    }
    char** list=(char **)malloc(helper->commit_num*sizeof(char*));
    for (int i = 0; i < commit->prev_c; i++)
    {
        list[i]=commit->prev_commits[i];
        *n_prev=i+1;
    }
    return list;
}

void print_commit(helperThing *helper, char *commit_id) {
    if(!commit_id){printf("%s\n","Invalid commit id");return;}

    for (int i = 0; i < helper->commit_num; i++)
    {
        if (strcmp(helper->all_commit[i]->commit_id,commit_id)==0)
        {
            printf("%s [%s]: %s\n",helper->all_commit[i]->commit_id,helper->all_commit[i]->commit_branch,helper->all_commit[i]->commit_message);
            for (int j = helper->all_commit[i]->add_c-1; j >=0; j--)
            {
                printf("    + %s\n",helper->all_commit[i]->add[j]);
            }
            for (int j = helper->all_commit[i]->rm_c-1; j >=0; j--)
            {
                printf("    - %s\n",helper->all_commit[i]->rm[j]);
            }
            printf("\n");
            printf("    Tracked files (%d):\n",helper->all_commit[i]->all_c);
            for (int j = helper->all_commit[i]->all_c-1; j >=0; j--)
            {
                printf("    [%10d] %s\n",*helper->all_commit[i]->all_hash[j],helper->all_commit[i]->all[j]);
            }
            return;
        }
    }printf("%s\n","Invalid commit id");
}

int svc_branch(helperThing *helper, char *branch_name) {
    // 1st check if there is modify change
    check_modify(helper);
    if(!branch_name){return -1;}
    if(regex("[^a-zA-Z0-9_/-]",branch_name)){return -1;}
    if(helper->change_num!=0){return -3;}
    if(containbranch(helper->branch_num,helper->all_branch,branch_name)){return -2;}
    helper->all_branch=(struct branchThing **)realloc(helper->all_branch,(helper->branch_num+1)*sizeof(struct branchThing*));
    helper->all_branch[helper->branch_num]=(struct branchThing*)malloc(1*sizeof(struct branchThing));
    helper->all_branch[helper->branch_num]->branch_name=(char *)malloc(strlen(branch_name)*sizeof(char)+2);
    strcpy(helper->all_branch[helper->branch_num]->branch_name,branch_name);
    helper->all_branch[helper->branch_num]->point=helper->cur_branch->point;
    helper->branch_num+=1;
    return 0;
}

int svc_checkout(helperThing *helper, char *branch_name) {
    // 2nd check if there is modify change
    check_modify(helper);
    if(!branch_name||!containbranch(helper->branch_num,helper->all_branch,branch_name)){return -1;}
    if(helper->change_num!=0){return -2;}
    reset(helper);
    reset_all(helper);
    int k;
    for (k = 0; k < helper->branch_num; k++)
    {
        if(strcmp(helper->all_branch[k]->branch_name,branch_name)==0){
            break;
        }
    }
    helper->cur_branch=helper->all_branch[k];

    helper->all_num=helper->cur_branch->point->all_c;
    helper->all_file=(char **)realloc(helper->all_file,helper->cur_branch->point->all_c*sizeof(char*));
    for (int j = 0; j < helper->cur_branch->point->all_c; j++)
    {
        int maxlength=260-strlen(helper->cur_branch->point->all[j]);
        helper->all_file[j]=(char *)malloc(strlen(helper->cur_branch->point->all[j])*sizeof(char)+maxlength);
        strcpy(helper->all_file[j],helper->cur_branch->point->all[j]);
    }

    for (int j = 0; j < helper->cur_branch->point->all_c; j++)
    {
        FILE* add=fopen(helper->cur_branch->point->all[j], "w");
        fprintf(add, "%s",helper->cur_branch->point->file[j]);
        fclose(add);
    }
    strcpy(helper->commit_id,helper->cur_branch->point->commit_id);
    strcpy(helper->HEAD,branch_name);
    return 0;
}

char **list_branches(helperThing *helper, int *n_branches) {
    if(!n_branches){return NULL;}
    char** list=(char **)malloc(helper->branch_num*sizeof(char*));
    for (int i = 0; i < helper->branch_num; i++)
    {
        list[i]=helper->all_branch[i]->branch_name;
        printf("%s\n", helper->all_branch[i]->branch_name);
    }
    *n_branches=helper->branch_num;
    return list;
}

int svc_add(helperThing *helper, char *file_name) {
    if(!file_name){
        return -1;
    }
    FILE *fn=fopen(file_name,"rb");
    if(!fn){
        return -3;
    }
    //find if already exist
    for (int i = 0; i < helper->all_num; i++)
    {   
        if(helper->all_file[i]!=NULL){
            if(strcmp(helper->all_file[i],file_name)==0){
                return -2;
            }
        }
    }
    int maxlength=260-strlen(file_name);
    // add to add_file
    helper->add_file=(char**)realloc(helper->add_file,(helper->add_num+1)*sizeof(char*));
    helper->add_file[helper->add_num]=(char *)malloc(sizeof(char)*strlen(file_name)+maxlength);
    strcpy(helper->add_file[helper->add_num],file_name);
    helper->add_num+=1;
    // add to all_file
    helper->all_file=(char**)realloc(helper->all_file,(helper->all_num+1)*sizeof(char*));
    helper->all_file[helper->all_num]=(char *)malloc(sizeof(char)*strlen(file_name)+maxlength);
    strcpy(helper->all_file[helper->all_num],file_name);
    helper->all_num+=1;
    // add to change_file
    if(!contain(helper->change_num,helper->change_file,file_name)){
        helper->change_file=(char**)realloc(helper->change_file,(helper->change_num+1)*sizeof(char*));
        helper->change_file[helper->change_num]=(char *)malloc(sizeof(char)*strlen(file_name)+maxlength);
        strcpy(helper->change_file[helper->change_num],file_name);
        helper->change_num+=1;
    }
    
    fclose(fn);
    return hash_file(helper,file_name);
}

int svc_rm(helperThing *helper, char *file_name) {
    if(!file_name){
        return -1;
    }
    for (int i = 0; i < helper->all_num; i++)
    {   
        
        if(strcmp(file_name,helper->all_file[i])==0){

            int maxlength=260-strlen(file_name);
            

            if(!contain(helper->add_num,helper->add_file,file_name)){
                // add to rm_file
                helper->rm_file=(char**)realloc(helper->rm_file,(helper->rm_num+1)*sizeof(char*));
                helper->rm_file[helper->rm_num]=(char *)malloc(sizeof(char)*strlen(file_name)+maxlength);
                strcpy(helper->rm_file[helper->rm_num],file_name);
                helper->rm_num+=1;
                // add to change_file
                if(!contain(helper->change_num,helper->change_file,file_name)){
                    helper->change_file=(char**)realloc(helper->change_file,(helper->change_num+1)*sizeof(char*));
                    helper->change_file[helper->change_num]=(char *)malloc(sizeof(char)*strlen(file_name)+maxlength);
                    strcpy(helper->change_file[helper->change_num],file_name);
                    helper->change_num+=1;
                }
            }
            else{
                for (int j = 0; j <helper->add_num; j++){
                    if(strcmp(file_name,helper->add_file[j])==0){
                        for (int k = j; k < helper->add_num-1; k++)
                        {
                            strcpy(helper->add_file[k],helper->add_file[k+1]);

                        }   
                        free(helper->add_file[helper->add_num-1]);
                        helper->add_file=(char **)realloc(helper->add_file,(helper->add_num-1)*sizeof(char*));
                        helper->add_num-=1;
                        break;
                    }
                }
                for (int j = 0; j <helper->change_num; j++){
                    if(strcmp(file_name,helper->change_file[j])==0){
                        for (int k = j; k < helper->change_num-1; k++)
                        {
                            strcpy(helper->change_file[k],helper->change_file[k+1]);

                        }   
                        free(helper->change_file[helper->change_num-1]);
                        helper->change_file=(char **)realloc(helper->change_file,(helper->change_num-1)*sizeof(char*));
                        helper->change_num-=1;
                        break;
                    }
                }
            }
            

            // change the all_file
            for (int p = i; p < helper->all_num-1; p++)
            {
                strcpy(helper->all_file[p],helper->all_file[p+1]);
            }
            free(helper->all_file[helper->all_num-1]);
            helper->all_file=(char **)realloc(helper->all_file,(helper->all_num-1)*sizeof(char*));
            helper->all_num-=1;

            return hash_file(helper,file_name);
        }
    }
    return -2;  
}

int svc_reset(helperThing *helper, char *commit_id) {
    if (!commit_id){return -1;}
    int check_in_all_commit=0;
    int i;
    for (i = 0; i < helper->commit_num; i++)
    {
        if (strcmp(helper->all_commit[i]->commit_id,commit_id)==0)
        {
            check_in_all_commit=1;
            break;
        }
    }
    if (!check_in_all_commit){return -2;}
    //change current commit id to this one
    strcpy(helper->commit_id,commit_id);
    
    //discard all things 
    
    reset(helper);
    reset_all(helper);
    //reset tracked file
    helper->all_num=helper->all_commit[i]->all_c;
    helper->all_file=(char **)realloc(helper->all_file,helper->all_commit[i]->all_c*sizeof(char*));
    for (int j = 0; j < helper->all_commit[i]->all_c; j++)
    {
        helper->all_file[j]=(char *)malloc(strlen(helper->all_commit[i]->all[j])*sizeof(char)+2);
        strcpy(helper->all_file[j],helper->all_commit[i]->all[j]);
    }
    for (int j = 0; j < helper->all_commit[i]->all_c; j++)
    {
        FILE* add=fopen(helper->all_commit[i]->all[j], "w");
        fprintf(add, "%s",helper->all_commit[i]->file[j]);
        fclose(add);
    }
    return 0;
}

char *svc_merge(helperThing *helper, char *branch_name, struct resolution *resolutions, int n_resolutions) {
    // 3rd check if there is modify change
    check_modify(helper);
    if (!branch_name)
    {
        printf("%s\n", "Invalid branch name");
        return NULL;
    }
    if(!containbranch(helper->branch_num,helper->all_branch,branch_name))
    {
        printf("%s\n", "Branch not found");
        return NULL;
    }
    if (strcmp(helper->HEAD,branch_name)==0)
    {
        printf("%s\n", "Cannot merge a branch with itself");
        return NULL;
    }
    if(helper->change_num!=0)
    {
        printf("%s\n", "Changes must be committed");
        return NULL;
    }

    helper->if_merge=1;


    for (int i = 0; i < n_resolutions; i++)
    {
        if(resolutions[i].resolved_file){
            char* buffer=read_and_write(resolutions[i].resolved_file);
            FILE* add=fopen(resolutions[i].file_name, "w");
            fprintf(add, "%s",buffer);
            fclose(add);
            free(buffer);
        }
    }
    int k;
    for (k = 0; k < helper->branch_num; k++)
    {
        if(strcmp(helper->all_branch[k]->branch_name,branch_name)==0){
            break;
        }

    }
    for (int i = 0; i < helper->all_branch[k]->point->all_c; i++)
    {
        for (int j = 0; j < helper->cur_branch->point->all_c; j++)
        {
            if(strcmp(helper->all_branch[k]->point->all[i],helper->cur_branch->point->all[j])!=0){
                FILE* add=fopen(helper->all_branch[k]->point->all[i], "w");
                fprintf(add, "%s",helper->all_branch[k]->point->file[i]);
                fclose(add);
                svc_add(helper,helper->all_branch[k]->point->all[i]);
            }
        }
    }
    
        

    helper->merge_num=2;
    helper->merge_list=(char **)realloc(helper->merge_list,(helper->merge_num)*sizeof(char*));

    for (int i = 0; i < helper->merge_num; i++)
    {
        helper->merge_list[i]=(char *)malloc(50*sizeof(char));
        strcpy(helper->merge_list[i],helper->all_branch[i]->point->commit_id);
    }
    

    char*message=malloc(70*sizeof(char));
    strcpy(message,"Merged branch ");
    strcat(message,branch_name);
    char* id=svc_commit(helper, message);
    free(message);

    helper->if_merge=0;

    printf("%s\n", "Merge successful");

    //free
    for (int i = 0; i < helper->merge_num; i++)
    {
        free(helper->merge_list[i]);
    }
    free(helper->merge_list);
    helper->merge_list=(char **)malloc(sizeof(char*));
    helper->merge_num=0;



    return id;
}