#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
using namespace std;

unordered_map<int,vector<int>> graph;
unordered_map<string,int> id;
unordered_map<int,string> name;

int main() {
    ifstream f("locations.csv");
    string line,a,b;

    getline(f,line);

    while(getline(f,line)) {
        stringstream s(line);
        getline(s,a,',');
        getline(s,b,',');

        id[b]=stoi(a);
        name[stoi(a)]=b;
    }
    f.close();

    f.open("road_network.csv");
    getline(f,line);

    while(getline(f,line)) {
        stringstream s(line);
        getline(s,a,',');
        getline(s,b,',');

        int u=stoi(a);
        int v=stoi(b);

        graph[u].push_back(v);
        graph[v].push_back(u);
    }
    f.close();

    string start,dest;

    cout<<"Enter emergency location: ";
    getline(cin,start);

    cout<<"Enter hospital: ";
    getline(cin,dest);

    queue<int> q;
    unordered_map<int,int> parent;

    q.push(id[start]);
    parent[id[start]]=-1;

    while(!q.empty()) {
        int u=q.front();
        q.pop();

        if(u==id[dest])
            break;

        for(int v:graph[u]) {
            if(!parent.count(v)) {
                parent[v]=u;
                q.push(v);
            }
        }
    }
    
    if(!parent.count(id[dest])) {
        cout<<"No route found";
        return 0;
    }

    vector<int> path;

    for(int x=id[dest]; x!=-1; x=parent[x])
        path.push_back(x);

    reverse(path.begin(),path.end());

    cout<<"\nRoute: ";

    for(int x:path)
        cout<<name[x]<<" -> ";

    cout<<"\nRoad Segments: "<<path.size()-1;

    return 0;
}