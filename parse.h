#ifndef _PARSE_H_
#define _PARSE_H_

#include"data_structure.h"
#include"heuristic.h"
#include"generate.h"
#include"init.h"
#include"build.h"

int seed = 1;
char *filename = NULL;

void assign_items(char *filename)
{
    ifstream infile;
    infile.open(filename, ios::in);

    // deal with the unexist file
    if (!infile.is_open())
    {
        cout << "read file error" << endl;
        cout << "please check your filename" << endl;
        cout << "i'm assign items" << endl;
        exit(0);
    }

    for( int i=1; i<=itemnum; i++ )
    {
        int tmp;
        infile >> tmp;
        if( tmp < 0 )
        {
            best_solution[i] = 0;
        }
        else
        {
            best_solution[i] = 1;
        }
    }
}

void print_usage(){
    cerr << "Usage: " << "./wscpc" << " [function] [parameters...]" << endl;
    cout << "function:" << endl;
    cout << "-g: geneate data" << endl;
    cout << "example: ./wscpc -g itemnum elementnum density avg_cover_times path" << endl;
    cout << "-s: change to sat" << endl;
    cout << "example: ./wscpc -s path_of_scpc path_of_sat rate" << endl; //rate mean reserve how many soft clause
    cout << "-m: change to maxsat" << endl;
    cout << "example: ./wscpc -m path_of_wscpc path_of_maxsat" << endl;
    cout << "-r: run" << endl;
    cout << "example: ./wscpc -r path" << endl;
}

void parse_args(int argc, char *argv[]){
    if ( argc < 2 ) {
        print_usage();
        return ;
    }
    
    if ( string(argv[1]).compare(string("-g")) == 0 )
    {
        if( argc != 7 ){
            print_usage();
        }
        ITEMNUM = atoi(argv[2]);
        ELEMENTNUM = atoi(argv[3]);
        DENSITY = atof(argv[4]);
        AVG_COVER_TIMES = atoi(argv[5]);
        cout << "generating data..." << endl;
        printf("ITEMNUM = %d\nELEMENTNUM = %d\nDENSITY = %lf\nAVG_COVER_TIMES = %d\npath = %s\n", ITEMNUM, ELEMENTNUM, DENSITY, AVG_COVER_TIMES, argv[6]);
        generate_random_once(argv[6]);
    }
    
    else if ( string(argv[1]).compare(string("-r")) == 0 )
    {
        if( argc != 3 )
        {
            print_usage();
        }
        //sscanf(argv[2], "%d", &seed);
	    srand(seed);
	    build(argv[2]);
	    greedy();
	    settings();
	    local_search(argv[1]);
	    free_memory();
    }

    else if ( string(argv[1]).compare(string("-s")) == 0 )
    {
        if( argc != 5 )
        {
            print_usage();
        }
        cout << "changing to sat" << endl;
        //change_to_sat(argv[2], argv[3], atof(argv[4]));
    }

    else if ( string(argv[1]).compare(string("-m")) == 0 )
    {
        if( argc != 4 )
        {
            print_usage();
        }
        cout << "changing to maxsat" << endl;
        change_to_maxsat(argv[2], argv[3]);
    }

    else if( string(argv[1]).compare(string("-c")) == 0 )
    {
        srand(seed);
        build(argv[2]);
        assign_items(argv[3]);
        check_answer();
    }

    else
    {
        print_usage();
        return ;
    }
}

#endif