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

typedef struct fields{
    int K; // number_truck
    int M; // number_drone
    int truck_speed, drone_speed;
    int drone_duration, limited_time; // time giới hạn d - drone bay 1 vòng, D chung
    int truck_capacity, drone_capacity;  
}fields;

int n, K, M;
int truck_speed, drone_speed;
int drone_duration, limited_time; // time giới hạn d - drone bay 1 vòng, D chung
int truck_capacity, drone_capacity;        // trọng tải giới hạn     

vector<pair<double, double>> index_customer; // toạ độ khách hàng
vector<vector<double>> matrix_dist;   

truck__ truck[100];      
drone__ drone[100]; 
customer__ customer[100];
map<string, fields> params; 

int flag;

double calculate_distance(pair<double, double> &x, pair<double, double> &y){
    return sqrt(pow(x.first - y.first, 2) + pow(x.second - y.second, 2));
}

void read_test(string file_name){
    string fname = "./test/";
    fname += file_name;

    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    fstream file(fname, ios::in);
    if (file.is_open()){
        while (getline(file, line)){
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

    for (int i = 1; i < n; i++){
        index_customer.push_back(make_pair(stod(content[i][1]), stod(content[i][2])));
        customer[i].low = stoi(content[i][3]);
        customer[i].upper = stoi(content[i][4]);
        customer[i].weight = stoi(content[i][5]);
    }

    for (int i = 0; i < n; ++i){
        vector<double> temp;
        for (int j = 0; j < n; ++j){
            temp.push_back(calculate_distance(index_customer[i], index_customer[j]));
        }
        matrix_dist.push_back(temp);
    }
}

void read_param(string file_name){
    string fname = "./test/";
    fname += file_name;

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
    
    int num_params = content.size();

    for (int i = 1; i < num_params; i++)
    {
        istringstream iss(content[i][1]);
        string num_cus;
        getline(iss, num_cus, '.');
        string num_truck, num_drone;
        int num_length = content[i][3].length();
        if (num_length > 1) {
            num_truck = content[i][3][num_length-2];
            num_drone = content[i][4][num_length-2];
        } else {
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

vector<int> get_router(string line, int &start_load, int &cnt){
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

void check_truck(string line, int num_truck){
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
    double distance = matrix_dist[0][truck_trip[0]];
    int count_cus = start_load - 1;
    for (int j = 1; j < count_cus; ++ j){
        check_flag.insert(truck_trip[j]);
        distance += matrix_dist[truck_trip[j-1]][truck_trip[j]];
    }
    // cout << "distance: " << distance << endl;
    if (check_flag.size() < count_cus) 
        flag = 1, printf("Truck %d violates delivered customer GREATER than ONCE\n", num_truck);
    double working_time = distance/truck_speed;
    if (working_time > limited_time) 
        flag = 1, printf("Truck %d violates WORKING TIME: %f\n", num_truck, working_time);

    int load_truck = 0;
    for (int j = start_load; j < cnt; ++j){
        load_truck += truck_trip[j];
        customer[truck_trip[j-start_load]].delivered += truck_trip[j];
    }
    // cout <<"load_truck: " << load_truck << endl;
    if (load_truck > truck_capacity) 
        flag = 1, printf("Truck %d violates CAPACITY: %d\n", num_truck, load_truck);
 
}

double check_drone_router(string line, int num_drone, int router){
    int start_router = 0, cnt = 0; 
    vector<int> drone_trip = get_router(line, start_router, cnt);
            
    set<int> check_flag;
    check_flag.insert(drone_trip[0]);
    double router_dis = matrix_dist[0][drone_trip[0]];
    int count_cus = start_router - 1;
    for (int j = 1; j < count_cus; ++ j){
        check_flag.insert(drone_trip[j]);
        router_dis += matrix_dist[drone_trip[j-1]][drone_trip[j]];
    }
    // cout << "router_dis: " << router_dis << endl;
    if (check_flag.size() < count_cus) 
        flag = 1, printf("Drone %d delivered customer GREATER than ONCE in router %d\n", num_drone, router);
    double router_time = router_dis/drone_speed;
    if (router_time > drone_duration) 
        flag = 1, printf("Drone %d violates DURATION in router %d: %f\n", num_drone, router, router_time);

    int load_drone = 0;
    for (int j = start_router; j < cnt; ++j){
        load_drone += drone_trip[j];
        customer[drone_trip[j-start_router]].delivered += drone_trip[j];
    }
    // cout <<"load_drone: " << load_drone << endl;
    if (load_drone > drone_capacity) 
        flag = 1, printf("Drone %d violates CAPACITY in router %d: %d\n", num_drone, router, load_drone);     

    return router_dis;
}

void check_customer(){
    // cout << "n: " << n << endl;
    for (int i = 1; i < n; ++i){
        int amount =  customer[i].delivered;
        // cout << amount << " " << customer[i].low << " " << customer[i].upper << endl;
        if (amount < customer[i].low) 
            flag = 1, printf("Customer %d LESS than low: %d\n", i, amount);
        if (amount > customer[i].upper)
            flag = 1, printf("Customer %d GREATER upper: %d\n", i, amount);
    }
}

void check_solution(string file_name){
    std::ifstream file(file_name, std::ios_base::in);
    vector<string> lines;
    string line;
    getline(file, line);

    // check truck
    int i = 1;
    while (getline(file, line)) {
        int length = line.length();
        if (length > 1){
            check_truck(line, i++);
        } 
        else if (length == 1){
            M = stoi(line);
            break;
        }
        else exit(0);
    } 
    
    // check drone
    getline(file, line);

    for (int i = 0; i < M; ++i){
        // cout << "Drone " << i+1 << endl;

        int router = 0;
        double total_dis = 0;
        while (getline(file, line) && line.length() > 1) { 
            router++;
            double router_dis = check_drone_router(line, i+1, router);
            total_dis += router_dis;
        }
        // cout << "total_dis drone: " << i << " " << total_dis << endl;
        double total_time = total_dis/drone_speed;
        if (total_time > limited_time) 
            flag = 1, printf("Drone %d violates working time: %f\n", i+1, total_time);
    }
    check_customer();
}

int main(){
    // loop read file
    // int not_feasible;
    // int num_cus[5] = {6, 10, 12, 20, 50};
    // int area[5] = {5, 10, 20, 30, 40};
    // string str;

    // for (auto cus: num_cus){
    //     for (int i = cus < 50 ? 0 : 1; i < (cus < 50 ? 3 : 5); ++ i){
    //         for (int j = 1; j < 5; ++j){
    //             str = to_string(cus) + "." + to_string(area[i]) + "." + to_string(j);
    //             read_test(str + ".csv");
    //             check_solution(str + ".txt");
    //             if (flag) not_feasible++;
    //             else cout << "FEASIBLE SOLUTION !!!";
    //         } 
    //     }
    // }

    string test = "6.5.1";
    read_test(test + ".csv");

    read_param("params.csv");
    K = params[test].K;
    M = params[test].M;
    truck_speed = params[test].truck_speed;
    drone_speed = params[test].drone_speed;
    drone_duration = params[test].drone_duration;
    limited_time = params[test].limited_time;
    truck_capacity = params[test].truck_capacity;
    drone_capacity = params[test].drone_capacity;

    check_solution("fake_solu.txt");
    if (!flag) cout << "FEASIBLE SOLUTION";

    return 0;
}