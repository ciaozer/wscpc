#ifndef _BUILD_H_
#define _BUILD_H_

#include"data_structure.h"

void allocate_memory()
{
    int malloc_item = itemnum + 10;
    int malloc_element = elementnum + 10;

    m_item_count = new int[malloc_item];
    m_item = new int *[malloc_item];
    g_item_count = new int[malloc_item];
    g_item = new int *[malloc_item];
    n_ele_count = new int[malloc_element];
    n_ele = new int *[malloc_element];

    neighbor_count = new int[malloc_item];
    neighbor = new int *[malloc_item];

    original_weight = new long long[malloc_element];

    best_solution = new bool[malloc_item];
    current_solution = new bool[malloc_item];

    time_stamp = new long long[malloc_item];
    time_stamp_ccmpitem = new long long[malloc_item];

    element_cover_times = new int[malloc_element];
    only_cover_item = new int[malloc_element];
    uncovered_stack = new int[malloc_element];
    index_in_uncovered_stack = new int[malloc_element];
    item_conflict_times = new int[malloc_item];
    is_in_conflict_stack = new bool[malloc_item];
    conflict_stack = new int[malloc_item];
    index_in_conflict_stack = new int[malloc_item];
    score = new long long[malloc_item];
    best_array = new int[malloc_item];
    ccmp_item = new int[malloc_item];

    hard_conf = new bool[malloc_item];
    hard_cscc = new bool[malloc_item];

    solution_stack = new int[malloc_item];
    index_in_solution_stack = new int[malloc_item];
    candidate_stack = new int[malloc_item];
    index_in_candidate_stack = new int[malloc_item];

}

void free_memory()
{
    for( int i=1; i<=itemnum; i++ )
    {
        delete[] m_item[i];
        delete[] g_item[i];
        //delete[] neighbor[i];
    }

    for( int i=0; i<elementnum; i++ )
    {
        delete[] n_ele[i];
    }

    delete[] m_item;
    delete[] m_item_count;
    delete[] g_item;
    delete[] g_item_count;
    delete[] n_ele;
    delete[] n_ele_count;

    delete[] neighbor_count;

    delete[] original_weight;

    delete[] best_solution;
    delete[] current_solution;

    delete[] time_stamp;
    delete[] time_stamp_ccmpitem;

    delete[] element_cover_times;
    delete[] only_cover_item;
    delete[] uncovered_stack;
    delete[] index_in_uncovered_stack;
    delete[] item_conflict_times;
    delete[] is_in_conflict_stack;
    delete[] conflict_stack;
    delete[] index_in_conflict_stack;
    delete[] score;
    delete[] best_array;
    delete[] ccmp_item;

    delete[] hard_conf;
    delete[] hard_cscc;

    delete[] solution_stack;
    delete[] index_in_solution_stack;
    delete[] candidate_stack;
    delete[] index_in_candidate_stack;
}

void build(char *filename)
{
    ifstream infile;
    infile.open(filename, ios::in);
    if( !infile.is_open() )
    {
        cout << "read file " << filename << "error" << endl;
        exit(-1);
    }

    infile >> itemnum >> elementnum;

    allocate_memory();

    for( int i=1; i<=itemnum; i++ )
    {
        m_item_count[i] = 0;
        m_item[i] = NULL;
        g_item_count[i] = 0;
        g_item[i] = NULL;
        neighbor_count[i] = 0;
        neighbor[i] = NULL;

        hard_conf[i] = 1;
        time_stamp[i] = 0;
        hard_cscc[i] = 1;
    }
    for( int i=0; i<elementnum; i++ )
    {
        n_ele_count[i] = 0;
        n_ele[i] = NULL;
        element_cover_times[i] = 0;
    }

    uncovered_stack_count = 0;
    solution_stack_count = 0;
    conflict_stack_count = 0;
    candidate_stack_count = 0;

    //给初始权重赋值
    total_weight = 0; 
    current_uncovered_weight = 0;
    for( int i=0; i<elementnum; i++ )
    {
        infile >> original_weight[i];
        total_weight += original_weight[i];
    }
    cout << total_weight << endl;

    best_uncovered_weight = total_weight;

    for( int i=1; i<=itemnum; i++ )
    {
        int item_index;
        infile >> item_index;

        //读入该集合覆盖的元素
        infile >> m_item_count[i];
        m_item[i] = new int[m_item_count[i]+1];

        for( int ele=0; ele<m_item_count[i]; ele++ )
        {
            int cur_element;
            infile >> cur_element;
            n_ele_count[cur_element]++;  //该元素被覆盖次数加一
            m_item[i][ele] = cur_element;
        }

        //读入该集合冲突的集合
        infile >> g_item_count[i];
        g_item[i] = new int[g_item_count[i]+1];

        for( int it=0; it<g_item_count[i]; it++ )
        {
            int cur_item;
            infile >> cur_item;
            g_item[i][it] = cur_item;
        }

        // index_in_candidate_stack[i] = candidate_stack_count;
        // PUSH(i, candidate_stack);
        // current_solution[i] = 0;
    }

    infile.close();

    //建立n_ele关系
    for( int i=0; i<elementnum; i++ )
    {
        element_cover_times[i] = 0;
        n_ele[i] = new int[n_ele_count[i]+1];
        n_ele_count[i] = 0;  //重新计数
    }

    for( int it=1; it<=itemnum; it++ )
    {
        for( int ele=0; ele<m_item_count[it]; ele++ )
        {
            int cur_ele = m_item[it][ele];
            n_ele[cur_ele][n_ele_count[cur_ele]] = it;
            n_ele_count[cur_ele]++;
        }
    }

    //建立neighbor关系
    bool is_in_neighbor[itemnum+10];
    for( int i=1; i<=itemnum; i++ )
    {
        is_in_neighbor[i] = 0;
    }
    for( int i=1; i<=itemnum; i++ )
    {
        int temp_neighbor_count = 0;
        int temp_neighbor[itemnum+10];
        is_in_neighbor[i] = 1;  //不能把自己加进来

        //冲突邻居
        for( int j=0; j<g_item_count[i]; j++ )
        {
            int cur_item = g_item[i][j];
            temp_neighbor[temp_neighbor_count] = cur_item;
            temp_neighbor_count++;
            is_in_neighbor[cur_item] = 1;
        }

        //覆盖同一个元素的邻居
        for( int j=0; j<m_item_count[i]; j++ )
        {
            int cur_ele = m_item[i][j];
            for( int k=0; k<n_ele_count[cur_ele]; k++ )
            {
                int another_item = n_ele[cur_ele][k];
                if( is_in_neighbor[another_item] == 0 )
                {
                    is_in_neighbor[another_item] = 1;
                    temp_neighbor[temp_neighbor_count] = another_item;
                    temp_neighbor_count++;
                }
            }
        }

        //根据统计数据分配内存
        is_in_neighbor[i] = 0;
        neighbor[i] = new int[temp_neighbor_count+1];
        neighbor_count[i] = temp_neighbor_count;
        for( int j=0; j<temp_neighbor_count; j++ )
        {
            neighbor[i][j] = temp_neighbor[j];
            is_in_neighbor[temp_neighbor[j]] = 0;   //重置邻居关系
        }
    }

    cout << "finish read file" << endl;
}

void print_best_solution()
{
    for( int i=1; i<=itemnum; i++ )
    {
        if( best_solution[i] == true )
        {
            cout << i << " ";
        }
    }
    cout << endl;
}

int check_answer()
{//0表示不正确，1表示正确
    int check_cover_times[elementnum+10];
    int check_uncovered_weight = 0;
    memset(check_cover_times, 0, sizeof(check_cover_times));

    //检验是否有冲突
    for( int i=1; i<=itemnum; i++ )
    {
        if( best_solution[i] == 1 )
        {//只处理选中该集合的情况，没选中不会有冲突
            for( int j=0; j<g_item_count[i]; j++ )
            {
                if( best_solution[g_item[i][j]] == 1 )
                {
                    cout << i << " conflict with " << g_item[i][j] << endl;
                    return 0;
                }
            }

            //重新计算元素被覆盖次数
            for( int j=0; j<m_item_count[i]; j++ )
            {
                check_cover_times[m_item[i][j]]++;
            }
        }
    }

    //检查未覆盖权重是否正确
    for( int i=0; i<elementnum; i++ )
    {
        if( check_cover_times[i] == 0 )
        {
            check_uncovered_weight += original_weight[i];
        }
    }

    if( check_uncovered_weight != best_uncovered_weight )
    {
        cout << "real: " << check_uncovered_weight << endl;
        cout << "count: " << best_uncovered_weight << endl;
        return 0;
    }

    return 1;
}

void settings()
{
    cutfoff_time = 10;
    max_tries = 100000000;
    max_flips = 200000000;
    p_random_walk = 0.01;
    p_without_hard_conf = 0.2;
}

#endif