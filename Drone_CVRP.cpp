// mới bước 2
#include <bits/stdc++.h>

using namespace std;

long load_truck[1000]; // trọng tải của truck
long load_drone[1000]; // trọng tải của drone
long time_truck[1000]; // time đã đi của truck
long time_drone[1000]; // time đã đi của drone
long delivered[1000]; // lượng hàng đã giao cho khách
long flag[1000]; // cờ kiểm soát lượng hàng đã giao đủ chưa

long n, K, M, speed;
int d, D;   // time giới hạn d - drone bay 1 vòng, D chung
int m_truck, m_drone; // trọng tải giới hạn
int L[1000]; // yêu cầu tối thiểu
int U[1000]; // yêu cầu tối đa
int W[1000]; // lợi nhuận

int x, y; // toạ độ
double matrix_time[1000][1000]; // toạ độ khách hàng

vector<pair<int, int>> index_customer;

double calculate_distance(pair<int, int> &x, pair<int, int> &y){
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

int index_satisfied(vector<int> rate){
    // check time 
    // khi lớn nhất mà ko thoả mãn thì xoá nó, tiếp tục lặp tìm
    if (rate.size() == 0) return -1;

    int i = max_element(rate.begin(), rate.end()) - rate.begin();
    if (time_truck[i] + matrix_time[k][i] + matrix_time[i][0] > D){
        matrix_time.erase(matrix_time.begin() + i);
        index_satisfied(rate);
    }
    
    return max_element(rate.begin(), rate.end()) - rate.begin();
}

int select_customer(int k){
    vector<int> rate;
    for (int i=0; i<n; ++i){
       rate.push_back((W[i]*(L[i]+U[i])/2) / matrix_time[k][i]);
    }
        
    return index_satisfied(rate);
    
}

int main(){

    cin >> n;
    n=5; K=5; M=2; speed=1;
    for(int i=0; i<n; ++i){
        cin >> x >> y;
        index_customer.push_back(make_pair(x, y));
    }

    for(int i=0; i<n; ++i){
        cin >> L[i] >> U[i];
    }
    
    
    double distance;
    for (int i=0; i<n; i++)    {
        for (int j=0; j<n; ++j){
            distance = calculate_distance(index_customer[i], index_customer[j]);
            matrix_time[i][j] = matrix_time[j][i] = distance / speed;
            ;
        }
    }
    
    // xe tải j
    for (int j=0; j<K; j++){
        if (load_truck[j] == 0) continue;
        // tại khách vị trí k
        for (int k=0; k<n; k++){
            // tìm vị trí i ưu tiên
            int i = select_customer(k);
            if (i == -1) continue;

            int rate = (load_truck[i]/m_truck) / ((D - time_truck) / D)
            if (rate >= 1){
                int amount = U[i] - delivered[i];
                if (amount <= load_truck[i]){
                    load_truck[i] -= amount;
                    delivered[i] += amount;
                } else {
                    load_truck[i] -= load_truck[i];
                    delivered[i] += load_truck[i];
                }
            } else {
                int amount = L[i] - delivered[i];
                if (amount <= load_truck[i]){
                    load_truck[i] -= amount;
                    delivered[i] += amount;
                } else {
                    load_truck[i] -= load_truck[i];
                    delivered[i] += load_truck[i];
                }
            }
            
            if (delivered[i] >= L[i]) flag[i] = 1; 
        }
    }
    

    return 0;
}
