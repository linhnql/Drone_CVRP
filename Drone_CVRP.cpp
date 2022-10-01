#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

typedef struct __truck
{
    int load;            // lượng hàng còn lại
    double total_time;   // tổng thời gian đã chạy
    int cus_amount[100]; // lượng hàng truck i giao cho khách j
    int route[100];      // luot route_idx giao cho khach idx
    // int flag[100];
} __truck;

typedef struct __drone
{
    int load;
    double total_time;
    double process_time;
    int route[100];
    double route_time[10];
    int cus_amount[100][100]; // lượng hàng drone i trong hành trình k giao cho khách j
    // int flag[100];
} __drone;

typedef struct __customer
{
    double x;
    double y;
    int low;
    int upper;
    int weight; // lợi nhuận trên 1 đơn vị hàng
    int delivered;
    int truck_flag;
    int drone_flag[100];
} __customer;

typedef struct __drone_rate
{
    double rate;
    int idx;
    double to_low;
} __drone_rate;

int n, K = 1, M = 1;
int truck_speed = 40, drone_speed = 60;
int drone_duration = 30, limited_time = 30;       // time giới hạn d - drone bay 1 vòng, D chung
int truck_capacity = 1500, drone_capacity = 40; // trọng tải giới hạn
bool resFlag = false;

vector<pair<double, double>> index_customer; // toạ độ khách hàng
vector<vector<double>> matrix_dist;          // khoảng cách giữa các khách hàng (n, vector<double>(n))

__truck truck[100];
__drone drone[100];
__customer customer[100];

void read_test(string file_name)
{
    string fname = "./test/";
    fname += file_name;

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

double calculate_distance(pair<double, double> &x, pair<double, double> &y)
{
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

bool compare(pair<double, int> &x, pair<double, int> &y)
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

    int deleted = 0;
    for (int i = 0; i < n; ++i)
    {
        if (customer[i].truck_flag == 1)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            continue;
        }
        double time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / truck_speed;
        double total_with_back_dp = truck[num].total_time + time_to_i_and_back_depot;
        if (total_with_back_dp > limited_time)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
        }
    }
    sort(rate.begin(), rate.end(), compare);
    return rate;
}

bool compare_drone(__drone_rate &x, __drone_rate &y)
{
    if (x.to_low >= 0 && y.to_low >= 0)
    {
        return x.rate > y.rate;
    }
    return x.to_low < y.to_low;
}
vector<__drone_rate> select_customer_drone(int k, int num)
{
    vector<__drone_rate> rate;
    for (int i = 0; i < n; ++i)
    {
        __drone_rate addRate;
        addRate.idx = i;
        addRate.rate = i == k ? 0 : (customer[i].weight * (customer[i].low + customer[i].upper) / 2) / matrix_dist[k][i];
        addRate.to_low = customer[i].delivered - customer[i].low;
        rate.push_back(addRate);
    }
    int deleted = 0;
    for (int i = 0; i < n; ++i)
    {
        if (customer[i].drone_flag[num] == 1 || customer[i].delivered == customer[i].upper)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            continue;
        }
        double time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / drone_speed;
        // double total_with_back_dp = drone[num].total_time + time_to_i_and_back_depot;
        double total_process_with_back_dp = drone[num].process_time + time_to_i_and_back_depot;
        // check time hanh trinh va time work day
        if (total_process_with_back_dp > drone_duration)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
        }
    }
    sort(rate.begin(), rate.end(), compare_drone);
    return rate;
}

void check_drone_sol()
{
    for (int i = 1; i < n; i++)
    {
        if (customer[i].delivered < customer[i].low)
            return;
    }
    cout << "Result\n";
    resFlag = true;
    for (int i = 0; i < K; ++i)
    {
        cout << "Truck: " << i << "\nRoute:\n";
        for (int j = 1; j < n; ++j)
        {
            cout << "- " << truck[i].route[j] << " " << truck[i].cus_amount[truck[i].route[j]] << "\n";
        }
        cout << "\n";
    }
    for (int i = 0; i < M; ++i)
    {
        cout << "Drone: " << i << endl;
        cout << "Route 0:"
             << "\n";
        for (int j = 1; j < n; ++j)
        {
            cout << "- " << drone[i].route[j] << " " << drone[i].cus_amount[0][drone[i].route[j]] << "\n";
        }
    }
}

void BT_drone(int j, int idx, int k, int route_idx) // drone j di chu trinh thu k
{
    if (resFlag)
        return;

    drone[j].route[route_idx] = idx;
    // nếu tất cả các drone đã đi qua tất cả khách -> check sol
    if (j >= M)
    {
        check_drone_sol();
        return;
    }

    customer[idx].drone_flag[j] = 1;
    // truck[j].flag[idx] = 1;

    int rate = (drone[j].load / drone_capacity) / ((limited_time - drone[j].process_time) / limited_time);
    int amount = customer[idx].low - customer[idx].delivered;
    if (amount <= 0)
        amount = customer[idx].upper - customer[idx].delivered;
    int delivered = amount > drone[j].load ? drone[j].load : amount;

    drone[j].load -= delivered;
    customer[idx].delivered += delivered;
    drone[j].cus_amount[k][idx] = delivered;

    vector<__drone_rate> rateArr = select_customer_drone(idx, j);
    if (rateArr.size() == 0)
        BT_drone(j + 1, 0, 0, 0);
    else
        for (int i = 0; i < rateArr.size(); i++)
        {
            truck[j].total_time += matrix_dist[idx][i] / truck_speed;
            BT_drone(j, rateArr[i].idx, 0, route_idx + 1);
            truck[j].total_time -= matrix_dist[idx][i] / truck_speed;
        }
    customer[idx].drone_flag[j] = 0;
    drone[j].load += delivered;
    customer[idx].delivered -= delivered;
    drone[j].cus_amount[k][idx] = 0;
}

void check_truck_sol()
{
    // giao low (lam sau)
    BT_drone(0, 0, 0, 0);
}

void BT_truck(int j, int idx, int route_idx)
{
    if (resFlag)
        return;
    truck[j].route[route_idx] = idx;
    if (j >= K)
    {
        check_truck_sol();
        return;
    }

    customer[idx].truck_flag = 1;
    // truck[j].flag[idx] = 1;

    int rate = (truck[j].load / truck_capacity) / ((limited_time - truck[j].total_time) / limited_time);

    int amount = (rate < 1 ? customer[idx].low : customer[idx].upper) - customer[idx].delivered;
    int delivered = amount > truck[j].load ? truck[j].load : amount;

    truck[j].load -= delivered;
    customer[idx].delivered += delivered;
    truck[j].cus_amount[idx] = delivered;

    vector<pair<double, int>> rateArr = select_customer_truck(idx, j);
    if (rateArr.size() == 0)
        BT_truck(j + 1, 0, 0);
    else
        for (int i = 0; i < rateArr.size(); i++)
        {
            truck[j].total_time += matrix_dist[idx][i] / truck_speed;
            BT_truck(j, rateArr[i].second, route_idx + 1);
            truck[j].total_time -= matrix_dist[idx][i] / truck_speed;
        }
    customer[idx].truck_flag = 0;
    truck[j].load += delivered;
    customer[idx].delivered -= delivered;
    truck[j].cus_amount[idx] = 0;
}

int main()
{
    // int num_cus[5] = {6, 10, 12, 20, 50};
    // int area[5] = {5, 10, 20, 30, 40};
    // string str;
    // for (auto cus: num_cus){
    //     for (int i = cus < 50 ? 0 : 1; i < (cus < 50 ? 3 : 5); ++ i){
    //         for (int j = 1; j < 5; ++j){
    //             str = to_string(cus) + "." + to_string(area[i]) + "." + to_string(j) + ".csv";
    //             read_test(str);
    //             ....
    //         }
    //     }
    // }
    read_test("6.5.3.csv");

    for (int i = 0; i < n; ++i)
    {
        vector<double> temp;
        for (int j = 0; j < n; ++j)
        {
            temp.push_back(calculate_distance(index_customer[i], index_customer[j]));
        }
        matrix_dist.push_back(temp);
    }

    for (int i = 0; i < M; ++i)
    {
        drone[i].load = drone_capacity;
    }
    for (int i = 0; i < K; ++i)
    {
        truck[i].load = truck_capacity;
    }
    BT_truck(0, 0, 0);

    return 0;
}
