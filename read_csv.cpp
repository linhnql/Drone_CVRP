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

void read_test(){
    string fname;
    fname = "6.5.1.csv";

    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    fstream file (fname, ios::in);
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
    // thêm depot vị trí (0, 0)
    index_customer.push_back(make_pair(0, 0));
    low[0] = 0;
    upper[0] = 0;
    weight[0] = 0;
    // flag[0] = 1;

    for (int i = 1; i < n; i++)
    {
        index_customer.push_back(make_pair(stod(content[i][1]), stod(content[i][2])));
        low[i] = stoi(content[i][3]);
        upper[i] = stoi(content[i][4]);
        weight[i] = stoi(content[i][5]);
    }
}

double calculate_distance(pair<double, double> &x, pair<double, double> &y){
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

int main(){   
    read_test();

    // matrix_dist.resize(n);
    for (int i=0; i<n; ++i)    {
        vector<double> temp;
        for (int j=0; j<n; ++j){
            temp.push_back(calculate_distance(index_customer[i], index_customer[j]));
            // cout << "matrix_dist[i][j]" << " ";
        }
        matrix_dist.push_back(temp);
        cout << "\n";
    }
    for (int i = 0; i < n; i++){
        // for (int j = 0; j < n; j++){
        //     cout << matrix_dist[i][j] << " ";
        // }    
        cout << index_customer[i].first << " " << index_customer[i].second << " " << low[i] << " " << upper[i] << " " << weight[i] << endl;
    }
    return 0;
}
