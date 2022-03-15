#include<bits/stdc++.h>
using namespace std;

typedef long long int ll;
#define f first
#define s second

struct details {
    int sch_time;
    int task;
    int workflow;
};


class Compare{
public:
    bool operator() (const details &a, const details &b){
        return a.sch_time>b.sch_time;
    }
};

typedef pair<int, int> pi;


map<int,map< pair<int, int>, vector< int>>>mp;
// { workflow:{task, cost}:{vector of dependencies} }

vector<int>rec_time; // workflow starting time
vector<int> num_task; // to store number of task in each workflow
vector<vector<int>>task_cost; // to store cost => task_cost[workflow][task]= cost
int cnt =-1; //to track number of workflows


// function to extract data from json strings
void convert_jsoninput(){
    ifstream reader("input.json", std::ifstream::binary);
    string line;
    int cur_bracket=0, sq_bracket =0;
    map<pair<int,int>, vector<int>>york;
    vector<int>dependencies_;
    if(reader.is_open()) {
        while(getline(reader, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char x) { return std::isspace(x); }), line.end());
            // cout<<line<<endl;
            int work_flow_;
            int task_t;
            int cost_;
            int put = 0;
            if (cur_bracket==0 && sq_bracket==0 && line[0]=='['){
                sq_bracket++;
            }
            else if (line[0]=='{'){
                cur_bracket++;
                put =0;
            }
            else if (cur_bracket==1 && sq_bracket==1){
                if (line.length()>1 && line[1]=='n'){
                    cnt++;
                    work_flow_= cnt;
                }
                else if (line.length()>1 && line[1]=='s'){
                    string temp ="";
                    for (int g = 15;g<line.length();g++){
                        if (isdigit(line[g])){
                            temp+=line[g];
                        }
                    }
                    rec_time.push_back(stoi(temp));
                }
                else if (line.length()>8 && line[8]=='['){
                    sq_bracket++;
                }
                else if (line[0]=='}'){
                    cur_bracket--;
                }
            }
            else if (cur_bracket== 2 && sq_bracket==2){
                if (line.length()>1 && line[1]=='n'){
                    string temp= "";
                    for (int g =12;g<line.length();g++){
                        if (isdigit(line[g])){
                            temp+=line[g];
                        }
                    }
                    task_t= stoi(temp);
                }
                else if (line.length()>3 && line[1]=='d' && line[3]=='s'){
                	// do nothing
                }
                else if (line.length()>1 && line[1]=='c'){
                    string temp= "";
                    for (int g =7;g<line.length();g++){
                        if (isdigit(line[g])){
                            temp+=line[g];
                        }
                    }
                    cost_= stoi(temp);
                }
                else if (line.length()>15 && line[1]=='d' && line[15]=='[' && line.length()==17){
                    york[{task_t, cost_}] = (dependencies_);
	                dependencies_.clear();
                }
                else if (line.length()>15 && line[1]=='d' && line[15]=='[' && line.length()==16){
                    sq_bracket++;
                }
                else if (line[0]=='}'){
                    cur_bracket--;
                    put=2;
                }
            }
            else if (cur_bracket==2 && sq_bracket==3){
                if (line[0]!=']'){
                    string temp= "";
                    for (int g =5;g<line.length();g++){
                        if (isdigit(line[g])){
                            temp+=line[g];
                        }
                    }
                    dependencies_.push_back(stoi(temp));
                }
                else if (line[0]==']'){
                    sq_bracket--;
                    put =2;
                }
            }
            else if (line[0]=='}'){
                cur_bracket--;
                put =2;
            }
            else if (cur_bracket==1 && sq_bracket==2 && line[0]==']'){
                sq_bracket--;
                put=2;
            }

            if (put==2 && cur_bracket==2 && sq_bracket==2 ){
                york[{task_t, cost_}] = (dependencies_);
                dependencies_.clear();
            }
            if (put==2 && cur_bracket==1 && sq_bracket==1){
                mp[work_flow_]= (york);
                int task_counter=0;
                for (auto it:york){
                	task_counter++;
                }
                num_task.push_back(task_counter+1);
                york.clear();
                put =0;
            }
        }

    }

     reader.close();
}

void print (vector<vector<pi>>exetime, vector<vector<int>>worker_assigned){
	std::ofstream outdata;
	outdata.open("output.txt",std::ofstream::binary);

	if(!outdata){
		cerr<<"output.txt not found"<<endl;
		return;
	}
	outdata<<"["<<endl;
	for (int workflow_i =0;workflow_i<exetime.size();workflow_i++){
		outdata<<"{"<<endl;
		outdata<<"name:workflow"<<workflow_i+1<<endl;
		outdata<<"scheduled_at:"<<exetime[workflow_i][0].f<<endl;
		outdata<<"completed_at:"<<exetime[workflow_i][exetime[workflow_i].size()-1].s<<endl;
		outdata<<"tasks:["<<endl;
		for (int task_j=0;task_j<exetime[workflow_i].size();task_j++){
			outdata<<"{"<<endl;
			outdata<<"name:task"<<task_j+1<<endl;
			outdata<<"worker:"<<worker_assigned[workflow_i][task_j]+1<<endl;
			outdata<<"scheduled_at:"<<exetime[workflow_i][task_j].f<<endl;
			outdata<<"completed_at"<<exetime[workflow_i][task_j].s<<endl;
			outdata<<"},"<<endl;
		}
		outdata<<"]"<<endl;
		outdata<<"},"<<endl;
	}
	outdata<<"]"<<endl;
}

void solve(int workers) {
    int wfn; 
    
    wfn = cnt+1;
    vector<vector<int>>min_scheduled;

    vector<vector<pi>>exetime(wfn);
    vector<vector<int>>cost(wfn);
    vector<vector<int>>worker_assigned(wfn);
    int ptr=0;
    for (auto it: mp){
        int scheduled_at= it.f;
        vector<int>indegree(num_task[ptr], 0);
        indegree[0]=(rec_time[ptr]);
        for (auto j:it.s){
            exetime[ptr].push_back({0,0});
            int curr_task= j.f.f-1;
            //j.f.f= task; j.f.s= cost; j.s= dependencies;
            worker_assigned[ptr].push_back(0);
            if (curr_task==0){
            	cost[ptr].push_back(j.f.s);
            }
            for (int i=0;i<j.s.size();i++){
                int  y= j.s[i]-1;
                cost[ptr].push_back(j.f.s);
                if (curr_task!=0){
                    indegree[curr_task]= max(indegree[curr_task], (cost[ptr][y] + indegree[y]));
                }
            }
        }

        min_scheduled.push_back(indegree);
        
        ptr++;
        if (ptr==wfn){
        	break;
        }
    }

    // the task having minimum schedule time should schedule first
    priority_queue<details, vector<details>, Compare> next_to_schedule;  

    // wroker who finishes first should be first to assign to
    priority_queue<pi, vector<pi>, greater<pi> > worker_store;

    for (int i =0;i<min_scheduled.size();i++){
        for (int j = 0;j<min_scheduled[i].size();j++){
            details m ;
            m.sch_time = min_scheduled[i][j];
            m.workflow = i;
            m.task = j;
            next_to_schedule.push(m);
        }
    }

    for (int z =0;z<workers;z++){
        worker_store.push({0,z});
    }


    // assignment of worker to the task
    while (!next_to_schedule.empty()){
        int _workflow = next_to_schedule.top().workflow;
        int _task= next_to_schedule.top().task;
        int _sch_time = next_to_schedule.top().sch_time;
        next_to_schedule.pop();

        int worker_availability = worker_store.top().f;
        int worker= worker_store.top().s;

        // TWO POSSIBLITY:
	       //  WORKER.FINISHING_TIME> NEXT_TASK.SCHEDULED_AT
	       //  	WORKER IS BUSY THEN TASK WAITS THEN IT WILL BE SCHEDULED WHEN WORKER IS FREE
	       //  ELSE 
	       //  	WORKER WILL WAIT TILL THE NEXT TASK TO ARRIVE

        int sch_at = max(worker_availability, _sch_time);

        
        int worker_new_cost = sch_at+cost[_workflow][_task];
        worker_store.pop();
        worker_store.push({worker_new_cost, worker});

        exetime[_workflow][_task]= {sch_at, worker_new_cost};
        worker_assigned[_workflow][_task]= worker;
    }

    print(exetime, worker_assigned);
}


signed main(){
 
    int workers;
    cin>>workers;
    if (workers==0){
    	cout<<"IMPOSSIBLE TO SCHEDULE WORKFLOW"<<endl;
    	return 0;
    }
    // take the input in json => array of workflows object
    convert_jsoninput();
    solve(workers);

    return 0;
}


