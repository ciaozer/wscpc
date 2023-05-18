#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_

#include"data_structure.h"
#include"preprocess.h"
#include"init.h"

inline void covered(int element_index)
{//栈操作，更新权重
    int last_uncovered_element = POP(uncovered_stack);
    int index = index_in_uncovered_stack[element_index];
    uncovered_stack[index] = last_uncovered_element;
    index_in_uncovered_stack[last_uncovered_element] = index;
        
    current_uncovered_weight -= original_weight[element_index];
}

inline void uncovered(int element_index)
{
    index_in_uncovered_stack[element_index] = uncovered_stack_count;
    PUSH(element_index, uncovered_stack);

    current_uncovered_weight += original_weight[element_index];
}

int select_item_random()
{//随机选取一个未被覆盖的元素,随机选取能覆盖该元素的一个集合使其满足
    //随机移除一个集合

    while(1)
    {
        int rand_index = random() % solution_stack_count;
        if( hard_conf[solution_stack[rand_index]] == 1 )
        {
            return solution_stack[rand_index];
        }
    }
}

int select_item()
{
    int selected_item;
    if( random()%MAX_RAND_INT*BASIC_SCALE < p_random_walk )
    {
        if( conflict_stack_count > 0 )
        {//随机选一个有冲突的集合删除
            selected_item = conflict_stack[random()%conflict_stack_count];
            return selected_item;
        }
        else    //随机选取一个未被覆盖的元素，再随机选取能覆盖它的一个集合
        {
            int selected_element = uncovered_stack[random()%uncovered_stack_count];
            selected_item = n_ele[selected_element][random()%n_ele_count[selected_element]];
            return selected_item;
        }
    }

    if( random()%MAX_RAND_INT*BASIC_SCALE < p_without_hard_conf )
    {   
        if( conflict_stack_count > 0 )
        {//随机选一个有冲突的集合删除
            selected_item = conflict_stack[random()%conflict_stack_count];
            return selected_item;
        }
        else
        {   //从随机一个未覆盖元素中选一个能覆盖最多元素的集合
            if(uncovered_stack_count == 0)
            {
                cout << "uncovered_stack empty";
            }
            int selected_element = uncovered_stack[random()%uncovered_stack_count];

            int cur_item = n_ele[selected_element][0];
            best_array[0] = cur_item;
            int best_count = 1;
            int best_score = score[cur_item];

            for( int i=1; i<n_ele_count[selected_element]; i++ )
            {
                cur_item = n_ele[selected_element][i];
                if( score[cur_item] > best_score )
                {
                    best_array[0] = cur_item;
                    best_count = 1;
                    best_score = score[cur_item];
                }
                else if( score[cur_item] == best_score )
                {
                    best_array[best_count] = cur_item;
                    best_count++;
                }
            }

            return best_array[random()%best_count];
        }
    }

    //大多数时候采用如下策略
    if( conflict_stack_count > 0 )  //反转后能覆盖更多元素，这里面没有冲突的
    {
        int best_count = 0;         //冲突集中的一个子集
        
        for( int i=0; i<conflict_stack_count; i++ )
        {
            int cur_item = conflict_stack[i];

            if( hard_cscc[cur_item] == 1 )
            {
                best_array[best_count] = cur_item;
                best_count++;
            }
        }

        if( best_count > 0 )
        {
            if( best_count == 1 )
            {
                return best_array[0];
            }

            if( best_count > 15 && (random()%MAX_RAND_INT)*BASIC_SCALE < 0.6 )
            {
                int v1,v2;
                if ( best_count == 2 ) 
                {
                    v1 = best_array[0];
                    v2 = best_array[1];
                }
                else 
                {
                    v1 = best_array[rand()%best_count];
                    v2 = best_array[rand()%best_count];
                    while ( v2 == v1 )  
                        v2 = best_array[rand()%best_count];
                }
                
                int best_var=v1;
                if ( score[v2] > score[v1] ) 
                {
                    best_var = v2;
                }
                else if ( score[v2] == score[v1] && time_stamp[v2] < time_stamp[v1] )
                { 
                    best_var=v2;
                }

                return best_var;
            }

            else        //有较少的好集合
            {
                int best_item = best_array[0];
                for ( int i = 1; i < best_count; i++ ) 
                {
                    int cur_item = best_array[i];

                    if( item_conflict_times[cur_item] > item_conflict_times[best_item] )
                    {
                        best_item = cur_item;
                    }

                    else if( item_conflict_times[cur_item] == item_conflict_times[best_item] )
                    {
                        if ( score[cur_item] > score[best_item]) 
                        {
                            best_item = cur_item;
                        }
                        else if ( score[cur_item] == score[best_item] && time_stamp[cur_item] < time_stamp[best_item] ) 
                        {
                            best_item = cur_item;
                        }
                    }
                }

                return best_item;
            }
        }
    }

    //不存在满足conf变了的conflict集合
    int ccmpitem_count = 0;
	
	for( int i=0; i<conflict_stack_count; i++ )
	{
		int cur_item = conflict_stack[i];

		if( hard_conf[cur_item] == 1 && time_stamp_ccmpitem[cur_item] != step)
		{
			ccmp_item[ccmpitem_count] = cur_item;
            ccmpitem_count++;
			time_stamp_ccmpitem[cur_item] = step;
		}
	}
	
	for( int i=0; i<uncovered_stack_count; i++ )
	{
		int cur_ele = uncovered_stack[i];

        for( int j=0; j<n_ele_count[cur_ele]; j++ )
        {
            int cur_item = n_ele[cur_ele][j];

            if( hard_conf[cur_item] == 1 && time_stamp_ccmpitem[cur_item] !=step )
            {
                ccmp_item[ccmpitem_count] = cur_item;
                ccmpitem_count++;
                time_stamp_ccmpitem[cur_item] = step;
            }
        }
	}
	
	if( ccmpitem_count > 0 )
	{
		int cur_item = ccmp_item[0];
		best_array[0] = cur_item;
		int best_count = 1;
		int best_conflict = item_conflict_times[cur_item];
		int best_score = score[cur_item];
		
		for( int i=1; i<ccmpitem_count; i++ )
		{
			cur_item = ccmp_item[i];
			
			if( item_conflict_times[cur_item] > best_conflict )
			{
				best_array[0] = cur_item;
				best_count = 1;
				best_conflict = item_conflict_times[cur_item];
				best_score = score[cur_item];
			}

			else if( item_conflict_times[cur_item] == best_conflict )
			{
				if( score[cur_item] > best_score )
				{
					best_array[0] = cur_item;
					best_count = 1;
					best_score = score[cur_item];
				}

				else if( score[cur_item] == best_score )
				{
					best_array[best_count] = cur_item;
                    best_count++;
				}
			}
		}
		
		return best_array[random()%best_count];
	}

	else    //不存在ccmpitem
	{
		if( conflict_stack_count > 0 )
        {//随机选一个有冲突的集合删除
            selected_item = conflict_stack[random()%conflict_stack_count];
            return selected_item;
        }
        else    //随机选取一个未被覆盖的元素，再随机选取能覆盖它的一个集合
        {
            int selected_element = uncovered_stack[random()%uncovered_stack_count];
            selected_item = n_ele[selected_element][random()%n_ele_count[selected_element]];
            return selected_item;
        }
	}
}

void flip(int flipitem)
{
    current_solution[flipitem] = 1-current_solution[flipitem];
    int original_score = score[flipitem];

    //更新candidate，当前分数，element_cover_times, solution, conflict等变量
    if( current_solution[flipitem] == 1 )  //选入进当前解
    {
        //更新solution_stack
        index_in_solution_stack[flipitem] = solution_stack_count;
        PUSH(flipitem, solution_stack);

        //更新item_conflict_times和conflict_stack
        for( int i=0; i<g_item_count[flipitem]; i++ )   //对于该集合的所有冲突集合
        {
            int cur_item = g_item[flipitem][i];

            if( current_solution[cur_item] == 1 )   //当前解选入了冲突的集合
            {
                item_conflict_times[cur_item]++;
                item_conflict_times[flipitem]++;
                if( item_conflict_times[cur_item] == 1 )    //加入该集合后出现冲突
                {
                    index_in_conflict_stack[cur_item] = conflict_stack_count;
                    PUSH(cur_item, conflict_stack);
                    is_in_conflict_stack[cur_item] = 1;
                }
            }
        }
        if( item_conflict_times[flipitem] > 0 )
        {
            index_in_conflict_stack[flipitem] = conflict_stack_count;
            PUSH(flipitem, conflict_stack);
        }

        //该集合覆盖的所有元素被覆盖次数+1
        for( int i=0; i<m_item_count[flipitem]; i++ )
        {
            int cur_ele = m_item[flipitem][i];
            element_cover_times[cur_ele]++;

            if( element_cover_times[cur_ele] == 1 ) //恰好被该集合覆盖
            { 
                covered(cur_ele);
                only_cover_item[cur_ele] = flipitem;    //唯一能覆盖该元素的集合就是翻转集合

                for( int j=0; j<n_ele_count[cur_ele]; j++ ) //对于能覆盖该元素的所有集合
                {
                    int cur_item =n_ele[cur_ele][j];
                    score[cur_item] -= original_weight[cur_ele];    //再选入该集合不会使分数增加
                }
            }   

            else if( element_cover_times[cur_ele] == 2 )
            {
                score[only_cover_item[cur_ele]] += original_weight[cur_ele];
            }
        }

        // //更新candidate，遍历candidate，删除与flipitem冲突的
        // for( int i=0; i<candidate_stack_count; i++ )
        // {
        //     int cur_item = candidate_stack[i];

        //     if(  || cur_item == flipitem )
        //     {

        //     }
        // }
    }

    else                //从solution中移除
    {   
        //将其从solution中移除
        int last_solution = POP(solution_stack);
        int index_for_last_solution = index_in_solution_stack[flipitem];
        index_in_solution_stack[last_solution] = index_for_last_solution;
        solution_stack[index_for_last_solution] = last_solution;

        //更新item_conflict_times和conflict_stack
        for( int i=0; i<g_item_count[flipitem]; i++ )
        {
            int cur_item = g_item[flipitem][i];

            if( current_solution[cur_item] == 0 )
            {
                continue;
            }

            item_conflict_times[cur_item]--;

            if( item_conflict_times[cur_item] == 0 )
            {
                int last_conflict = POP(conflict_stack);
                int index_for_last_conflict = index_in_conflict_stack[flipitem];
                index_in_conflict_stack[last_conflict] = index_for_last_conflict;
                conflict_stack[index_for_last_conflict] = last_conflict;
                is_in_conflict_stack[cur_item] = 0;

                hard_cscc[cur_item] = 1;
            }
        }
        item_conflict_times[flipitem] = 0;
        int last_conflict = POP(conflict_stack);
        int index_for_last_conflict = index_in_conflict_stack[flipitem];
        index_in_conflict_stack[last_conflict] = index_for_last_conflict;
        conflict_stack[index_for_last_conflict] = last_conflict;
        is_in_conflict_stack[flipitem] = 0;

        //更新元素被覆盖次数和权重
        for( int i=0; i<m_item_count[flipitem]; i++ )
        {
            int cur_ele = m_item[flipitem][i];
            element_cover_times[cur_ele]--;
            if( element_cover_times[cur_ele] == 0 )
            {
                uncovered(cur_ele);

                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    score[cur_item] += original_weight[cur_ele];
                }
            }

            else if( element_cover_times[cur_ele] == 1 )
            {   //找到唯一能覆盖它的集合，更新得分
                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    only_cover_item[cur_ele] = cur_item;
                    score[cur_item] -= original_weight[cur_ele];
                }
            }
        }

        //更新candidate，遍历flipitem的冲突集，冲突的都可能可以加入候选集
        // for( int i=0; i<g_item_count[flipitem]; i++ )
        // {
        //     int cur_item = g_item[flipitem][i];
        //     item_conflict_times[cur_item]--;

        //     hard_conf[cur_item] = 1;

        //     if( item_conflict_times[cur_item] == 0 )
        //     {
        //         PUSH(cur_item, candidate_stack);
        //     }
        // }

    }

    hard_cscc[flipitem] = 0;
    score[flipitem] = -original_score;
    hard_conf[flipitem] = 1;

    //更新hard_conf，后面决定是所有邻居还是只有硬邻居
    for( int i=0; i<neighbor_count[flipitem]; i++ )
    {
        int cur_item = neighbor[flipitem][i];
        hard_conf[cur_item] = 1;
    }
}

void local_search(char *filename)
{
    int flipitem;

    for( step=1; step<max_flips; step++ )
    {
        if( conflict_stack_count == 0 && current_uncovered_weight < best_uncovered_weight )
        {
            best_uncovered_weight = current_uncovered_weight;
            cout << "o " << best_uncovered_weight << "     steps: " << step << endl;
            
            for( int i=1; i<=itemnum; i++ )
            {
                best_solution[i] = current_solution[i];
            }

            if( current_uncovered_weight == 0 )
            {
                cout << "solved" << endl;
                check_answer();
                print_best_solution();
                return ;
            }
        }

        flipitem = select_item();
        if( flipitem < 1 || flipitem > itemnum )
        {
            cout << "select error" << endl;
        }

        flip(flipitem);

        //补全当前解
        // while( candidate.size() > 0 )
        // {
        //     int index = random()%candidate.size();
        //     auto it = candidate.begin();
        //     advance(it, index);

        //     flip(*it);
        //     candidate.erase(it);
        // }
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

        time_stamp[flipitem] = step;

        // cout << "best:" << best_uncovered_weight << "    current:" << current_uncovered_weight << endl;

        if( get_runtime() >= cutfoff_time )
        {
            cout << check_answer() << endl;
            print_best_solution();
            break;
        }
    }
}

#endif
