#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

typedef struct truck__{
    int load; // lượng hàng còn lại
    double total_time; // tổng thời gian đã chạy
    int cus_amount[100]; // lượng hàng truck i giao cho khách j
    int flag[100];
}truck__;

typedef struct drone__{
    int load;
    double total_time;
    double process_time;
    int route;
    double route_time[10];
    int cus_amount[10][100]; // lượng hàng drone i trong hành trình j giao cho khách k
    int flag[100];
}drone__;

typedef struct customer__{
    double x;
    double y;
    int low;
    int upper;
    int weight; // lợi nhuận trên 1 đơn vị hàng
    int delivered;
    int truck_flag;
    int drone_flag;
}customer__;

int n, K = 1, M = 1;
int truck_speed = 40, drone_speed = 60;
int drone_duration = 30, limited_time = 30; // time giới hạn d - drone bay 1 vòng, D chung
int truck_capacity = 1500, drone_capacity = 40;        // trọng tải giới hạn     

vector<pair<double, double>> index_customer; // toạ độ khách hàng
vector<vector<double>> matrix_dist;   

truck__ truck[100];      
drone__ drone[100]; 
customer__ customer[100];

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

vector<int> get_route(string line, int &start_load, int &cnt){
    std::istringstream iss(line);
    int n = 0; cnt = 0;
    std::vector<int> drone_trip;

    while (iss >> n){
        cnt++;
        if (n == -1) start_load = cnt;
        drone_trip.push_back(n);
    }
    return drone_trip;
}

void check_truck(string line){
    std::istringstream iss(line);
    int start_load;
    int n, cnt = 0;
    std::vector<int> truck_trip;

    while (iss >> n){
        cnt++;
        if (n == -1) start_load = cnt;
        truck_trip.push_back(n);
    }
    
    set<int> check_flag;
    check_flag.insert(truck_trip[0]);
    double working_time = matrix_dist[0][truck_trip[0]];
    int count_cus = start_load - 1;
    for (int j = 1; j < count_cus; ++ j){
        check_flag.insert(truck_trip[j]);
        working_time += matrix_dist[truck_trip[j-1]][truck_trip[j]];
    }
    cout << "working_time: " << working_time << endl;
    if (check_flag.size() < count_cus) 
        cout << "Xe tai tham khach hon 1 lan" << endl;
    if (working_time > limited_time) 
        cout << "Xe tai vi pham thoi gian" << endl;

    int load_truck = 0;
    for (int j = start_load; j < cnt; ++j){
        load_truck += truck_trip[j];
        customer[truck_trip[j-start_load]].delivered += truck_trip[j];
    }
    cout << "load_truck: " << load_truck << endl;
    if (load_truck > truck_capacity) 
        cout << "Xe tai vi pham trong tai" << endl;
 
}

double check_drone_router(string line, int router){
    int start_router = 0, cnt = 0; 
    vector<int> drone_trip = get_route(line, start_router, cnt);
            
    set<int> check_flag;
    check_flag.insert(drone_trip[0]);
    double router_time = matrix_dist[0][drone_trip[0]];
    int count_cus = start_router - 1;
    for (int j = 1; j < count_cus; ++ j){
        check_flag.insert(drone_trip[j]);
        router_time += matrix_dist[drone_trip[j-1]][drone_trip[j]];
    }
    cout << "router_time: " << router_time << endl;
    if (check_flag.size() < count_cus) 
        cout << "Drone tham khac hon 1 lan trong router thu "<< router << endl;
    if (router_time > drone_duration) 
        cout << "Drone vi pham thoi gian bay trong 1 hanh trinh o router thu " << router << endl;

    int load_drone = 0;
    for (int j = start_router; j < cnt; ++j){
        load_drone += drone_trip[j];
        customer[drone_trip[j-start_router]].delivered += drone_trip[j];
    }
    cout <<"load_drone: " << load_drone << endl;
    if (load_drone > drone_capacity) 
        cout << "Drone vi pham trong tai trong router thu "<< router << endl;     

    return router_time;
}

void check_solution(string file_name){
    std::ifstream file(file_name, std::ios_base::in);
    int result ;
    file >> result ; 
    file >> K; 
    vector<string> lines;
    string line;
    getline(file, line);

    // check truck
    for (int i = 0; i < K; ++i){
        getline(file, line);
        check_truck(line);
    } 
    
    // check drone
    file >> M; 
    cout << M <<endl; 
    getline(file, line);
    int num_drone;
    file >> num_drone; 
    getline(file, line);
    for (int i = 0; i < M; ++i){
        cout << i << endl;

        vector<int> drone_trip;
        int router = 0;
        double total_time = 0;
        while (getline(file, line) && line.length() > 1) { 
            router++;
            double router_time = check_drone_router(line, router);
            total_time += router_time;
        }
        cout << "total_time: " << total_time << endl;
        if (total_time > limited_time) cout << "Drone vi phạm thời gian tổng";
    }
}

int main(){
    
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

    check_solution("fake_solu.txt");
    return 0;
}