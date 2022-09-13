// mới bước 2
#include <bits/stdc++.h>

using namespace std;

double calculate_distance(pair<int, int> &x, pair<int, int> &y){
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

long load_truck[1000]; // trọng tải của truck
long load_drone[1000]; // trọng tải của drone
long time_truck[1000]; // time đã đi của truck
long time_drone[1000]; // time đã đi của drone
long delivered[1000]; // lượng hàng đã giao cho khách
long flag[1000]; // cờ kiểm soát lượng hàng đã giao đủ chưa

long n, K, M, speed;
int d, D;   // time giới hạn d - drone bay 1 vòng, D chung
int m_truck, m_drone; // trọng tải giới hạn
int low[1000]; // yêu cầu tối thiểu
int upper[1000]; // yêu cầu tối đa
int weight[1000]; // lợi nhuận

int x, y; // toạ độ
double matrix_time[1000][1000]; // toạ độ khách hàng

vector<pair<int, int>> index_customer;

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

    for(int i=1; i<content.size(); i++){
        index_customer.push_back(make_pair(stoi(content[i][1]), stoi(content[i][2])));
        low[i] = stoi(content[i][3]);
        upper[i] = stoi(content[i][4]);
        weight[i] = stoi(content[i][5]);
        cout << low[i] << " " << upper[i] << " " <<  weight[i] ;
        cout<<"\n";
    }
}
int main(){
    read_test();
    return 0;
}