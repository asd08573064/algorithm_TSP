#include <utility>   
#include <stdio.h>
#include <stdlib.h>
#include <string>       
#include <iostream>
#include <time.h>
#include <queue>
#include <vector>
#include <string>

using namespace std;
int V;
vector<int> ans;
FILE* wr;
int UB = INT_MAX;

class carrier
{
    public:
        carrier(int i_, int j_, int min_){
            this->i = i_;
            this->j = j_;
            this->min = min_;
        };
        int i;
        int j;
        int min;
};

class Node {
    public:
        int **table;
        int *path;
        int lowerbound;
        int size;
        int level;
        bool *check;
        string flag;
        Node(int s, int lb, int lv){
            
            this->size = s;
            this->lowerbound = lb;
            this->level = lv;
            this->check = new bool[s];
            
        };
        void reduce();
        void reduce_table_with(int i, int j);
        void reduce_table_without(int i, int j);
};




void Node::reduce(){
    for(int i = 0; i < size; i++){
        int min = INT_MAX;
        for(int j = 0; j < size; j++){
            if(table[i][j] < min && table[i][j] != -1){
                min = table[i][j];
            }
        }
        if(min != 0 && min != INT_MAX){
            lowerbound += min;
            for(int j = 0; j < size; j++){
                if(table[i][j] != -1){
                    table[i][j] -= min;
                }
            }
        }
    }

    for(int j = 0; j < size; j++){
        int min = INT_MAX;
        for(int i = 0; i < size; i++){
            if(table[i][j] < min && table[i][j] != -1 ){
                min = table[i][j];
            }
        }
        if(min != 0 && min != INT_MAX){
            lowerbound += min;
            for(int i = 0; i < size; i++){
                if (table[i][j] != -1){
                    table[i][j] -= min;
                }
            }
        }
    }
}

void Node::reduce_table_with(int i, int j){
    for(int k = 0; k < size; k++){
        table[i][k] = -1;
        table[k][j] = -1;
    }
    table[j][i] = -1;
}

void Node::reduce_table_without(int i, int j){
    table[i][j] = -1;
}

void check_cycle(Node *current){
    int size = current->size;
    bool *check = new bool[size];
    memset(check, false, size);
    for(int k = 0; k < size; k++){
        int *check_ = new int[size];
        for(int i = 0; i < size; i++){
            check_[i] = -1;
        }
        if(!check[k] && current->path[k] != -1){
            int now = k;
            check_[0] = now;
            for(int i = 1; i < size && current->path[now] != -1; i++){
                check[now] = true;
                check_[i] = current->path[now];
                now = current->path[now];
            }
            for(int i = 0; i < size-1 && check_[i] != -1; i++){
                for(int j = i+1; j  < size && check_[j] != -1; j++){
                    current->table[check_[j]][check_[i]] = -1;
                }
            }
        }
        
    }
}


Node *new_node(int **table, int size, int lb, int level){
    Node *new_node = new Node(size, lb, level);
    new_node->table = new int*[size];
    new_node->path = new int[size];
    for(int i = 0; i < size; ++i){
        new_node->table[i] = new int[size];
    }
    for(int i = 0; i < size; i++){
        new_node->path[i] = -1;
        for(int j = 0; j < size; j++){
            new_node->table[i][j] = table[i][j];
        }
    }
    return new_node;
}

Node *new_node_withtable(Node *old, int size, int lb, int level){
    Node *new_node = new Node(size, lb, level);
    new_node->table = new int*[size];
    new_node->path = new int[size];
    for(int i = 0; i < size; ++i){
        new_node->table[i] = new int[size];
    }
    for(int i = 0; i < size; i++){
        new_node->path[i] = old->path[i];
        for(int j = 0; j < size; j++){
            new_node->table[i][j] = old->table[i][j];
        }
    }
    return new_node;
}

carrier find_min(Node *current){
    int size = current->size;
    int **table = current->table;
    int i_index = -1;
    int j_index = -1;
    int i_min = INT_MAX;
    int j_min = INT_MAX;
    int max = INT_MIN;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(table[i][j] != -1){
                int min = INT_MAX;
                int iindex;
                int jindex;
                for(int k = 0; k < size; k++){
                    if(table[i][k] < min && table[i][k] != -1 && k != j){
                        j_min = table[i][k];
                        min = j_min;
                    }
                }
                min = INT_MAX;
                for(int k = 0; k < size; k++){
                    if(table[k][j] < min  && table[k][j] != -1 && k != i){
                        i_min = table[k][j];
                        min = i_min;
                    }
                }
                if(j_min + i_min > max){
                    if(j_min == INT_MAX)
                        j_min = 0;
                    if(i_min == INT_MAX)
                        i_min = 0;
                    max = j_min + i_min;
                    i_index = i;
                    j_index = j;
                }
            }
        }
    }
    return carrier(i_index,j_index,max);
}

struct comp {
    bool operator()(const Node* lhs, const Node* rhs) const
    {
        return lhs->lowerbound > rhs->lowerbound;
    }
};

void solve(Node *root){
    priority_queue<Node*, std::vector<Node*>, comp> pq;
    Node *check_node = new_node(root->table,root->size,0,0);
    root->reduce();
    pq.push(root);
    while(!pq.empty()){
        Node* min = pq.top();
        int size = min->size;
        pq.pop();

        bool all_minusone = true;

        for(int i = 0; i < size ;i++){
            for(int j = 0; j < size; j++){
                if(min->table[i][j] != -1){
                    all_minusone = false;
                    i = size;
                    break;
                }
            }
        }

        
        if(all_minusone && min->level != root->size){
            continue;
        }

        if(min->lowerbound > UB || (min->lowerbound == UB && min->level != root->size)){
            continue;
        }

        if(min->level == root->size){
            fprintf(wr, "Solution : ");
            cout << "Solution : ";
            if(UB >= min->lowerbound){
                UB = min->lowerbound;
                int now = 0;
                int next = 0;
                int real_ub = 0;
                fprintf(wr, "0 ");
                cout << "0";
                for(int i = 0; i < root->size; i++){
                    real_ub += check_node->table[i][min->path[i]];
                    fprintf(wr, "-> %d ", min->path[now]);
                    cout << "->" << min->path[now];
                    now = min->path[now];
                }
                UB = real_ub;
                cout<<"UB = " << real_ub << endl;
            }
            continue;
        }

        if(min->lowerbound < UB){
            carrier data = find_min(min);    
            Node *node_with = new_node_withtable(min, size, min->lowerbound, min->level+1);
            Node *node_without =  new_node_withtable(min, size, data.min + min->lowerbound, min->level);
            node_without->reduce_table_without(data.i,data.j);
            node_with->reduce_table_with(data.i,data.j);
            node_with->path[data.i] = data.j;
            if(node_with->level < root->size-1)
                check_cycle(node_with);
            node_with->reduce();
            pq.push(node_with);
            pq.push(node_without);
        }
    }
}


int travllingSalesmanProblem(int **graph, int s) 
{ 
	vector<int> vertex; 
	
	for (int i = 0; i < V; i++) 
		if (i != s) 
			vertex.push_back(i); 
	int min_path = INT_MAX; 
	do { 
		int current_pathweight = 0; 
		int k = s; 
		for (int i = 0; i < vertex.size(); i++) { 
			current_pathweight += graph[k][vertex[i]]; 
			k = vertex[i]; 
		} 
		current_pathweight += graph[k][s]; 
		if(min_path > current_pathweight){
			min_path = current_pathweight;
			ans = vertex;
		}
	} while (next_permutation(vertex.begin(), vertex.end())); 
	return min_path; 
} 


int main(){
    FILE* f;
	f = fopen("input15.txt", "r");
	wr = fopen("BB.txt", "w");
    int cities;
    int **table;
    fscanf(f, "%d", &cities);
    cout << cities << endl;
    table = new int*[cities];
    for(int i = 0; i < cities; ++i) {
        table[i] = new int[cities];
    }
    cout << cities << endl;
    for(int i = 0; i < cities; i++){
        for(int j = 0; j < cities; j++){
            fscanf(f, "%d", &table[i][j]);
            cout << table[i][j] << ' ';
        }
        cout << '\n';
    }
    Node* root = new_node(table, cities, 0, 0);
    clock_t start = clock();
    solve(root);
    clock_t end = clock();
    fprintf(wr, "\ncost     : %d\n", UB);
    cout << "UB = " << UB << '\n';
	double dur = (double)(end - start) / CLOCKS_PER_SEC;
	fprintf(wr ,"time     : %.7f S\n", dur);
     cout << "BB : " << dur << '\n';


    FILE* f_bb;
	FILE* wr_bb;
	f_bb = fopen("input15.txt", "r");
	wr_bb = fopen("BF.txt", "w");
    
    fscanf(f_bb, "%d", &V);
    int **graph = new int*[V];
    for(int i = 0; i < V; ++i) {
        graph[i] = new int[V];
    }
    
    cout << "done!\n";
    for(int i=0; i<V; i++){
		for(int j=0; j<V; j++){
			fscanf(f_bb, "%d", &graph[i][j]);
		}
	}
  
	int s = 0; 
    start = clock();
    int bb = travllingSalesmanProblem(table, s);
    end = clock();
	cout << bb << endl; 
    fprintf(wr_bb, "0 ");
	for(int i = 0; i < V; i++){
		fprintf(wr_bb, "-> %d ", ans[i]);
	}
    fprintf(wr_bb, "\ncost     : %d\n", bb);
    cout << "UB = " << UB << '\n';
    dur = (double)(end - start) / CLOCKS_PER_SEC;
	fprintf(wr_bb ,"time     : %.7f S\n", dur);
    cout << "BF : " << dur << '\n';



}
