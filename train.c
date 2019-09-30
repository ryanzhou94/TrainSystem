// 20029947 scyzz2 Zewei Zhou

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// base lenth of the name of a station, used for dynamic allocation memory
#define NAME_LEN 10                  
// base number of stations, used for dynamic allocation memory
#define STATIONS 5                  
// abstract infinity number to present two cities which are not connected
#define INFINITY 999999             
                                    
// create a graph structure
struct graph{                       
        int start_station_num;      
        int end_station_num;        
        int **distances;            
        char **start_station;       
        char **end_station;         
};
typedef struct graph Graph;
typedef Graph * PtrToGraph;

// to read the name of end stations by using pointers and return
// integer number to indicate if everything is ok
int read_end_station(FILE *fp, PtrToGraph G);

// to read the name of start stations and distance at the same time 
// by using pointers and return integer number to indicate if everything is ok
int read_start_station_distance(FILE *fp, PtrToGraph G);

// to read the station name from users
char *get_station(void);

// to check if the name is included in the stirng array, 
// return 1 if the name is included in the station list
int isIncluded(char **station_name, int station_num, char *name);

// apply Dijkstra algorithm to get the shorest route and cost
void Dijkstra(Graph G, char *start, char *end);

// to tag each city a number from 0 to (City_Num - 1)
int convert_name_to_num(char **City_Name, int City_Num, char *name);

// to free memory allocated to graph including distance, start_station and end_station
void destroy_graph(PtrToGraph G);

// to find the start station in the name list of end station
int convert(char **City_Name, int City_Num, char *name);


int main(int argc, char const *argv[])
{
        // if the number of arguments is not 2
        if (argc != 2)                  
        {
                printf("Invalid command line arguments. Usage: train <disances.txt>\n");
                exit(4);
        }
        FILE *fp;
        // open the file only for reading
        fp = fopen(argv[1], "r");       
        // if the file cannot be opened
        if (fp == NULL)                 
        {
                perror("Cannot open file.");
                exit(1);
        }
        // declare a graph initialized with 0 start staion and 0 end station the program load data as well as check 
        Graph G = {0, 0};               
        // firstly read the first line which is names of end stations
        int check_1 = read_end_station(fp, &G);  
        // secondely read the rest which are names of start stations and distance          
        int check_2 = read_start_station_distance(fp, &G);
        // firstly check if the file is valid
        if (check_1 == 2 || check_2 == 2)
        {
                printf("Invalid distances file.\n");
                // close the file before exit the program
                fclose(fp);             
                destroy_graph(&G);
                // exit the program with exit code 2
                exit(2);                
        }
        // secondly check if memory allocation succeeds
        else if (check_1 == 3 || check_2 == 3)
        {
                printf("Unable to allocate memory.\n");
                // close the file before exit the program
                fclose(fp);             
                destroy_graph(&G);
                // exit the program with exit code 3
                exit(3);                
        }
        
        // so far, data have been loaded from file completely and the file can be closed right now
        fclose(fp);                     
        // prompt to input start station
        printf("Start station: ");      
        char *Start_Station = get_station();
        if (Start_Station == NULL)
        {
                printf("Unable to allocate memory.\n");
                destroy_graph(&G);
                exit(3);
        }
        char *End_Station; 
        // get_station() function will replace the first '\n' with a '\0', if user just enter a '\n'.
        // While user input something, the loop will repeatdly excute until user inputs nothing with a '\n'
        // The general idea about memory allocation is once the user wants to check train route(or path)(input is not '\n), 
        // then the program will malloc for Start_Station and End_Station. After that, the memory will be freed and this two
        // variables will points to 'NULL' and get ready for next iteration if the input is not '\n'.
        while(Start_Station[0] != '\0')        
        {
                // check if the name is included in the list of stations
                if (isIncluded(G.start_station, G.start_station_num, Start_Station) == 1)       
                {
                        // prompt to input end station if the name is included
                        printf("End station: ");                      
                        End_Station = get_station();
                        // check if memory allocation is successful
                        if (End_Station == NULL)                      
                        {
                                printf("Unable to allocate memory.\n");      
                                free(Start_Station);                  
                                destroy_graph(&G);
                                exit(3);                              
                        }
                        if (isIncluded(G.end_station, G.end_station_num, End_Station) == 0)
                        {
                                // there is no such station
                                printf("No such station.\n");         
                                printf("Start station: ");
                                free(End_Station);
                                free(Start_Station);
                                // points to NULL to avoid being wild pointer
                                Start_Station = NULL;                 
                                // points to NULL to avoid being wild pointer
                                End_Station = NULL;                   
                                // read start staion again
                                Start_Station = get_station();        
                                if (Start_Station == NULL)
                                {
                                        printf("Unable to allocate memory.\n");
                                        destroy_graph(&G);
                                        exit(3);
                                }
                                // move to next iteration
                                continue;                             
                        }
                        if (strcmp(Start_Station, End_Station) == 0)
                        {
                                printf("No journey, same start and end station.\n");
                                free(Start_Station);
                                free(End_Station);
                                Start_Station = NULL;
                                End_Station = NULL;
                                // read start station again
                                printf("Start station: ");            
                                Start_Station = get_station();
                                if (Start_Station == NULL)
                                {
                                        printf("Unable to allocate memory.\n");
                                        destroy_graph(&G);
                                        exit(3);
                                }
                                continue;
                        }
                        // apply Dijkstra algorithm
                        Dijkstra(G, Start_Station, End_Station);      
                        free(Start_Station);
                        free(End_Station);
                        Start_Station = NULL;
                        End_Station = NULL;
                        // read start station again
                        printf("Start station: ");                    
                        Start_Station = get_station();
                        if (Start_Station == NULL)
                        {
                                printf("Unable to allocate memory.\n");
                                destroy_graph(&G);
                                exit(3);
                        }
                }
                // the start station is not included in the list of start station
                else                                                  
                {
                        free(Start_Station);
                        Start_Station = NULL;
                        printf("No such station.\n");
                        // read start station again
                        printf("Start station: ");                    
                        Start_Station = get_station();
                        if (Start_Station == NULL)
                        {
                                printf("Unable to allocate memory.\n");
                                destroy_graph(&G);
                                exit(3);
                        }
                }
        }
        // free memory if user input nothing with a '\n' when inputing the name of start station
        free(Start_Station);                    
        // free the whole graph memory
        destroy_graph(&G);                      
        return 0;
}


// to read the name of end stations by using pointers and return integer number to indicate if everything is ok
int read_end_station(FILE *fp, PtrToGraph G)
{
        int i, j;
        // != FILE *, but it is a file position counter like a counter for unit
        int file_position_counter = 0;              
        int name_length_multiple = 1, station_num_multiple = 1;
        G->end_station = (char**)malloc(STATIONS * sizeof(char*));
        // if unable to allocate memory.
        if (G->end_station == NULL)
        {
                return 3;                           
        }
        for (i = 0; i < STATIONS; ++i)
        {
                G->end_station[i] = (char*)malloc(NAME_LEN * sizeof(char));
                if (G->end_station[i] == NULL)     
                {
                        return 3;
                }
        }
        char ch = getc(fp);                
        while(ch != '\n')
        {
                if (file_position_counter % 2 == 0)
                {        
                        // if reach the end of file, quit the function with 0
                        if (ch == EOF)                      
                        {
                                return 0;               
                        }
                        // should be ','
                        // else will be invalid distances file.
                        else if (ch != ',')         
                        {
                                return 2;           
                        }
                        ch = getc(fp);
                        // if ch reaches the end of file instead of '\n',
                        // or the name of station starts with a space ' ', 
                        if (ch == EOF || (isspace(ch) == 1))       
                        {                                        
                                return 2;                         
                        }
                }
                // file_position_counter % 2 == 1
                else                                
                {
                        // load data from file (for distance and names of start staions)
                        if (G->end_station_num >= (station_num_multiple * STATIONS))      
                        {
                                station_num_multiple++;
                                G->end_station = (char**)realloc(G->end_station, sizeof(char*) * station_num_multiple * STATIONS);
                                if (G->end_station == NULL)
                                {
                                        return 3;
                                }
                                for (i = G->end_station_num; i < station_num_multiple * STATIONS; ++i)
                                {
                                        G->end_station[i] = (char*)malloc(NAME_LEN * sizeof(char));
                                        if (G->end_station[i] == NULL)
                                        {
                                                return 3;
                                        }
                                }
                        }
                        int element = 0;
                        // if there are two continuous ',', return 2
                        if (ch == ',')              
                        {
                                return 2;
                        }
                        while(ch != ',' && ch != '\n')
                        {
                                if (ch == EOF)
                                {
                                        return 0;
                                }
                                if (element >= (name_length_multiple * NAME_LEN -1))
                                {
                                        name_length_multiple++;
                                        G->end_station[G->end_station_num] = (char*)realloc(G->end_station[G->end_station_num], sizeof(char) * name_length_multiple * NAME_LEN);
                                        if (G->end_station[G->end_station_num] == NULL)
                                        {
                                                return 3;
                                        }
                                }
                                G->end_station[G->end_station_num][element++] = ch;
                                // get the next character
                                ch = getc(fp);                    
                        }

                        G->end_station[G->end_station_num][element] = '\0';
                        G->end_station_num++;
                }
                // move to the next unit
                file_position_counter++;                          
        }
        // to check if there are two cities with the same name, if so, then return 2
        for (i = 0; i < G->end_station_num - 1; ++i)              
        {
                for (j = i + 1; j < G->end_station_num; ++j)
                {
                        if (strcmp(G->end_station[i], G->end_station[j]) == 0)
                        {
                                return 2;
                        }
                }
        }       
        return 0;
}

// to read the name of start stations and distance at the same time by using pointers 
// and return integer number to indicate if everything is ok
int read_start_station_distance(FILE *fp, PtrToGraph G)
{
        int i, j;
        int name_length_multiple = 1, station_num_multiple = 1;      
        G->start_station = (char**)malloc(STATIONS * sizeof(char*));
        if (G->start_station == NULL)
        {
                return 3;
        }
        for (i = 0; i < STATIONS; ++i)
        {
                G->start_station[i] = (char*)malloc(NAME_LEN * sizeof(char));
                if (G->start_station[i] == NULL)
                {
                        return 3;
                }       
        }
        // allocate only one line at first
        G->distances = (int**)malloc(sizeof(int*));     
        if (G->distances == NULL)
        {
                return 3;
        }
        G->distances[0] = (int*)malloc(sizeof(int) * G->end_station_num);
        if (G->distances[0] == NULL)
        {
                return 3;
        }
        char ch = getc(fp);
        if (ch == EOF)
        {
                return 0;
        }
        // start to load data including names of start stations and distance
        for(i = 0; ch != EOF; ++i)                      
        {
                // if the first char of each line is ',' , then the file is invalid
                if (ch == ',')                          
                {
                        return 2;
                }
                if (G->start_station_num >= (station_num_multiple * STATIONS))
                {                  
                        station_num_multiple++;
                        G->start_station = (char**)realloc(G->start_station, sizeof(char*) * station_num_multiple * STATIONS);
                        if (G->start_station == NULL)
                        {
                                return 3;
                        }
                        for (i = G->start_station_num; i < station_num_multiple * STATIONS; ++i)
                        {
                                G->start_station[i] = (char*)malloc(NAME_LEN * sizeof(char));
                                if (G->start_station[i] == NULL)
                                {
                                        return 3;
                                }
                        }         
                }
                int element = 0;
                // if ch meets the first EOF when reading the name of a station, return 2(invalid file)
                if (ch == EOF)                 
                {
                        return 0;
                } 
                while(ch != ',' && ch != EOF && ch != '\n')
                {
                        // not enough memory, use dynamic memory allocation
                        if (element >= (name_length_multiple * NAME_LEN - 1))    
                        {
                                name_length_multiple++;
                                G->start_station[G->start_station_num] = (char*)realloc(G->start_station[G->start_station_num], sizeof(char) * name_length_multiple * NAME_LEN);
                        }
                        if (G->start_station[G->start_station_num] == NULL)
                        {
                                // not enough memory
                                return 3;                                        
                        }
                        G->start_station[G->start_station_num][element++] = ch;
                        ch = getc(fp);
                }
                // make it a string
                G->start_station[G->start_station_num][element] = '\0';          
                if (ch != ',' && G->start_station[G->start_station_num][0] != '\0')
                {
                        // if ch have read somthing which is valid character instead of '\n' or EOF, but without a ',' at the end, than return 2    
                        return 2; 	                
                }
                if (ch == '\n' || ch == EOF)          
                {
                        // blank lines (lines with no characters)
                        while((ch = getc(fp)) == '\n')          
                        {                                       
                                // at the end of the file should be ignored
                                continue;
                        }
                        // it should be EOF at the end of each file, if not, then quit the function
                        if (ch != EOF)                          
                        {
                                return 2;
                        }
                        else
                        {
                                return 0;
                        }
                }
                G->start_station_num++;
                if (G->start_station_num > 1)
                {
                        // not enough memory, use dynamic memory allocation
                        G->distances = (int**)realloc(G->distances, sizeof(int*) * (G->start_station_num));        
                        if (G->distances == NULL)
                        {
                                // memory allocation failure
                                return 3;                                
                        }
                        G->distances[G->start_station_num - 1] = (int*)malloc(sizeof(int) * G->end_station_num);
                        if (G->distances[G->start_station_num - 1] == NULL)
                        {
                                // memory allocation failure
                                return 3;                                
                        }                                     
                }      
                for (j = 0; j < G->end_station_num; ++j)
                {
                        // a temporary array
                        char tmp_data[10];                               
                        ch = getc(fp);
                        // it should be numbers or ',' or '\n'
                        if ((ch < '0' || ch > '9') && ch != ',' && ch != '\n' && j != G->end_station_num - 1)   
                        {
                                return 2;
                        }
                        int k = 0;
                        // if ch meets a number character, then store it and read the next one until something else
                        while(ch >= '0' && ch <= '9')                    
                        {
                                tmp_data[k++] = ch;
                                ch = getc(fp);
                        }
                        // make it a string 
                        tmp_data[k] = '\0';                              
                        // empty       
                        if (tmp_data[0] == '\0')                           
                        {
                                G->distances[G->start_station_num - 1][j] = INFINITY;
                                continue;                                        
                        }
                        if (ch == ',' || ((j == G->end_station_num - 1) && (ch == '\n' || ch == EOF)))
                        {                                  
                                char *stop;
                                // convert the string into a number
                                int iszero = (int)strtol(tmp_data, &stop, 10);   
                                // the distance should be a positive non-zero integer or empty
                                if (iszero == 0)                                 
                                {
                                        return 2;
                                }                                                
                                G->distances[G->start_station_num - 1][j] = iszero;
                        } 
                        // ch != ',' && (j != G->end_station_num - 1 || (ch != '\n' && ch != EOF))
                        else                                    
                        {
                                return 2;
                        }
                }
                if (ch != '\n' && ch != EOF)                    
                {                                               
                        // reach the end of each line, if ch is '\n' means there is another start station
                        // if ch is EOF, means it is the end of file, the file is valid
                        // return 2 if the condition is false, which means it is not a '\n' or EOF
                        return 2;                               
                }  
                ch = getc(fp);                 
        }
        // to check if there are two cities with the same name, if so, then return 2
        for (i = 0; i < G->start_station_num - 2; ++i)          
        {
                for (j = i + 1; j < G->start_station_num; ++j)
                {
                        if (strcmp(G->start_station[i], G->start_station[j]) == 0)
                        {
                                // return 2 if there are two citis which have the same name
                                return 2;                       
                        }
                }
        }
        // return 0 if everthing is fine, the format of file is valid
        return 0;                                               
}

// to input
char *get_station(void)
{
       int Len = 10;
       int cnt = 0;
       char ch;
       // use dynamic memory allocation
       char *station = (char*)malloc(sizeof(char) * Len);       
       do
       {
                ch = getchar();
                cnt++;
                if (cnt >= Len)
                {
                        // if the length exceeds the boundary,
                        Len += 10;                               
                        // then alloc 10 more char memory for the array
                        station = (char*)realloc(station, sizeof(char) * Len);        
                        if (station == NULL)
                        {
                                return NULL;
                        }
                }
                station[cnt - 1] = ch;       
       }while (ch != '\n');
       // make it a string 
       station[cnt - 1] = '\0';                                        
       return station;
}

// to check if the name is included in the stirng array, return 1 if the name is included in the station list
int isIncluded(char **station_name, int station_num, char *name)
{
        int i;
        // traverse the whole 2D string array
        for (i = 0; i < station_num; ++i)                       
        {
                if (strcmp(station_name[i], name) == 0)                   
                {
                        // return 1 if the name is included in the City_Name
                        return 1;                               
                }
        }
        // return 0 if the name is not in the City_Name
        return 0;                                               
}

// to tag each city a number from 0 to (City_Num - 1)
int convert_name_to_num(char **City_Name, int City_Num, char *name)
{
        int i;
        // traverse the whole 2D string array
        for (i = 0; i < City_Num; ++i)                          
        {
                if (strcmp(City_Name[i], name) == 0)
                {
                        // return the (serial)number
                        return i;                              
                }
        }
        // return -1 if it is not found
        return -1;                                              
}

// to find the start station in the name list of end station
int convert(char **City_Name, int City_Num, char *name)
{
            int i;
            for (i = 0; i < City_Num; ++i)
            {
                    if (strcmp(City_Name[i], name) == 0)
                    {
                            return i;
                    }
            }
            return -1;
}

// use Dijkstra algorithm to get the shourtest route for start station to end station and print out result
// the general idea of Dijkstra is like sorting algorithm, each time select a shortest path and 
// change the vaiables which are related to it
void Dijkstra(Graph G, char *start, char *end)
{
        // to tag start station a number from 0 to (start_station_num - 1)  
        int Start = convert_name_to_num(G.start_station, G.start_station_num, start);          
        // to tag end station a number from 0 to (end_station_num - 1)
        int End = convert_name_to_num(G.end_station, G.end_station_num, end);                  
        int i, j, m, k = -1;                                  
        // in terms of different cases, start stations may be not the same as end stations, so it should be found in start stations      
        int m_in_start = 0;                                      
        // indicates the previous station of one station from start station
        int prev_station[G.end_station_num];                     
        // indicates the shortest route from start station
        int shortest_dist[G.end_station_num];                    
        // minimum distance in each iteration
        int min_dist;                                            
        // stores the shortest distance temporarily 
        int tmp_dist;                                            
        // indicates if the shortest route from start to one station determined        
        int isDetmd[G.end_station_num];                          
        for (i = 0; i < G.end_station_num; ++i)
        {
                // load the distance data
                shortest_dist[i] = G.distances[Start][i];        
                // each station's previous station should be the start station. 
                //Here we define the start station as the last element in the array no matter what
                prev_station[i] = G.end_station_num;             
                // at the very beginning, the shortest route to each station is not determined
                isDetmd[i] = 0;                                  
        }
        // if the name of start station is included in the list of end stations
        if (isIncluded(G.end_station, G.end_station_num, start) == 1)                          
        {
                    // the distance from start station to iestlf should be 0
                    shortest_dist[convert(G.end_station, G.end_station_num, start)] = 0;           
                    // the shortest route start station to itself should be determined 
                    isDetmd[convert(G.end_station, G.end_station_num, start)] = 1;                 
        }
        else                                                                                   
        {                               
                    // the name of start station is not included
                    // put the start station at the end of the array and initialize it                                                       
                    shortest_dist[G.end_station_num] = 0;
                    isDetmd[G.end_station_num] = 1;
        }
        
        // core code
        // traverse G.start.station_num times
        for (i = 0; i < G.start_station_num; ++i)                 
        {
                // at first of each iteration, the minimum distance should be infinity(999999/unconnected)
                min_dist = INFINITY;                              
                for (j = 0; j < G.end_station_num; j++)
                {
                        if (isDetmd[j] == 0 && shortest_dist[j] < min_dist)
                        {
                                if (isIncluded(G.start_station, G.start_station_num, G.end_station[j]) == 0)
                                {
                                        continue;
                                }
                                // G.start_station includes G.end_station[j], find the number in start stations
                                for (m = 0; m < G.start_station_num; ++m)                       
                                {
                                        if((strcmp(G.start_station[m], G.end_station[j]) == 0))
                                        {
                                                m_in_start = m;
                                                break; 
                                        }
                                }
                                k = j;
                                // change the minimum distance 
                                min_dist = shortest_dist[j];         
                        }
                }
                // if k remains -1, that means there is no shortest way, jump to next iteration
                if (k == -1)                                         
                {
                        continue;                                      
                }
                // the shortest route from start to k is determined
                isDetmd[k] = 1;                                      
                // update prev_station[] and shortest_dist[]
                for (j = 0; j < G.end_station_num; j++)              
                {
                        if (G.distances[m_in_start][j] == INFINITY)
                        {
                                tmp_dist = INFINITY;
                        }
                        else
                        {  
                                tmp_dist = min_dist + G.distances[m_in_start][j];
                        }
                        // if there is a shorter path the its distance is not determined, then update
                        if (isDetmd[j] == 0 && (tmp_dist < shortest_dist[j]))        
                        {
                                prev_station[j] = k;
                                shortest_dist[j] = tmp_dist;
                        }
                }
        }
        // if there are possible path
        if (shortest_dist[End] != INFINITY)                          
        {
                int route[G.end_station_num];
                int current = End;
                i = 0;
                // find each station's previous station and store them
                while (current != G.end_station_num)                 
                {
                        route[i] = current;
                        current = prev_station[current];      
                        ++i;
                } 
                // print out information about the best route from start to end station and its distance and cost
                printf("From %s\n", start);                          
                if (i == 1)    
                {
                        // there is no intermediate station
                        printf("direct\n");                          
                }
                else
                {
                        printf("via\n");                               
                }
                i--;
                // the number of intermediate stations  
                int inter_city = i;                                     
                while(i != -1)
                {
                        // has not arrived the end station
                        if (route[i] != End)                         
                        {
                                printf("%s\n", G.end_station[route[i--]]);
                                continue;
                        }
                        printf("To %s\n", end);
                        i--;
                }
                printf("Distance %d km\n", shortest_dist[End]);
                float cost = shortest_dist[End] * 1.2 + inter_city * 25;
                // if the result is not a whole number then it should be rounded up to the next nearest integer
                if ((cost - (int)cost) >= 0.01)                
                {      
                        // type cast float into int and print the cost
                        printf("Cost %d RMB\n", (int)(cost + 1));    
                }
                else
                {
                        // type cast float into int and print the cost
                        printf("Cost %d RMB\n", (int)cost);          
                }
        }
        else                                                         
        {
                // if the distance is INFINITY, the shortest journey is not found
                printf("No possible journey.\n");                    
        }
}

// to free memory allocated to graph including distance, start_station and end_station
void destroy_graph(PtrToGraph G)
{
        int i;
        // free the array of start stations
        for (i = 0; i < G->end_station_num; ++i)        
        {
                // make sure it is not NULL
                if (G->end_station[i] != NULL)          
                {
                        free(G->end_station[i]);        
                }  
        }
        // make sure it is not NULL
        if (G->end_station != NULL)                     
        {
                free(G->end_station);
        }
        // free the array of end stations
        for (i = 0; i < G->start_station_num; ++i)      
        {
                // make sure it is not NULL
                if (G->start_station[i] != NULL)        
                {
                        free(G->start_station[i]);
                }
        }
        // make sure it is not NULL
        if (G->start_station != NULL)                   
        {
                free(G->start_station);
        }
        // free the array of distance
        for (i = 0; i < G->start_station_num; ++i)      
        {
                // make sure it is not NULL
                if (G->distances[i] != NULL)            
                {
                        free(G->distances[i]);
                }
        }
        // make sure it is not NULL
        if (G->distances != NULL)                       
        {
            free(G->distances);
        }
}


