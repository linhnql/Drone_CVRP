#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

long load_truck[1000]; // trọng tải của truck
long load_drone[1000]; // trọng tải của drone
long time_truck[1000]; // time đã đi của truck
long time_drone[1000]; // time đã đi của drone
long time_drone_n[1000][100]; // time đã đi của drone
long delivered[1000]; // lượng hàng đã giao cho khách
// long flag[1000]; // cờ kiểm soát lượng hàng đã giao đủ chưa

long n, K, M, speed;
int route; // hành trình thứ route của drone
int d, D;   // time giới hạn d - drone bay 1 vòng, D chung
int m_truck, m_drone; // trọng tải giới hạn
int low[1000]; // yêu cầu tối thiểu
int upper[1000]; // yêu cầu tối đa
int weight[1000]; // lợi nhuận

int x, y; // toạ độ
vector<pair<double, double>> index_customer;  // toạ độ khách hàng
vector<vector<double>> matrix_time;     // time giữa các khách hàng

map<int, int> res; // customer - delivered

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

double calculate_distance(pair<double, double> &x, pair<double, double> &y){
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

int index_rate_truck(vector<int> rate, int k){
    // check time 
    // khi lớn nhất mà ko thoả mãn thì xoá nó, tiếp tục lặp tìm
    if (rate.size() == 0) return -1;

    int i = max_element(rate.begin(), rate.end()) - rate.begin();
    if (time_truck[i] + matrix_time[k][i] + matrix_time[i][0] > D){
        matrix_time.erase(matrix_time.begin() + i);
        index_rate_truck(rate, k);
    }
    time_truck[i] = time_truck[i] + matrix_time[k][i];
    return i;
}

int select_customer_truck(int k){
    vector<int> rate;
    for (int i=0; i<n; ++i){
       rate.push_back((weight[i]*(low[i]+upper[i])/2) / matrix_time[k][i]);
    }
        
    return index_rate_truck(rate, k);
}


int index_time_smallest(vector<double> rate, int k, int route){
    // check time 
    // khi lớn nhất mà ko thoả mãn thì xoá nó, tiếp tục lặp tìm
    if (rate.size() == 0) return -1;

    int i = min_element(rate.begin(), rate.end()) - rate.begin();
    if (delivered[i] > low[i]
        ||time_drone[i] + matrix_time[k][i] + matrix_time[i][0] > D 
        || time_drone_n[i][route] + matrix_time[k][i] + matrix_time[i][0] > d){
        matrix_time.erase(matrix_time.begin() + i);
        index_time_smallest(rate, k, n);
    }
    
    time_drone[i] = time_drone[i] + matrix_time[k][i];
    time_drone_n[i][route] = time_drone_n[i][route] + matrix_time[k][i];
    return i;
}

int index_rate_drone(vector<int> rate, int k, int route){
    // check time 
    // khi lớn nhất mà ko thoả mãn thì xoá nó, tiếp tục lặp tìm
    if (rate.size() == 0) return -1;

    int i = max_element(rate.begin(), rate.end()) - rate.begin();
    if (time_drone[i] + matrix_time[k][i] + matrix_time[i][0] > D 
        || time_drone_n[i][route] + matrix_time[k][i] + matrix_time[i][0] > d){
        matrix_time.erase(matrix_time.begin() + i);
        index_rate_drone(rate, k, route);
    }
    
    time_drone[i] = time_drone[i] + matrix_time[k][i];
    time_drone_n[i][route] = time_drone_n[i][route] + matrix_time[k][i];
    return i;
}

int select_customer_drone(int k, int route){
    vector<int> rate;
    for (int i=0; i<n; ++i){
       rate.push_back((weight[i]*(upper[i] - delivered[i]) / matrix_time[k][i]));
    }
        
    return index_rate_drone(rate, k, route);
}

int main(){   
    
    double distance;
    for (int i=0; i<n; i++)    {
        for (int j=0; j<n; ++j){
            distance = calculate_distance(index_customer[i], index_customer[j]);
            matrix_time[i][j] = distance / speed;
            ;
        }
    }
    
    // xuất phát từ xe tải j
    for (int j=0; j<K; j++){
        if (load_truck[j] == 0) continue;
        // tại khách vị trí k
        for (int k=0; k<n; k++){
            // tìm vị trí i ưu tiên - bước 1
            int i = select_customer_truck(k);
            if (i == -1) continue;

            // bước 2
            int rate = (load_truck[i]/m_truck) / ((D - time_truck[i]) / D);
            if (rate >= 1){
                int amount = upper[i] - delivered[i];
                if (amount <= load_truck[i]){
                    load_truck[i] -= amount;
                    delivered[i] += amount;
                } else {
                    load_truck[i] -= load_truck[i];
                    delivered[i] += load_truck[i];
                }
            } else {
                int amount = low[i] - delivered[i];
                if (amount <= load_truck[i]){
                    load_truck[i] -= amount;
                    delivered[i] += amount;
                } else {
                    load_truck[i] -= load_truck[i];
                    delivered[i] += load_truck[i];
                }
            }
            
            if (delivered[i] >= low[i]) {
                // flag[i] = 1; 
                res.insert({i, delivered[i]});
            }
        }

    }
    
    // drone, phải có 1 stucture lưu flag = 0 ? cần flag k nhỉ
    vector<int> cus_less;
    // vector<double> temp;
    for (int j=0; j<M; j++){
        if (load_drone[j] == 0) route++;
        // tại khách vị trí k
        for (int k=0; k<n; k++){
            int i = index_time_smallest(matrix_time[k], k, route);
            if (i == -1) continue;
            
            int amount = low[i] - delivered[i];
            if (amount <= load_drone[i]){
                load_drone[i] -= amount;
                delivered[i] += amount;
            } else {
                load_drone[i] -= load_drone[i];
                delivered[i] += load_drone[i];
            }
        }
    } 
    
    // xuất phát từ drone j
    for (int j=0; j<M; j++){
        if (load_drone[j] == 0) route++;
        // tại khách vị trí k
        for (int k=0; k<n; k++){
            // tìm vị trí i ưu tiên - bước 1
            int i = select_customer_drone(k, route);
            if (i == -1) continue;

            int amount = upper[i] - delivered[i];
            if (amount <= load_drone[i]){
                load_drone[i] -= amount;
                delivered[i] += amount;
            } else {
                load_drone[i] -= load_drone[i];
                delivered[i] += load_drone[i];
            }
            
            if (delivered[i] >= low[i]) {
                // flag[i] = 1; 
                res.insert({i, delivered[i]});
            }
        }
    }

    for (const auto& elem : res) {
        cout << '\t' << elem.first << '\t' << elem.second << '\n';
    }

    return 0;
}
