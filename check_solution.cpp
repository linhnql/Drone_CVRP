#include <bits/stdc++.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

typedef struct truck__
{
    int load;            // lượng hàng còn lại
    double total_time;   // tổng thời gian đã chạy
    int cus_amount[100]; // lượng hàng truck i giao cho khách j
    int flag[100];
} truck__;

typedef struct drone__
{
    int load;
    double total_time;
    double process_time;
    int route;
    double route_time[200];
    int cus_amount[200][200]; // lượng hàng drone i trong hành trình j giao cho khách k
    int flag[200];
} drone__;

typedef struct customer__
{
    double x;
    double y;
    int low;
    int upper;
    int weight; // lợi nhuận trên 1 đơn vị hàng
    int delivered;
    int truck_flag;
    int drone_flag;
} customer__;

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

vector<pair<double, double>> index_customer; // toạ độ khách hàng
vector<vector<double>> matrix_dist;

truck__ truck[100];
drone__ drone[100];
customer__ customer[100];
map<string, fields> params;

int flag;

double calculate_distance(pair<double, double> &x, pair<double, double> &y)
{
    return sqrt((x.first - y.first) * (x.first - y.first) + (x.second - y.second) * (x.second - y.second));
}

void read_test(string file_name)
{
    file_name = "./test/" + file_name + ".csv";
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
    file.close();
    
    for (int i = 0; i < n; ++i)
    {
        vector<double> temp;
        for (int j = 0; j < n; ++j)
        {
            temp.push_back(calculate_distance(index_customer[i], index_customer[j]));
        }
        matrix_dist.push_back(temp);
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
    file.close();
}

vector<int> get_router(string line, int &start_load, int &cnt)
{
    std::istringstream iss(line);
    double n = 0;
    int cnt = 0;
    std::vector<int> drone_trip;

    while (iss >> n)
    {
        cnt++;
        if (n == -1)
            start_load = cnt;
        drone_trip.push_back(n);
    }
    return drone_trip;
}

void check_truck(ofstream& file_check, string line, int num_truck)
{
    std::istringstream iss(line);
    int start_load;
    double n;
    int cnt = 0;
    std::vector<int> truck_trip;

    while (iss >> n)
    {
        cnt++;
        if (n == -1)
            start_load = cnt;
        truck_trip.push_back(n);
    }

    set<int> check_cus;
    check_cus.insert(truck_trip[0]);
    double distance = matrix_dist[0][truck_trip[0]];
    int count_cus = start_load - 1;
    for (int j = 1; j < count_cus; ++j)
    {
        check_cus.insert(truck_trip[j]);
        distance += matrix_dist[truck_trip[j - 1]][truck_trip[j]];
    }
    distance += matrix_dist[truck_trip[count_cus - 1]][0];
        // cout << "distance: " << distance << endl;
    if (check_cus.size() < count_cus)
        flag = 1,
        file_check << "Truck " << num_truck << " violates delivered customer GREATER than ONCE\n";
    // cout << line << " " << count_cus << " " << distance << " " << truck_speed << endl;
    double working_time = distance / truck_speed;
    cout.precision(4);
    if (working_time > limited_time)
        flag = 1, file_check << "Truck " << num_truck << " violates WORKING TIME: " << fixed << working_time << "\n";

    int load_truck = 0;
    for (int j = start_load; j < cnt; ++j)
    {
        load_truck += truck_trip[j];
        customer[truck_trip[j - start_load]].delivered += truck_trip[j];
    }
    // cout <<"load_truck: " << load_truck << endl;
    if (load_truck > truck_capacity)
        flag = 1, file_check << "Truck " << num_truck << " violates CAPACITY: " << load_truck << "\n";
}

double check_drone_router(ofstream& file_check, string line, int num_drone, int router)
{
    int start_router = 0, cnt = 0;
    vector<int> drone_trip = get_router(line, start_router, cnt);

    set<int> check_cus;
    check_cus.insert(drone_trip[0]);
    double router_dis = matrix_dist[0][drone_trip[0]];
    int count_cus = start_router - 1;
    for (int j = 1; j < count_cus; ++j)
    {
        check_cus.insert(drone_trip[j]);
        router_dis += matrix_dist[drone_trip[j - 1]][drone_trip[j]];
    }
    // cout << "router_dis: " << router_dis << endl;
    if (check_cus.size() < count_cus)
        flag = 1, file_check << "Drone " << num_drone << " delivered customer GREATER than ONCE in router " << router << "\n";
    double router_time = router_dis / drone_speed;
    router_dis += matrix_dist[drone_trip[count_cus - 1]][0];

    if (router_time > drone_duration)
        flag = 1, file_check << "Drone "<< num_drone << " violates DURATION in router " << router << ": " << router_time << "\n";

    int load_drone = 0;
    for (int j = start_router; j < cnt; ++j)
    {
        load_drone += drone_trip[j];
        customer[drone_trip[j - start_router]].delivered += drone_trip[j];
    }
    // cout <<"load_drone: " << load_drone << endl;
    if (load_drone > drone_capacity)
        flag = 1, file_check << "Drone " << num_drone << " violates CAPACITY in router " << router << ": " << load_drone << "\n";

    return router_dis;
}

void check_customer(ofstream& file_check)
{
    // cout << "n: " << n << endl;
    for (int i = 1; i < n; ++i)
    {
        int amount = customer[i].delivered;
        // cout << amount << " " << customer[i].low << " " << customer[i].upper << endl;
        if (amount < customer[i].low)
            flag = 1, file_check << "Customer " << i << " LESS than low: " << amount << "\n";
        if (amount > customer[i].upper)
            flag = 1, file_check << "Customer " << i << " GREATER upper: " << amount << "\n";
    }
}

void check_solution(ofstream& file_check, string solution_name)
{
    for (int i = 1; i < n; ++i)
    {
        customer[i].delivered = 0;
    }
    string file_name = "./solution/" + solution_name + ".txt";
    std::ifstream file(file_name, std::ios_base::in);
    if (!file.is_open()){
        flag = 1;
        cout << "Could not open the file\n";
        return;
    }
    file_check << "\n"
               << solution_name.c_str() << "\n";
    vector<string> lines;
    string line;
    getline(file, line);

    // check truck
    int cnt_truck = 1, cnt_drone;
    while (getline(file, line))
    {
        // int length = line.length();
        if (line.find("-1") != std::string::npos)
            check_truck(file_check, line, cnt_truck++);
        else
        {
            cnt_drone = stoi(line);
            break;
        }
    }
    cnt_truck--;
    if (cnt_truck > K)
        flag = 1, file_check << "Violates NUMBER truck: " << cnt_truck << "\n";

    if (cnt_drone > M)
        flag = 1, file_check << "Violates NUMBER drone: " << cnt_drone << "\n";
    // check drone
    getline(file, line);
    for (int i = 0; i < cnt_drone; ++i)
    {
        // cout << "Drone " << i+1 << endl;

        int router = 0;
        double total_dis = 0;
        while (getline(file, line) && line.find("-1") != std::string::npos)
        {
            router++; 
            double router_dis = check_drone_router(file_check, line, i + 1, router);
            total_dis += router_dis;
        }
        double total_time = total_dis / drone_speed;
        // cout << "total_time drone: " << i << " " << total_time << endl;
        cout.precision(4);
        if (total_time > limited_time)
            flag = 1, file_check<< "Drone " << i+1 <<" violates WORKING TIME: " << fixed << total_time << "\n";
    }
    check_customer(file_check);
}

int main()
{
    string fname = "./solution/one_solution.txt";
    ofstream outfile;
    outfile.open(fname, ios::out | ios::trunc);
    // loop check file
    int not_feasible = 0;
    int feasible = 0;
    // int num_cus[5] = {6, 10, 12, 20, 50};
    // int area[5] = {5, 10, 20, 30, 40};
    // string str;

    // read_param("params.csv");
    // for (auto cus : num_cus)
    // {
    //     for (int i = cus < 50 ? 0 : 1; i < (cus < 50 ? 3 : 5); ++i)
    //     {
    //         for (int j = 1; j < 5; ++j)
    //         {
    //             str = to_string(cus) + "." + to_string(area[i]) + "." + to_string(j);
    //             read_test(str);

    //             flag = 0;
    //             K = params[str].K;
    //             M = params[str].M;
    //             truck_speed = params[str].truck_speed;
    //             drone_speed = params[str].drone_speed;
    //             drone_duration = params[str].drone_duration;
    //             limited_time = params[str].limited_time;
    //             truck_capacity = params[str].truck_capacity;
    //             drone_capacity = params[str].drone_capacity;
                
    //             // file check 2 dòng cuối k cùng trống
    //             check_solution(outfile, str);
    //             cout << str << " " << limited_time << endl;
    //             if (flag)
    //                 not_feasible++;
    //             else
    //                 feasible++, outfile << "FEASIBLE SOLUTION\n";
    //         }
    //     }
    // }
    string test = "6.10.1";
    read_test(test);

    read_param("params.csv");
    flag = 0;
    K = params[test].K;
    M = params[test].M;
    truck_speed = params[test].truck_speed;
    drone_speed = params[test].drone_speed;
    drone_duration = params[test].drone_duration;
    limited_time = params[test].limited_time;
    truck_capacity = params[test].truck_capacity;
    drone_capacity = params[test].drone_capacity;

    // outfile << test << "\n";
    check_solution(outfile, test);
    if (!flag)
        feasible++, outfile <<  "FEASIBLE SOLUTION\n";
    else not_feasible++;

    // outfile << "\nNumber feasible solution: " <<  feasible << "\n"; // 64 - not_feasible
    // outfile << "Number not feasible solution: " <<  not_feasible << "\n";
    // outfile.close();
    return 0;
}
