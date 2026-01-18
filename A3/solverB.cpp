#include<bits/stdc++.h>
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

ifstream input;
ofstream output;

string test;
vector<string> ans;

void print_output(string output_string)
{
    output.open(test+".mapping");
    output<<output_string;
    output.close();
}

bool solve_sat(int nodes,vector<vector<int>> sat_constraints,vector<int> actual_nodes,int clique_size)
{
    shuffle(sat_constraints.begin(),sat_constraints.end(),rng);
    
    output.open("satinput_A3");
    output<<"p cnf "<<nodes<<' '<<(int)sat_constraints.size()<<'\n';
    for(int i=0;i<(int)sat_constraints.size();i++)
    {
        for(auto x:sat_constraints[i]) output<<x<<' ';
        output<<0<<'\n';
    }
    output.close();

    int n=(int)actual_nodes.size()-1;
    bool poss=false;
    string string_output;

    system("./minisat satinput_A3 satoutput_A3");

    input.open("satoutput_A3");
    string sat; input>>sat;
    if(sat=="UNSAT")
    {
        poss=false;
        string_output="0\n";
    }
    else
    {
        poss=true;
        int x; input>>x;
        vector<int> ans;
        while(x!=0)
        {
            if(x>=1 && x<=n) ans.push_back(actual_nodes[x]);
            input>>x;
        }

        string_output="#1\n";
        for(int i=0;i<(int)ans.size();i++)
        {
            if(i != ((int)ans.size()-1)) string_output += (to_string(ans[i])+" ");
            else string_output += (to_string(ans[i]));
        }
    }
    input.close();

    ans[clique_size]=string_output;

    return poss;
}

bool check(vector<vector<int>> adj,vector<int> deg,int n,int m,int clique_size)
{
    // cout<<"Solving for clique size: "<<clique_size<<'\n';
    //only consider nodes having degree >= clique_size-1
    vector<bool> remove_nodes(n+1,false);
    queue<int> qq;

    for(int i=1;i<=n;i++)
    {
        if(deg[i] < clique_size-1) 
        {
            qq.push(i);
            remove_nodes[i]=true;
        }
    }

    while(!qq.empty())
    {
        int f=qq.front();
        qq.pop();
        for(int x=1;x<=n;x++)
        {
            if(adj[f][x]==0) continue;
            if(remove_nodes[x]) continue;
            deg[x]--;
            if(deg[x] < clique_size-1) 
            {
                qq.push(x);
                remove_nodes[x]=true;
            }
        }
    }

    int idx=1;
    vector<int> actual_nodes;
    actual_nodes.push_back(0);

    for(int i=1;i<=n;i++)
    {
        if(remove_nodes[i]) continue;
        actual_nodes.push_back(i);
        idx++;
    }

    n=idx-1;

    //so now we consider only the relevant nodes

    vector<vector<int>> new_adj(n+1,vector<int>(n+1));
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=n;j++) new_adj[i][j]=adj[actual_nodes[i]][actual_nodes[j]];
    }

    vector<vector<int>> sat_constraints;

    for(int i=1;i<=n;i++)
    {
        //SAT problem on modified graph
        for(int j=i+1;j<=n;j++)
        {
            if(new_adj[i][j]==0) sat_constraints.push_back({-i,-j});
        }
    }

    idx=n+1;
    vector<vector<int>> mapping(n+1,vector<int>(clique_size+1));
    for(int i=0;i<=n;i++)
    {
        for(int j=0;j<=min(i,clique_size);j++) 
        {
            mapping[i][j]=idx;
            idx++;
        }
    }

    //make dp(i,0)=true for all i and dp(0,j)=false for all j>0
    for(int i=0;i<=n;i++) sat_constraints.push_back({mapping[i][0]});

    //A<->B : (~AvB) ^ (Av~B)
    for(int i=1;i<=n;i++)
    {
        int p;
        for(int j=1;j<min(clique_size+1,i);j++)
        {
            //dp(i,j) = (dp(i-1,j-1) ^ i) v dp(i-1,j)
            //assume dp(i-1,j-1) ^ i as some "new node" p
            // p <-> dp(i-1,j-1) ^ i
            // (~p v dp(i-1,j-1)) ^ (~p v i) ^ (p v ~dp(i-1,j-1) v ~i)

            sat_constraints.push_back({-mapping[i][j],mapping[i-1][j-1],mapping[i-1][j]});
            sat_constraints.push_back({-mapping[i][j],i,mapping[i-1][j]});
            sat_constraints.push_back({mapping[i][j],-mapping[i-1][j]});
            sat_constraints.push_back({mapping[i][j],-mapping[i-1][j-1],-i});
        }   

        //dp(i,i) <=> i^dp(i-1,i-1)
        //(~(i,i) v i)^(~(i,i) v (i-1,i-1))^((i,i)v~iv~(i-1,i-1))
        if(i<=clique_size)
        {
            //so here j=i and mapping[i-1][j] is false
            sat_constraints.push_back({-mapping[i][i],mapping[i-1][i-1]});
            sat_constraints.push_back({-mapping[i][i],i});
            sat_constraints.push_back({mapping[i][i],-mapping[i-1][i-1],-i});
        }
    }

    //we need mapping[n][clique_size] to be true
    if(clique_size<=n) sat_constraints.push_back({mapping[n][clique_size]});
    else if(clique_size>n || n==0)  return false;

    return solve_sat(idx-1,sat_constraints,actual_nodes,clique_size);
}

int main(int argc,char* argv[])
{
    ios_base::sync_with_stdio(0);

    test=(string)argv[1];
    input.open(test+(string)".graph");

    int n,m;
    input>>n>>m;

    ans.resize(n+1,"");
    vector<vector<int>> adj(n+1,vector<int>(n+1));
    vector<int> deg(n+1);

    for(int i=1;i<=m;i++)
    {
        int u,v;
        input>>u>>v;
        adj[u][v]=1;
        adj[v][u]=1;
        deg[u]++; deg[v]++;
    }

    input.close();

    int right=1;
    while((right*(right-1))/2 <= m) right++;
    right--;
    int left=1;

    while(left<right)
    {
        int mid=(left+right)/2+1;
        if(check(adj,deg,n,m,mid)) left=mid;
        else right=mid-1;
    }

    assert(left==right);
    if(ans[left] != "") print_output(ans[left]);
    else
    {
        check(adj,deg,n,m,left);
        print_output(ans[left]);
    }

	return 0;
}