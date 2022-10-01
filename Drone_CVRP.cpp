#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

typedef struct truck__
{
    int load;            // lượng hàng còn lại
    double total_time;   // tổng thời gian đã chạy
    int cus_amount[100]; // lượng hàng truck i giao cho khách j
    int route[100];      // luot route_idx giao cho khach idx
    int total_route;     // luot route_idx giao cho khach idx
    // int flag[100];
} truck__;

typedef struct drone__
{
    int load;
    int flag_duration;
    double total_time;
    double process_time;
    int route[100][100];
    int total_route[100];
    int count_route;
    double route_time[100];
    int cus_amount[100][100]; // lượng hàng drone i trong hành trình k giao cho khách j
    // int flag[100];
} drone__;

typedef struct customer__
{
    double x;
    double y;
    int low;
    int upper;
    int weight; // lợi nhuận trên 1 đơn vị hàng
    int delivered;
    int truck_flag[100];
    int drone_flag[100];
} customer__;

typedef struct drone_rate
{
    double rate;
    int idx;
    double to_low;
} drone_rate;

typedef struct fields
{
    int K; // number_truck
    int M; // number_drone
    int truck_speed, drone_speed;
    int drone_duration, limited_time; // time giới hạn d - drone bay 1 vòng, D chung
    int truck_capacity, drone_capacity;
} fields;

int n, K, M;
int truck_speed, drone_speed;
int drone_duration, limited_time;   // time giới hạn d - drone bay 1 vòng, D chung
int truck_capacity, drone_capacity; // trọng tải giới hạn
bool resFlag;

vector<pair<double, double>> index_customer; // toạ độ khách hàng
vector<vector<double>> matrix_dist;          // khoảng cách giữa các khách hàng (n, vector<double>(n))
map<string, fields> params;
string output_name;

truck__ truck[100];
drone__ drone[100];
customer__ customer[100];

void read_test(string file_name)
{
    string fname = "./test/";
    fname += file_name + ".csv";

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
    customer[0].truck_flag[0] = 1;

    for (int i = 1; i < n; i++)
    {
        index_customer.push_back(make_pair(stod(content[i][1]), stod(content[i][2])));
        customer[i].low = stoi(content[i][3]);
        customer[i].upper = stoi(content[i][4]);
        customer[i].weight = stoi(content[i][5]);
    }
}

void read_param(string file_name)
{
    file_name = "./test/" + file_name;
    fstream file(file_name, ios::in);
    vector<vector<string>> content;
    vector<string> row;
    string line, word;

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

    int num_params = content.size();

    for (int i = 1; i < num_params; i++)
    {
        istringstream iss(content[i][1]);
        string num_cus;
        getline(iss, num_cus, '.');
        string num_truck, num_drone;
        int num_length = content[i][3].length();
        if (num_length > 1)
        {
            num_truck = content[i][3][num_length - 2];
            num_drone = content[i][4][num_length - 2];
        }
        else
        {
            num_truck = num_length == 0 ? (num_cus == "12" ? "2" : "4") : content[i][3];
            num_drone = num_length == 0 ? (num_cus == "12" ? "2" : "4") : content[i][4];
        }
        string limited_time = content[i][5].length() == 0 ? (num_cus == "12" ? "50" : "80") : content[i][5];
        string str = content[i][1];
        fields fields__;
        fields__.K = stoi(num_truck);
        fields__.M = stoi(num_drone);
        fields__.limited_time = stoi(limited_time);
        fields__.truck_capacity = stoi(content[i][6]);
        fields__.drone_capacity = stoi(content[i][7]);
        fields__.drone_speed = stoi(content[i][8]);
        fields__.truck_speed = stoi(content[i][9]);
        fields__.drone_duration = stoi(content[i][10]);
        params.insert(make_pair(str, fields__));
    }
}

double calculate_distance(pair<double, double> &x, pair<double, double> &y)
{
    return sqrt((x.first - y.first) * (x.first - y.first) + (x.second - y.second) * (x.second - y.second));
}

void init(string test)
{
    read_test(test);
    output_name = test;

    K = params[test].K;
    M = params[test].M;
    truck_speed = params[test].truck_speed;
    drone_speed = params[test].drone_speed;
    drone_duration = params[test].drone_duration;
    limited_time = params[test].limited_time;
    truck_capacity = params[test].truck_capacity;
    drone_capacity = params[test].drone_capacity;

    resFlag = false;
    index_customer.clear();
    matrix_dist.clear();

    // truck__ ii;
    // for (int i = 0; i < 100; i++) truck[i] = ii;
    // drone__ iii;
    // for (int i = 0; i < 100; i++) drone[i] = iii;
    // customer__ iiii;
    // for (int i = 0; i < 100; i++) customer[i] = iiii;

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
        if (customer[i].delivered == customer[i].upper)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            continue;
        }
        if (customer[i].truck_flag[num] == 1)
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

bool compare_drone(drone_rate &x, drone_rate &y)
{
    if (x.to_low >= 0 && y.to_low >= 0)
    {
        return x.rate > y.rate;
    }
    return x.to_low < y.to_low;
}
vector<drone_rate> select_customer_drone(int k, int num)
{
    vector<drone_rate> rate;
    drone[num].flag_duration = 0;
    for (int i = 0; i < n; ++i)
    {
        drone_rate addRate;
        addRate.idx = i;
        addRate.rate = i == k ? 0 : (customer[i].weight * (customer[i].low + customer[i].upper) / 2) / matrix_dist[k][i];
        addRate.to_low = customer[i].delivered - customer[i].low;
        rate.push_back(addRate);
    }
    int deleted = 0;
    bool done = true;
    bool limited = false;
    bool process = false;
    for (int i = 0; i < n; ++i)
    {
        if (customer[i].delivered == customer[i].upper)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            continue;
        }
        else
        {
            done = false;
        }
        if (customer[i].drone_flag[num] == 1)
        {
            if (customer[i].delivered < customer[i].upper)
                process = true;
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            continue;
        }

        double time_to_i_and_back_depot = (matrix_dist[k][i] + matrix_dist[i][0]) / drone_speed;
        double total_with_back_dp = drone[num].total_time + time_to_i_and_back_depot;
        double total_process_with_back_dp = drone[num].process_time + time_to_i_and_back_depot;
        // check time hanh trinh va time work day
        if (total_with_back_dp > limited_time)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            limited = true;
        }
        else if (total_process_with_back_dp > drone_duration || drone[num].load == 0)
        {
            rate.erase(rate.begin() + i - deleted);
            deleted++;
            drone[num].flag_duration = 1;
        }
    }
    if (drone[num].flag_duration != 1)
    {
        if (process)
            drone[num].flag_duration = 1;
        if (limited)
            drone[num].flag_duration = 2;
        if (done)
            drone[num].flag_duration = 3;
    }
    sort(rate.begin(), rate.end(), compare_drone);
    return rate;
}

void print_solution()
{
    long long res = 0;
    for (int i = 1; i < n; i++)
    {
        res += customer[i].delivered * customer[i].weight;
        if (customer[i].delivered < customer[i].low)
            return;
    }
    string fname = "./solution/";
    fname += output_name + ".txt";
    ofstream outfile;
    outfile.open(fname, ios::out | ios::trunc);
    resFlag = true;
    outfile << res << "\n";
    for (int i = 0; i < K; ++i)
    {
        for (int j = 1; j < truck[i].total_route; ++j)
        {
            outfile << truck[i].route[j] << " ";
        }
        outfile << "-1 ";
        for (int j = 1; j < truck[i].total_route; ++j)
        {
            outfile << truck[i].cus_amount[truck[i].route[j]] << " ";
        }
        outfile << "\n";
    }
    int ans = 0;
    for (int i = 0; i < M; ++i)
    {
        for (int ct = 0; ct <= drone[i].count_route; ct++)
        {
            if (drone[i].total_route[ct] > 1) {
                ans++;
                break;
            }
        }
    }
    outfile << ans << "\n";
    for (int i = 0; i < ans; ++i)
    {
        outfile << i + 1 << "\n";
        for (int ct = 0; ct <= drone[i].count_route; ct++)
        {
            for (int j = 1; j < drone[i].total_route[ct]; ++j)
            {
                outfile << drone[i].route[j][ct] << " ";
            }
            outfile << "-1 ";
            for (int j = 1; j < drone[i].total_route[ct]; ++j)
            {
                outfile << drone[i].cus_amount[ct][drone[i].route[j][ct]] << " ";
            }
            outfile << "\n";
        }
    }
    outfile.close();
}

void BT_Drone(int j, int idx, int k, int route_idx) // drone j di chu trinh thu k
{
    if (resFlag)
        return;
    drone[j].count_route = k;
    drone[j].total_route[k]++;
    drone[j].route[route_idx][k] = idx;
    // nếu tất cả các drone đã đi qua tất cả khách -> check sol
    if (j >= M)
    {
        print_solution();
        return;
    }

    customer[idx].drone_flag[j] = 1;

    int rate = (drone[j].load / drone_capacity) / ((limited_time - drone[j].process_time) / limited_time);
    int amount = customer[idx].low - customer[idx].delivered;
    if (amount <= 0)
        amount = customer[idx].upper - customer[idx].delivered;
    int delivered = amount > drone[j].load ? drone[j].load : amount;

    drone[j].load -= delivered;
    customer[idx].delivered += delivered;
    drone[j].cus_amount[k][idx] = delivered;

    vector<drone_rate> rateArr = select_customer_drone(idx, j);
    if (rateArr.size() == 0)
    {
        if (drone[j].flag_duration != 1)
        {
            BT_Drone(j + 1, 0, 0, 0);
        }
        else if (drone[j].flag_duration == 1)
        {
            drone__ tmp = drone[j];
            drone[j].total_time += matrix_dist[idx][0] / drone_speed;
            drone[j].load = drone_capacity;
            drone[j].process_time = 0;
            customer[idx].drone_flag[j] = 0;
            BT_Drone(j, 0, k + 1, 0);
            customer[idx].drone_flag[j] = 1;
            drone[j] = tmp;
        }
    }
    else
        for (int i = 0; i < rateArr.size(); i++)
        {
            drone[j].total_time += matrix_dist[idx][i] / drone_speed;
            drone[j].process_time += matrix_dist[idx][i] / drone_speed;
            BT_Drone(j, rateArr[i].idx, k, route_idx + 1);
            drone[j].total_time -= matrix_dist[idx][i] / drone_speed;
            drone[j].process_time -= matrix_dist[idx][i] / drone_speed;
        }

    customer[idx].drone_flag[j] = 0;
    drone[j].load += delivered;
    customer[idx].delivered -= delivered;
    drone[j].cus_amount[k][idx] = 0;
}

void BT_Truck(int j, int idx, int route_idx)
{
    if (resFlag)
        return;
    truck[j].route[route_idx] = idx;
    truck[j].total_route++;
    if (j >= K)
    {
        BT_Drone(0, 0, 0, 0);
        return;
    }

    customer[idx].truck_flag[j] = 1;

    int amount;
    if (customer[idx].low > customer[idx].delivered)
    {
        int rate = (truck[j].load / truck_capacity) / ((limited_time - truck[j].total_time) / limited_time);
        amount = (rate < 1 ? customer[idx].low : customer[idx].upper) - customer[idx].delivered;
    }
    else
    {
        amount = customer[idx].upper - customer[idx].delivered;
    }

    int delivered = amount > truck[j].load ? truck[j].load : amount;

    truck[j].load -= delivered;
    customer[idx].delivered += delivered;
    truck[j].cus_amount[idx] = delivered;

    vector<pair<double, int>> rateArr = select_customer_truck(idx, j);
    if (rateArr.size() == 0)
    {
        BT_Truck(j + 1, 0, 0);
    }

    else
        for (int i = 0; i < rateArr.size(); i++)
        {
            truck[j].total_time += matrix_dist[idx][i] / truck_speed;
            BT_Truck(j, rateArr[i].second, route_idx + 1);
            truck[j].total_time -= matrix_dist[idx][i] / truck_speed;
        }
    customer[idx].truck_flag[j] = 0;
    truck[j].load += delivered;
    customer[idx].delivered -= delivered;
    truck[j].cus_amount[idx] = 0;
}

int main()
{
    read_param("params.csv");
    // int num_cus[5] = {6, 10, 12, 20, 50};
    // int area[5] = {5, 10, 20, 30, 40};
    // string str;
    // for (auto cus: num_cus){
    //     for (int i = cus < 50 ? 0 : 1; i < (cus < 50 ? 3 : 5); ++ i){
    //         for (int j = 1; j < 5; ++j){
    //             str = to_string(cus) + "." + to_string(area[i]) + "." + to_string(j);
    //             cout << str << "\n";
    //             init(str);
    //             BT_Truck(0, 0, 0);
    //         }
    //     }
    // }

    init("10.5.1");
    BT_Truck(0, 0, 0);

    return 0;
}
