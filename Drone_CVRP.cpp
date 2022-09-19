#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

long load_truck[100]; // trọng tải của truck
long load_drone[100]; // trọng tải của drone
double time_truck[100]; // time đã đi của truck
double time_drone[100]; // time đã đi của drone
double time_drone_n[100][100]; // time đã đi của drone
long delivered[100]; // lượng hàng đã giao cho khách
int flag[100];

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
vector<vector<double>> matrix_dist(n, vector<double>(n));     // khoảng cách giữa các khách hàng

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
    
    for(int i=1; i<n; i++){
        index_customer.push_back(make_pair(stod(content[i][1]), stod(content[i][2])));
        low[i] = stoi(content[i][3]);
        upper[i] = stoi(content[i][4]);
        weight[i] = stoi(content[i][5]);
        // cout << low[i] << " " << upper[i] << " " <<  weight[i] ;
        // cout << index_customer[i].first << " " << index_customer[i].second << " ";
        // cout << "\n";
    }
}

double calculate_distance(pair<double, double> &x, pair<double, double> &y){
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

int select_customer_truck(int k, int truck_num){
    vector<double> rate;
    for (int i=0; i<n; ++i){
        if (i == k) rate.push_back(0);
        else 
            rate.push_back((weight[i] * (low[i]+upper[i])/2) / matrix_dist[k][i]);
    }
     
    int i = max_element(rate.begin(), rate.end()) - rate.begin();
    if (i == 0) return -1;
    double time_to_i = matrix_dist[k][i]/truck_speed;

    while (flag[i] == 1 
        || time_truck[truck_num] + time_to_i + matrix_dist[i][0]/truck_speed > work_time){
        matrix_dist.erase(matrix_dist.begin() + i);
        if (rate.size() == 0) return -1;

        i = max_element(rate.begin(), rate.end()) - rate.begin();
        time_to_i = matrix_dist[k][i]/truck_speed;
    }

    time_truck[truck_num] += time_to_i;
    flag[i] = 1;

    return i;
}


int index_time_smallest(vector<double> rate, int k, int route, int drone_num){
    // check time 
    // khi lớn nhất mà ko thoả mãn thì xoá nó, tiếp tục lặp tìm
    if (rate.size() == 0) return -1;

    int i = min_element(rate.begin(), rate.end()) - rate.begin();
    double time_to_i =  matrix_dist[k][i]/drone_speed;
    double time_i_to_depot = matrix_dist[i][0]/drone_speed;
    while (delivered[i] > low[i]
        || time_drone[drone_num] + time_to_i + time_i_to_depot > work_time 
        || time_drone_n[drone_num][route] + time_to_i + time_i_to_depot > drone_duration){
        matrix_dist.erase(matrix_dist.begin() + i);
        if (rate.size() == 0) return -1;

        i = min_element(rate.begin(), rate.end()) - rate.begin();
        time_to_i =  matrix_dist[k][i]/drone_speed;
        time_i_to_depot = matrix_dist[i][0]/drone_speed;
    }
    
    time_drone[drone_num] += time_to_i;
    time_drone_n[drone_num][route] += time_to_i;
    return i;
}

int select_customer_drone(int k, int route, int drone_num){
    vector<int> rate;
    for (int i=0; i<n; ++i){
       rate.push_back((weight[i]*(upper[i] - delivered[i]) / matrix_dist[k][i]));
    }
        
    if (rate.size() == 0) return -1;

    int i = max_element(rate.begin(), rate.end()) - rate.begin();
    double time_to_i =  matrix_dist[k][i]/drone_speed;
    double time_i_to_depot = matrix_dist[i][0]/drone_speed;
    while (time_drone[drone_num] + time_to_i + time_i_to_depot > work_time 
        || time_drone_n[drone_num][route] + time_to_i + time_i_to_depot > drone_duration){
        matrix_dist.erase(matrix_dist.begin() + i);
        if (rate.size() == 0) return -1;
        
        i = max_element(rate.begin(), rate.end()) - rate.begin();
        time_to_i =  matrix_dist[k][i]/drone_speed;
        time_i_to_depot = matrix_dist[i][0]/drone_speed;
    }
    
    time_drone[drone_num] += time_to_i;
    time_drone_n[drone_num][route] += time_to_i;
    return i;
}

int main(){   
    read_test();

    for (int i=0; i<n; ++i) {
        vector<double> temp;
        for (int j=0; j<n; ++j) {
            temp.push_back(calculate_distance(index_customer[i], index_customer[j]));
        }
        matrix_dist.push_back(temp);
    }

    // nói chung đang sai ở đây
    // xuất phát từ xe tải j
    for (int j=0; j<K; ++j){
        cout << "J = " << j << "\n";
        if (load_truck[j] >= m_truck || time_truck[j] >= work_time) continue;
        // khách hàng k
        for (int k=0; k<n; ++k){
            cout << "k = " << k << "\n";
            // tìm vị trí i ưu tiên - bước 1
            int i = select_customer_truck(k, j);
            cout << "Customer index: "<< i << endl;
            if (i == -1) continue;

            // bước 2
            int rate = (load_truck[i]/m_truck) / ((work_time - time_truck[i]) / work_time);
            if (rate >= 1){
                int amount_full = upper[i] - delivered[i];
                if (amount_full <= load_truck[i]){ // truck thừa hàng nên giao = upper
                    load_truck[i] -= amount_full;
                    delivered[i] += amount_full;
                    truck[j][i] = amount_full;
                } else {
                    int amount_less = load_truck[i]; // truck thiếu hàng để = upper nên giao hết còn lại
                    load_truck[i] -= amount_less;
                    delivered[i] += amount_less;
                    truck[j][i] = amount_less;
                }
            } else {
                int amount_qualified = low[i] - delivered[i];
                if (amount_qualified <= load_truck[i]){ // truck đủ hàng để >= low
                    load_truck[i] -= amount_qualified;
                    delivered[i] += amount_qualified;
                    truck[j][i] = amount_qualified;
                } else {
                    int amount_less = load_truck[i]; // truck thiếu hàng để >= low nên giao hết còn lại 
                    load_truck[i] -= amount_less;
                    delivered[i] += amount_less;
                    truck[j][i] = amount_less;
                }
            }
        }

    }
    
    // drone giao cho đủ low, xuất phát từ dron j
    for (int j=0; j<M; ++j){
        if (time_drone[j] >= work_time) continue;
        if (load_drone[j] >= m_drone || time_drone_n[j][route] >= drone_duration) route++;
        // tại khách vị trí k
        for (int k=0; k<n; ++k){
            int i = index_time_smallest(matrix_dist[k], k, route, j);
            if (i == -1) continue;
            
            int excess_amount = low[i] - delivered[i];
            if (excess_amount <= load_drone[i]){ // drone thừa hàng nên giao = low
                load_drone[i] -= excess_amount;
                delivered[i] += excess_amount;
                drone[j][route][i] = excess_amount;
            } else {                             // drone thiếu hàng để = low nên giao hết, route khác
                int drone_less = load_drone[i];
                load_drone[i] -= drone_less;
                delivered[i] += drone_less;
                drone[j][route][i] = drone_less;
            }
        }
    } 
    
    // xuất phát từ drone j
    for (int j=0; j<M; ++j){
        if (time_drone[j] >= work_time) continue;
        if (load_drone[j] >= m_drone || time_drone_n[j][route] >= drone_duration) route++;
        // tại khách vị trí k
        for (int k=0; k<n; ++k){
            // tìm vị trí i ưu tiên - bước 1
            int i = select_customer_drone(k, route, j);
            if (i == -1) continue;

            int excess_amount = upper[i] - delivered[i];
            if (excess_amount <= load_drone[i]){ // drone thừa hàng nên giao = upper
                load_drone[i] -= excess_amount;
                delivered[i] += excess_amount;
                drone[j][route][i] = excess_amount;
            } else {                      // drone thiếu hàng để = upper nên giao hết, route khác
                int drone_less = load_drone[i];
                load_drone[i] -= drone_less;
                delivered[i] += drone_less;
                drone[j][route][i] = drone_less;
            }
        }
    }

    for (int i=0; i<K; ++i){
        cout << "Truck: " << i << endl;
        for (int j=0; j<n; ++j){
            cout << truck[i][j] << " ";
        }
        cout << "\n";
    }

    for (int i=0; i<M; ++i){
        cout << "Drone: " << i << endl;
        for (int j=0; j<route; ++j){
            cout << "Route " << j << "\n";
            for (int k=0; k<n; ++k){
                cout << drone[i][j][k] << " ";
            }
            cout << "\n";
        }
    }

    return 0;
}
