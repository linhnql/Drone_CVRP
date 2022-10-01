#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

long load_truck[100]; // trọng tải của truck
long load_drone[100]; // trọng tải của drone
double time_truck[100]; // time đã đi của truck
double time_drone[100]; // time đã đi của drone
double time_drone_n[100][100]; // time đã đi của drone
long delivered[100]; // lượng hàng đã giao cho khách

long n, K=1, M=1;
int truck_speed=40, drone_speed=60;
int route; // hành trình thứ route của drone
int drone_duration=30, work_time=30;   // time giới hạn d - drone bay 1 vòng, D chung
int m_truck=1500, m_drone=40; // trọng tải giới hạn
int low[100]; // yêu cầu tối thiểu
int upper[100]; // yêu cầu tối đa
int weight[100]; // lợi nhuận

int x, y; // toạ độ
vector<pair<double, double>> index_customer;  // toạ độ khách hàng
vector<vector<double>> matrix_dist(n, vector<double>(n));     // time giữa các khách hàng

int truck[100][100]; // lượng hàng truck i giao cho khách j
int drone[100][100][100]; // lượng hàng drone i trong hành trình j giao cho khách k

vector<vector<string>> param;
typedef struct fields{
    int K; // number_truck
    int M; // number_drone
    int truck_speed, drone_speed;
    int drone_duration, work_time; // time giới hạn d - drone bay 1 vòng, D chung
    int truck_capacity, drone_capacity;  
}fields;
map<string, fields> params;  

void read_param(string file_name){
    file_name = "./test/" + file_name;

    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    fstream file (file_name, ios::in);
    if(file.is_open()){
       while(getline(file, line)){
            row.clear();

            stringstream str(line);

            while(getline(str, word, ','))
            row.push_back(word);
            content.push_back(row);
            }
        }
    else
        cout<<"Could not open the file\n";
    
    n = content.size();
    for (int i = 1; i < n; i++)
    {
        istringstream iss(content[i][1]);
        string num_cus;
        getline(iss, num_cus, '.');
        string num_truck, num_drone;
        int num_length = content[i][3].length();
        if (num_length > 1) {
            num_truck = content[i][3][num_length-2];
            num_drone = content[i][4][num_length-2];
        } else {
            num_truck = num_length == 0 ? (num_cus == "12" ? "2" : "4") : content[i][3];
            num_drone = num_length == 0 ? (num_cus == "12" ? "2" : "4") : content[i][4];
        }
        string work_time = content[i][5].length() == 0 ? (num_cus == "12" ? "50" : "80") : content[i][5];
        string str = content[i][1];
        fields fields__;
        fields__.K = stoi(num_truck);
        fields__.M = stoi(num_drone);
        fields__.work_time = stoi(work_time);
        fields__.truck_capacity = stoi(content[i][6]);
        fields__.drone_capacity = stoi(content[i][7]);
        fields__.drone_speed = stoi(content[i][8]);
        fields__.truck_speed = stoi(content[i][9]);
        fields__.drone_duration = stoi(content[i][10]);
        params.insert(make_pair(str, fields__));
       
    }
}

int main(){   
    read_param("params.csv");

    // matrix_dist.resize(n);
    // for (int i=0; i<n; ++i)    {
    //     vector<double> temp;
    //     for (int j=0; j<n; ++j){
    //         temp.push_back(calculate_distance(index_customer[i], index_customer[j]));
    //         // cout << "matrix_dist[i][j]" << " ";
    //     }
    //     matrix_dist.push_back(temp);
    //     cout << "\n";
    // }
    // for (int i = 0; i < n; i++){
    //     // for (int j = 0; j < n; j++){
    //     //     cout << matrix_dist[i][j] << " ";
    //     // }    
    //     cout << index_customer[i].first << " " << index_customer[i].second << " " << low[i] << " " << upper[i] << " " << weight[i] << endl;
    // }
    // for(auto it = params.cbegin(); it != params.cend(); ++it)    {
    //     std::cout << it->first << "\n";
    // 
    string temp = "6.5.3";
    cout << params[temp].truck_speed;

    return 0;
}
