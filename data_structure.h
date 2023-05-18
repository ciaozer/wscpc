#ifndef _DATA_STRUCTURE_H_
#define _DATA_STRUCTURE_H_

#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<cmath>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
#include<sys/times.h>
#include<unistd.h>
#include<unordered_set>
#include<algorithm>

using namespace std;

#define POP(stack) stack[--stack##_count]
#define PUSH(A, stack) stack[stack##_count++] = A

const float MAX_RAND_FLOAT = 10000000.0;
const int MAX_RAND_INT = 10000000;
const float BASIC_SCALE = 0.0000001;

struct ITEM{
    int relation_num;           //出现在哪个关系中，从0开始
    int item_index;             //集合的编号
    int sense;                  //1表示被选择，0表示不被选择
};

static struct tms start_time;
static double get_runtime()
{
    struct tms stop_time;
    times(&stop_time);
    return (double)(stop_time.tms_utime - start_time.tms_utime +stop_time.tms_stime - start_time.tms_stime) / sysconf(_SC_CLK_TCK);
}

static void start_timing()
{
	times(&start_time);
}

bool is_weighted;  //问题是否带权重
int itemnum;
int elementnum;
        
int tries;
int max_tries;
unsigned int max_flips;
unsigned int step;
long long *time_stamp;  //记录在哪个step改变过集合是否被选中的状态
long long *time_stamp_ccmpitem;

int cutfoff_time;
int opt_time;

double p_random_walk;
double p_without_hard_conf;

int *m_item_count;          //每个集合覆盖的元素数
int **m_item;               //m_item[i][j]表示i号集合覆盖j号元素
int *g_item_count;          //每个集合的冲突个数
int **g_item;               //g_item[i][j]表示i号集合与j号集合冲突
int *n_ele_count;           //每个元素被覆盖的次数
int **n_ele;                //n_ele[i][j]表示i号元素被j号集合覆盖

long long *original_weight;
long long best_uncovered_weight;
long long current_uncovered_weight;
long long total_weight;     //所有元素的权重和

int *unit_ele;              //单子句，unit_ele[i]=j表示i号元素仅被j号集合覆盖
         
bool *best_solution;         //best_solution[i]=0表示不选该集合，1表示选该集合
bool *current_solution;

int *solution_stack;        //存储当前解
int solution_stack_count;
int *index_in_solution_stack;    //集合在解集栈中的编号
int *candidate_stack;       //可以加入当前解的集合的栈
int candidate_stack_count;
int *index_in_candidate_stack;  //集合在候选集中的编号

int *element_cover_times;   //元素被覆盖的次数
int *only_cover_item;       //当仅被一个元素覆盖时，存储覆盖该元素的集合编号
int *uncovered_stack;       //存储未被覆盖的元素编号
int uncovered_stack_count;         
int *index_in_uncovered_stack;   //元素在不满足栈中的编号    
int *item_conflict_times;       //当前集合与在当前解中的集合冲突了几次
int *conflict_stack;        //当前解中冲突的集合
int conflict_stack_count;
bool *is_in_conflict_stack; //是否在当前解冲突的集合中
int *index_in_conflict_stack;    
long long *score;                 //选入该集合可以多覆盖多少权重，选出会损失多少权重
int *best_array;            //记录相同的最大值，平局随机选
int *ccmp_item;

bool *hard_conf;            //当邻居变化后为1，否则为0
bool *hard_cscc;

int **neighbor;         //neighbor[i][j]表示集合i的第j个邻居
int *neighbor_count;

void build_neighbor_relation();
void allocate_memory();
int select_item();
void flip(int flip_item);
inline void covered(int element_index);
inline void uncovered(int element_index);
    
void settings();
void build(char* filename);
void local_search(char* filename);
void print_best_solution();
void free_memory();
int check_answer();
bool parse_parameters(int argc, char **argv);


#endif