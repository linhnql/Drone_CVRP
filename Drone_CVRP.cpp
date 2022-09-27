#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

typedef struct __truck{
    int load; // lượng hàng còn lại
    double total_time; // tổng thời gian đã chạy
    int cus_amount[100]; // lượng hàng truck i giao cho khách j
    // int flag[100];
}__truck;

typedef struct __drone{
    int load;
    double total_time;
    int route;
    double route_time[10];
    int cus_amount[10][100]; // lượng hàng drone i trong hành trình j giao cho khách k
    // int flag[100];
}__drone;

typedef struct __customer{
    double x;
    double y;
    int low;
    int upper;
    int weight; // lợi nhuận trên 1 đơn vị hàng
    int delivered;
    int truck_flag;
    int drone_flag;
}__customer;

long n, K = 1, M = 1;
int truck_speed = 40, drone_speed = 60;
int drone_duration = 30, work_time = 30; // time giới hạn d - drone bay 1 vòng, D chung
int truck_capacity = 1500, drone_capacity = 40;        // trọng tải giới hạn     

vector<pair<double, double>> index_customer; // toạ độ khách hàng
vector<vector<double>> matrix_dist;          // khoảng cách giữa các khách hàng (n, vector<double>(n))

__truck truck[100];      
__drone drone[100]; 
__customer customer[100];

void read_test()
{
    string fname;
    fname = "6.5.3.csv";

    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    fstream file(fname, ios::in);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            row.clear();

            stringstream str(line);

            while (getline(str, word, ','))
                row.push_back(word);
            content.push_back(row);
        }
    }
    else
        cout << "Could not open the file\n";

    n = content.size();
    // thêm depot vị trí (0, 0)
    index_customer.push_back(make_pair(0, 0));
    customer[0].low = 0;
    customer[0].upper = 0;
    customer[0].weight = 0;
    customer[0].truck_flag = 1;

    for (int i = 1; i < n; i++)
    {
        index_customer.push_back(make_pair(stod(content[i][1]), stod(content[i][2])));
        customer[i].low = stoi(content[i][3]);
        customer[i].upper = stoi(content[i][4]);
        customer[i].weight = stoi(content[i][5]);
    }
}

void check_truck_sol()
{
    cout << "\n";
    for (int i = 1; i < n; i++)
    {
        if (customer[i].delivered < customer[i].low) return; 
    }
    for (int i = 0; i < K; ++i)
    {
        cout << "Truck: " << i << endl;
        for (int j = 1; j < n; ++j)
        {
            cout << truck[i].cus_amount[j] << " ";
        }
        cout << "\n";
    }
}

void drone_solve(){
    for (int i = 0; i < M; ++i)
    {
        cout << "Drone: " << i << endl;
        for (int j = 0; j < drone[i].route; ++j)
        {
            cout << "Route " << j << "\n";
            for (int k = 0; k < n; ++k)
            {
                cout << drone[i].cus_amount[j][k] << " ";
            }
            cout << "\n";
        }
    }
}

double calculate_distance(pair<double, double> &x, pair<double, double> &y)
{
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

bool compare_truck(pair<double, int> &x, pair<double, int> &y)
{
    return x.first > y.first;
}
vector<pair<double, int>> select_customer_truck(int k, int num)
{
    vector<pair<double, int>> rate;
    for (int i = 0; i < n; ++i)
    {
        if (i == k)
            rate.push_back({0, i});
        else
            rate.push_back({(customer[i].weight * (customer[i].low + customer[i].upper) / 2) / matrix_dist[k][i], i});
    }
    // for (auto element : rate) {
    //     cout << element << " ";
    // }

    int deleted = 0;
    for (int i = 0; i < n; ++i)
    {
        // if (truck[num].flag[i])
        if (customer[i].truck_flag == 1)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            continue;
        }
        double time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / truck_speed;
        double total_with_back_dp = truck[num].total_time + time_to_i_and_back_depot;
        if (total_with_back_dp > work_time)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
        }
    }
    // while (truck_flag[i] == 1 || total_with_back_dp > work_time)
    // {
    //     rate.erase(rate.begin() + i);
    //     // for (auto element : rate) {
    //     //     cout << element << " ";
    //     // }cout << endl;
    //     if (rate.size() == 0)
    //         return rate;

    //     i = max_element(rate.begin(), rate.end()) - rate.begin();
    //     time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / truck_speed;

    //     total_with_back_dp = time_truck[num] + time_to_i_and_back_depot;
    // }

    // time_truck[num] += matrix_dist[k][i] / truck_speed;
    sort(rate.begin(), rate.end(), compare_truck);
    return rate;
}

int index_time_smallest(vector<double> rate, int k, int route, int num)
{
    // check time
    // khi lớn nhất mà ko thoả mãn thì xoá nó, tiếp tục lặp tìm
    if (rate.size() == 0)
        return -1;
    for (auto ele: rate) cout << ele << " "; cout << endl;

    int i = min_element(rate.begin(), rate.end()) - rate.begin();
    double time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / drone_speed;
    double total_with_back_dp = drone[num].total_time + time_to_i_and_back_depot;
    double route_with_back_dp = drone[num].route_time[route] + time_to_i_and_back_depot;

    while (customer[i].delivered > customer[i].low || total_with_back_dp > work_time || route_with_back_dp > drone_duration)
    {
        rate.erase(rate.begin() + i);
        if (rate.size() == 0)
            return -1;

        i = min_element(rate.begin(), rate.end()) - rate.begin();
        time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / drone_speed;
        total_with_back_dp = drone[num].total_time + time_to_i_and_back_depot;
        route_with_back_dp = drone[num].route_time[route] + time_to_i_and_back_depot;
    }

    drone[num].total_time += matrix_dist[k][i] / drone_speed;
    drone[num].route_time[route] += matrix_dist[k][i] / drone_speed;
    return i;
}

int select_customer_drone(int k, int route, int num)
{
    vector<double> rate;
    for (int i = 0; i < n; ++i)
    {
        if (k == i) rate.push_back(-1);
        else  
            rate.push_back((customer[i].weight * (customer[i].upper - customer[i].delivered) / matrix_dist[k][i]));
    }
    for (auto ele: rate) cout << ele << " "; cout << endl;
    // if (rate.size() == 0)
    //     return -1;
    if (*max_element(rate.begin(), rate.end()) == -1) return -1;

    int i = max_element(rate.begin(), rate.end()) - rate.begin();
    double time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / drone_speed;
    double total_with_back_dp = drone[num].total_time + time_to_i_and_back_depot;
    double route_with_back_dp = drone[num].route_time[route] + time_to_i_and_back_depot;

    while (total_with_back_dp > work_time || route_with_back_dp > drone_duration)
    {
        // rate.erase(rate.begin() + i);
        // if (rate.size() == 0)
        //     return -1;
        rate[i] = -1;
        i = max_element(rate.begin(), rate.end()) - rate.begin();
        time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / drone_speed;
        total_with_back_dp = drone[num].total_time + time_to_i_and_back_depot;
        route_with_back_dp = drone[num].route_time[route] + time_to_i_and_back_depot;
    }

    drone[num].total_time += matrix_dist[k][i] / drone_speed;
    drone[num].route_time[route] += matrix_dist[k][i] / drone_speed;
    return i;
}

void BT_truck(int j, int idx)
{
    cout << idx << " ";
    // nếu tất cả các xe đã đi qua tất cả khách -> check sol
    if (j >= K)
    {
        for (int i = 0; i < n; i++)
            if (!customer[i].truck_flag) // customer[i].truck_flag
                return;
        check_truck_sol();
        return; 
    }

    customer[idx].truck_flag = 1;

    // truck[j].flag[idx] = 1; 
    int rate = (truck[j].load / truck_capacity) / ((work_time - truck[j].total_time) / work_time);
    int delivered, remain_load;
    if (rate >= 1)
    {
        int amount_full = customer[idx].upper - customer[idx].delivered;
        if (amount_full <= truck[j].load)
        {
            delivered = remain_load = amount_full; 
            // truck thừa hàng nên giao = upper
            // truck[j].load -= amount_full;
            // customer[idx].delivered += amount_full;
            // truck[j].cus_amount[idx] = amount_full;
        }
        else
        {
            delivered = truck[j].load;
            remain_load = 0;
            // truck thiếu hàng để = upper nên giao hết còn lại
            // int amount_less = truck[j].load;
            // truck[j].load = 0;
            // customer[idx].delivered += amount_less;
            // truck[j].cus_amount[idx] = amount_less;
        }
    }
    else
    {
        int amount_qualified = customer[idx].low - customer[idx].delivered;
        if (amount_qualified <= truck[j].load)
        { 
            delivered = remain_load =  amount_qualified;
            // truck đủ hàng để >= low
            // truck[j].load -= amount_qualified;
            // customer[idx].delivered += amount_qualified;
            // truck[j].cus_amount[idx] = amount_qualified;
        }
        else
        {
            delivered = truck[j].load;
            remain_load = 0;
            // int amount_less = truck[j].load; // truck thiếu hàng để >= low nên giao hết còn lại
            // truck[j].load = 0;
            // customer[idx].delivered += amount_less;
            // truck[j].cus_amount[idx] = amount_less;
        }
    }

    truck[j].load -= remain_load;
    customer[idx].delivered += delivered;
    truck[j].cus_amount[idx] = delivered;

    vector<pair<double, int>> rateArr = select_customer_truck(idx, j);
    if (rateArr.size() == 0)
        BT_truck(j + 1, 0);
    else
        for (int i = 0; i < rateArr.size(); i++)
        {
            truck[j].total_time += matrix_dist[idx][i]/truck_speed;
            BT_truck(j, rateArr[i].second);
            truck[j].total_time -= matrix_dist[idx][i]/truck_speed;
        }
    customer[idx].truck_flag = 0;
    truck[j].load += remain_load;
    customer[idx].delivered -= delivered;
    truck[j].cus_amount[idx] = 0;
    // truck[j].flag[idx] = 0; 
    // if (rate >= 1)
    // {
    //     int amount_full = customer[idx].upper - customer[idx].delivered;
    //     if (amount_full <= truck[j].load)
    //     {
    //         // truck thừa hàng nên giao = upper
    //         truck[j].load += amount_full;
    //         customer[idx].delivered -= amount_full;
    //     }
    //     else
    //     {
    //         // truck thiếu hàng để = upper nên giao hết còn lại
    //         customer[idx].delivered -= truck[j].load;;
    //     }
    // }
    // else
    // {
    //     int amount_qualified = customer[idx].low - customer[idx].delivered;
    //     if (amount_qualified <= truck[j].load)
    //     { // truck đủ hàng để >= low
    //         truck[j].load += amount_qualified;
    //         customer[idx].delivered -= amount_qualified;
    //     }
    //     else
    //     {
    //         // truck[j].load = 0;
    //         customer[idx].delivered -= truck[j].load;
    //     }
    // }
}

int main()
{
    read_test();

    for (int i = 0; i < n; ++i)
    {
        vector<double> temp;
        for (int j = 0; j < n; ++j)
        {
            temp.push_back(calculate_distance(index_customer[i], index_customer[j]));
        }
        matrix_dist.push_back(temp);
    }

    for (int i = 0, j = 0; i < M, j < K; ++i, ++j)
    {
        truck[j].load = truck_capacity;
        drone[i].load = drone_capacity;
        // truck[i].flag[0] = 1;
        // drone[i].flag[0] = 1;
    }
    BT_truck(0, 0);

    // drone giao cho đủ low, xuất phát từ dron j
    // customer[1].delivered = 50;
    // customer[2].delivered = 50;
    // customer[3].delivered = 100;
    // customer[4].delivered = 500;
    // customer[5].delivered = 100;
    // customer[6].delivered = 500;
    // customer[1].truck_flag = 1;
    // customer[2].truck_flag = 1;
    // customer[3].truck_flag = 1;
    // customer[4].truck_flag = 1;
    // customer[5].truck_flag = 1;
    // customer[6].truck_flag = 1;
    for (int j = 0; j < M; ++j)
    {
        drone[j].route = 0;
        drone[j].load = drone_capacity;

        // tại khách vị trí k
        for (int k = 0; k < n; ++k)
        {
            if (customer[k].truck_flag) continue;
            int i = index_time_smallest(matrix_dist[k], k, drone[j].route, j);

            if (i == -1)
                continue;

            int excess_amount = customer[i].low - customer[i].delivered;
            if (excess_amount <= drone[j].load)
            { // drone thừa hàng nên giao = low
                drone[j].load -= excess_amount;
                customer[i].delivered += excess_amount;
                drone[j].cus_amount[drone[j].route][i] = excess_amount;
            }
            else
            { // drone thiếu hàng để = low nên giao hết, drone[j].route khác
                int drone_less = drone[j].load;
                drone[j].load = 0;
                customer[i].delivered += drone_less;
                drone[j].cus_amount[drone[j].route][i] = drone_less;
            }

            if (drone[j].total_time >= work_time)
                break;
            if (drone[j].route_time[drone[j].route] >= drone_duration)
            {
                drone[j].route++;
                drone[j].load = drone_capacity;
            }
        }
    }

    // xuất phát từ drone j
    for (int j = 0; j < M; ++j)
    {
        if (drone[j].total_time >= work_time)
            continue;

        // tại khách vị trí k
        for (int k = 0; k < n; ++k)
        {
            // tìm vị trí i ưu tiên - bước 1
            int i = select_customer_drone(k, drone[j].route, j);
            if (i == -1)
                continue;

            int excess_amount = customer[i].upper - customer[i].delivered;
            if (excess_amount <= drone[j].load)
            { // drone thừa hàng nên giao = upper
                drone[j].load -= excess_amount;
                customer[i].delivered += excess_amount;
                drone[j].cus_amount[drone[j].route][i] = excess_amount;
            }
            else
            { // drone thiếu hàng để = upper nên giao hết, route khác
                int drone_less = drone[j].load;
                drone[j].load = 0;
                customer[i].delivered += drone_less;
                drone[j].cus_amount[drone[j].route][i] = drone_less;
            }
            if (drone[j].total_time >= work_time)
                break;
            if (drone[j].route_time[drone[j].route] >= drone_duration)
            {
                drone[j].route++;
                drone[j].load = drone_capacity;
            }
        }
    }
    drone_solve();

    return 0;
}
