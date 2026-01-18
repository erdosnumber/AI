#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <bits/stdc++.h>

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<double> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning

public:
	// Constructor- a node is initialised with its name and its categories
    map<string, int> idx;
    Graph_Node(string name,int n,vector<string> vals){
		Node_Name=name;
	
		nvalues=n;
		values=vals;
        for (int i = 0; i < values.size(); ++i){
            idx[values[i]] = i;
        }
		
	}
	string get_name(){
		return Node_Name;
	}

	vector<int> get_children(){
		return Children;
	}

	vector<string> get_Parents(){
		return Parents;
	}

	vector<double> get_CPT(){
		return CPT;
	}
	int get_nvalues(){
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<double> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }



};


 // The whole network represted as a list of nodes
class network{


public:
    list <Graph_Node> Pres_Graph;
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
// get the node at nth index
    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
    
            cout<<"node not found\n";
        return listIt;
    }
	

};

network read_network(string filename)
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile(filename); 
  	string temp;
  	string name;
  	vector<string> values;
  	
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		
      		
      		ss.str(line);
     		ss>>temp;
     		
     		
     		if(temp.compare("variable")==0)
     		{
                    
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					
     					
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);
     					
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				int pos=Alarm.addNode(new_node);

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    
     				ss>>temp;
     				ss>>temp;
     				
                    list<Graph_Node>::iterator listIt;
                    list<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<double> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
                        
     					curr_CPT.push_back(atof(temp.c_str()));
     					
     					ss2>>temp;
                       
                        

    				}
                    
                    listIt->set_CPT(curr_CPT);


     		}
            else
            {
                
            }
     		
     		

    		
    		
    	}
    	
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}


int main(int argc, char* argv[]){

    auto start_time=chrono::high_resolution_clock::now();
	network Alarm;
    string filename_alarm=(string)argv[1];
	Alarm=read_network(filename_alarm);
    
// Example: to do something
    string filename_data=(string)argv[2];
    string line;
    ifstream myfile(filename_data);

    vector<vector<string>> data;

    while (std::getline(myfile, line)){
        data.push_back(vector<string>());
        stringstream iss(line); 
        string word;
        while (iss >> word) {
            data.back().push_back(word);
        }
    }

    myfile.close();


    string header_line;
    ifstream header_file(filename_alarm);

    vector<string> header_data;

    for (int i = 0; i < 154 && getline(header_file, header_line); ++i) {
        header_data.push_back(header_line);
    }

    header_file.close();

    map<string, int> idx;
    for (auto &node : Alarm.Pres_Graph){
        auto name = node.get_name();
        idx[name] = Alarm.get_index(name);
    }


    for (auto &node : Alarm.Pres_Graph){
        auto parents = node.get_Parents();
        int size = node.get_nvalues();
        for (auto &p : parents){
            size *= Alarm.get_nth_node(idx[p])->get_nvalues(); // get size of CPT
        }
        node.set_CPT(vector<double>(size, 1/double(size)));
    }
    
    vector<pair<vector<string>, double>> gen_data; // data with weights

    auto learn = [&](){
        for (auto &node : Alarm.Pres_Graph){
            auto parents = node.get_Parents();
            parents.push_back(node.get_name());

            vector<Graph_Node> G; // make vector of parents + current node
            for (auto parents : parents){
                G.push_back(*Alarm.search_node(parents));
            }

            vector<double> count(node.get_CPT().size(), 0); // initialize with 1 for smoothening

            for (auto &[row, w] : gen_data){
                int i = 0, sz = node.get_CPT().size();
                for (auto &p : G){
                    sz /= p.get_nvalues(); // getting the index of the CPT to which this data corresponsds using basic maths
                    i += sz * p.idx[row[idx[p.get_name()]]];
                }
                count[i] += w; // add the corresponding weight
            }
 
            vector<double> CPT(count.size());
            for (int i = 0; i < count.size() / node.get_nvalues(); ++i){
                double tot = 0;
                for (int j = 0; j < node.get_nvalues(); ++j){
                    tot += count[i * node.get_nvalues() + j]; // get total count for different values of current node
                }
                if (tot==0)
                {
                    for (int j = 0;j<node.get_nvalues();j++)
                    {
                        CPT[i*node.get_nvalues()+j]= ((double)1)/((double)node.get_nvalues());
                    }
                }
                else
                {
                    double smooth=0.001*tot;
                    for (int j = 0; j < node.get_nvalues(); ++j){
                        CPT[i * node.get_nvalues() + j] = (count[i * node.get_nvalues() + j]+smooth) / (double(tot)+smooth*node.get_nvalues()); // normalize probabilities to 1
                    }
                }
            }
            node.set_CPT(CPT);
        }
    };

    auto find_mapping=[&](int q,vector<string> &row)->int
    {
        //row represents row of data
        auto node = *Alarm.get_nth_node(q);
        auto parents = node.get_Parents();

        vector<Graph_Node> G; // make vector of parents + current node
        for (auto parents : parents){
            G.push_back(*Alarm.search_node(parents));
        }

        int i = 0, sz = node.get_CPT().size();
        for (auto &p : G){
            sz /= p.get_nvalues(); // getting the index of the CPT to which this data corresponsds using basic maths
            i += sz * p.idx[row[idx[p.get_name()]]];
        }

        assert(sz == node.get_nvalues());
        return i;
    };

    auto infer = [&](){
        gen_data.clear();
        for (auto &row : data){
            bool found = false; int q = 0;
            for (auto &word : row){
                found |= (word == "\"?\"");
                if (found){
                    break;
                }
                q++;
            }
            if (!found){
                gen_data.push_back({row, 1});
                continue;
            }

            auto node = *Alarm.get_nth_node(q);
            int i=find_mapping(q,row);
            vector<double> probability_values(node.get_nvalues());
            vector<vector<string>> new_rows(node.get_nvalues());

            for (int j = 0; j < node.get_nvalues(); ++j){
                auto new_row = row; new_row[q] = node.get_values()[j];
                probability_values[j]=node.get_CPT()[i+j];
                for(int k=0;k<node.get_children().size();k++)
                {
                    auto child=*Alarm.get_nth_node(node.get_children()[k]);
                    int i1=find_mapping(node.get_children()[k],new_row);
                    int j1=child.idx[new_row[node.get_children()[k]]];
                    probability_values[j]=(probability_values[j]*child.get_CPT()[i1+j1]);
                }

                new_rows[j]=new_row;
            }

            double sum=0;
            for(int j=0;j<node.get_nvalues();j++) sum+=probability_values[j];
            for(int j=0;j<node.get_nvalues();j++) probability_values[j]/=sum;
            sum=0;
            for(int j=0;j<node.get_nvalues();j++) sum+=probability_values[j];
            assert(abs(sum-1)<1e-8);

            for(int j=0;j<node.get_nvalues();j++) 
            gen_data.push_back({new_rows[j],probability_values[j]});
        }
    };

    auto output = [&]() -> void{
        cout << fixed << setprecision(4);

        FILE* file=freopen("solved_alarm.bif", "w", stdout);
        for (auto &s : header_data){
            cout << s << "\n";
        }

        for (auto &node : Alarm.Pres_Graph){
            auto parents = node.get_Parents();
            parents.insert(parents.begin(), node.get_name());
            cout << "probability ( ";
            for (auto &x : parents){
                cout << " " << x << " ";
            }
            cout << ") \{ ";
            cout <<"//" << parents.size() << " " <<  "variable(s) and " << node.get_CPT().size() <<  " values" << "\n";
            cout << "\ttable ";

            vector<double> CPT = node.get_CPT();
            vector<double> new_CPT(CPT.size());

            for (int i = 0; i < node.get_CPT().size(); ++i){
                new_CPT[(i % node.get_nvalues()) * (node.get_CPT().size() / node.get_nvalues()) + i / node.get_nvalues()] = CPT[i];
            }

            for (auto &x : new_CPT){
                cout << x << " ";
            }
            cout << ";\n";
            cout << "}\n";
        }
        fclose(file);
    };

    auto end_time=chrono::high_resolution_clock::now();
    double time_completed=0;

    while (time_completed < 100){
        infer(); 
        learn();
        output();
        end_time=chrono::high_resolution_clock::now();
        time_completed=chrono::duration_cast<chrono::duration<double>>(end_time-start_time).count();
    }

	
}



