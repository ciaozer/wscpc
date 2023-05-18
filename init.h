#ifndef _INIT_H_
#define _INIT_H_

#include"data_structure.h"
#include"build.h"
#include"heuristic.h" 

long long *greedy_cover_weight; //第i个集合还能覆盖的权重和
int *greedy_conflict;    //第i个集合还冲突的集合数

// void greedy_flip(int flipitem)
// {
//     current_solution[flipitem] = 1-current_solution[flipitem];

//     //更新candidate，当前分数，covered_times
//     index_in_solution_stack[flipitem] = solution_stack_count;
//     PUSH(flipitem, solution_stack);

//     //该集合覆盖的所有元素被覆盖次数+1
//     for( int i=0; i<m_item_count[flipitem]; i++ )
//     {
//         int cur_ele = m_item[flipitem][i];
//         element_cover_times[cur_ele]++;
//         if( element_cover_times[cur_ele] == 1 )
//         { 
//             covered(cur_ele);

//             //所有能覆盖该元素的集合的greedy_cover_weight减少
//             for( int j=0; j<n_ele_count[cur_ele]; j++ )
//             {
//                 int cur_item = n_ele[cur_ele][j];
//                 greedy_cover_weight[cur_item] -= original_weight[cur_ele];
//             }
//         }   
//     }

//     //与该集合相冲突的所有集合冲突数+1
//     //更新candidate，遍历candidate，删除与flipitem冲突的
//     //或者遍历flipitem的冲突，看看是否在candidate中
//     for( int i=0; i<g_item_count[flipitem]; i++ )
//     {
//         int cur_item = g_item[flipitem][i];
//         item_conflict_times[cur_item]++;
//         if( item_conflict_times[cur_item] == 1 )
//         {
//             candidate.erase(cur_item);
//         }
//     }     
// }

void greedy()
{   //随机选取集合加入solution，直到candidate为空
    //所有元素都未被覆盖
    // for( int i=0; i<elementnum; i++ )
    // {
    //     uncovered(i);
    // }

    for( int i=1; i<=itemnum; i++ )
    {
        current_solution[i] = random() % 2;
        best_solution[i] = current_solution[i];
        item_conflict_times[i] = 0;  //没有任何集合在当前解中，冲突数都为0
        is_in_conflict_stack[i] = 0;
    }

    // while( candidate_stack_count > 0 ) //还存在可以加入结果的集合
    // {
    //     // int index = random()%candidate.size();
    //     // auto it = candidate.begin();
    //     // advance(it, index);
    //     int cur_item = candidate_stack[random()%candidate_stack_count];

    //     flip(cur_item);
    //     //candidate.erase(it);
    //     int last_candidate = POP(candidate_stack);
    //     int index_for_last_candidate = index_in_candidate_stack[cur_item];
    //     index_in_candidate_stack[last_candidate] = index_for_last_candidate;
    //     candidate_stack[index_for_last_candidate] = last_candidate;
    // }

    //处理元素被覆盖次数，并且将未被覆盖的元素入栈
    for( int i=0; i<elementnum; i++ )
    {
        for( int j=0; j<n_ele_count[i]; j++ )       //遍历能覆盖该元素的集合
        {
            int cur_item = n_ele[i][j];
            if( current_solution[cur_item] == 1 )   //如果当前解选入了该集合
            {
                element_cover_times[i]++;           //多覆盖一次
                only_cover_item[i] = cur_item;      //唯一能覆盖该元素的集合
            }
        }

        if( element_cover_times[i] == 0 )           //该元素未被覆盖
        {
            //cout << "hello" << endl;
            uncovered(i);                   //处理uncovered_stack和未被覆盖的权重和
        }
    }

    //处理集合的初始分数
    for( int i=1; i<=itemnum; i++ )
    {
        score[i] = 0;
        for( int j=0; j<m_item_count[i]; j++ )              //遍历集合能覆盖的所有元素
        {
            int cur_ele = m_item[i][j];
            if( element_cover_times[cur_ele] == 0 )         //该元素未被当前解覆盖，也说明当前解没选该集合
            {
                score[i] += original_weight[cur_ele];       //选入该集合会多得一些分
            }
            else if( element_cover_times[cur_ele] == 1 && current_solution[cur_ele] == 1 )  //恰好由该集合覆盖
            {
                score[i] -= original_weight[cur_ele];       //移除该集合会失去一些分
            }

            //其他情况翻转并不会改变覆盖的元素
        }
    }

    //处理solution_stack
    for( int i=1; i<=itemnum; i++ )
    {
        if( current_solution[i] == 1 )
        {
            index_in_solution_stack[i] = solution_stack_count;
            PUSH(i, solution_stack);
        }
    }

    //处理item_conflict_times，获得集合冲突的次数，只需要遍历solution就可以
    for( int i=0; i<solution_stack_count; i++ )
    {
        int cur_item = solution_stack[i];

        for( int j=0; j<g_item_count[cur_item]; j++ )  //遍历所有与该集合冲突的集合
        {
            int another_item = g_item[cur_item][j];
            if( current_solution[another_item] == 1 )   
            {                                   //这两个集合都在当前解中，相互冲突
                item_conflict_times[cur_item]++;       //不需要处理cur_item，会在它自己的循环中处理
            }
        }
    }

    //处理conflict_stack，冲突次数大于0的集合会进入栈，只需要遍历solution

    for( int i=0; i<solution_stack_count; i++ )
    {
        int cur_item = solution_stack[i];

        if( item_conflict_times[cur_item] > 0 )
        {
            is_in_conflict_stack[cur_item] = 1;
            index_in_conflict_stack[cur_item] = conflict_stack_count;
            PUSH(cur_item, conflict_stack);
        }      
    }

    //处理candidate，不在solution中，且item_conflict_times为0的集合是candidate
    // for( int i=1; i<=itemnum; i++ )  //写的有问题
    // {
    //     if( current_solution[i] == 0 && item_conflict_times[i] == 0 )
    //     {
    //         index_in_candidate_stack[i] = candidate_stack_count;
    //         PUSH(i, candidate_stack);
    //     }
    // }

    cout << current_uncovered_weight << endl;
    cout << "finish greedy" << endl;
    // cout << conflict_stack_count << endl;
    // for( int i=0; i<conflict_stack_count; i++ )
    // {
    //     cout << conflict_stack[i] << " ";
    //     if( i%50 == 49 )
    //         cout << endl;
    // }
    // cout << endl;
    // for( int i=0; i<solution_stack_count; i++ )
    // {
    //     cout << solution_stack[i] << " ";
    //     if(i%50 == 49 )
    //         cout << endl;
    // }
    // cout << endl;
}

// void greedy()
// {//根据覆盖权重和(除去已经覆盖的权重）与当前冲突数（candidate和solution之外）的比值来选择candidate加入solution
//     greedy_cover_weight = new long long[itemnum+10];
//     greedy_conflict = new int[itemnum+10];

//     //所有元素都未被覆盖
//     for( int i=0; i<elementnum; i++ )
//     {
//         uncovered(i);
//     }

//     for( int i=1; i<=itemnum; i++ )
//     {
//         current_solution[i] = 0;
//         item_conflict_times[i] = 0;  //没有任何集合在当前解中，冲突数都为0
//     }

//     cout << total_weight << endl;
//     cout << current_uncovered_weight << endl;

//     //初始化两个需要维护的变量
//     for( int i=1; i<=itemnum; i++ )
//     {
//         greedy_conflict[i] = g_item_count[i];

//         greedy_cover_weight[i] = 0;
//         for( int j=0; j<m_item_count[i]; j++ )
//         {
//             greedy_cover_weight[i] += original_weight[m_item[i][j]];
//         }
//     }


//     double greedy_best; //记录最大的比值
//     double greedy_current;  //记录当前比值
//     int greedy_index;   //记录需要选中的集合
//     bool is_conflict;   //记录是否需要处理冲突集合
//     while( candidate_stack_count > 0 ) //还存在可以加入结果的集合
//     {
//         greedy_best = 0;
//         greedy_current = 0;
//         greedy_index = 1;
//         is_conflict = false;
        
//         for(int i=0; i<candidate_stack_count; i++ )
//         {
//             int cur_candidate = candidate_stack[i];
//             if( greedy_conflict[cur_candidate] == 0 )     //该集合不与剩下的集合冲突
//             {
//                 greedy_index = cur_candidate;
//                 break;
//             }

//             is_conflict = true;
//             greedy_current = (double)greedy_cover_weight[cur_candidate] / (double)greedy_conflict[cur_candidate];
//             if( greedy_current > greedy_best )
//             {
//                 greedy_best = greedy_current;
//                 greedy_index = cur_candidate;
//             }
//         }

//         greedy_flip(greedy_index);
//         candidate.erase(greedy_index);

//         //更新两个greedy变量,greedy_cover_weight在greedy_flip中已经处理
//         for( int i=0; i<g_item_count[greedy_index]; i++ )
//         {
//             int cur_item = g_item[greedy_index][i];
//             if( candidate.find(cur_item) == candidate.end() && current_solution[cur_item] == 0 )
//             {   //不在candidate中也不在solution中
//                 greedy_conflict[cur_item]--;
//             }
//         }
//     }

//     for( int i=1; i<=itemnum; i++ )
//     {
//         best_solution[i] = current_solution[i];
//         best_uncovered_weight = current_uncovered_weight;
//     }

//     cout << "finish greedy" << endl;
//     cout << "now:" << current_uncovered_weight << endl;

//     cout << check_answer() << endl;

//     delete[] greedy_cover_weight;
//     delete[] greedy_conflict;
// }

#endif