#include <bits/stdc++.h>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;


ofstream ofs;
ofstream ofss;
    

struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;   //机器人目标点
    int target_berth;
    bool on_road; //机器人是否正在前往的路上
    bool stuck; //机器人是否被困住
    int narrow;//判断机器人是否进入窄道碰撞
    vector<int> move_order;//机器人的运行指令
    Robot() {
        stuck = 0;
        on_road = false;
        target_berth = -1;
        narrow = 0;
    }
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
        on_road = false;
    }
}robot[robot_num + 10];

struct Goods
{
    int id, x, y, val; //货物出现帧数，地址，价值
    int min_berth;//到所有泊口最短的那个
    int goods_berth_dis[10];//到所有泊口的距离
    vector<int> berth_path[10];//到所有泊口的路径
    double weight_val; //货物堆泊口的加权价值
    Goods(){
        id = 0;
        x = 0;
        y = 0;
        val = 0;
    }
    Goods(int id, int x, int y, int val){
        this->id = id;
        this->x = x;
        this->y = y;
        this->val = val;
        this->weight_val = 0;
    }

    bool operator<(const Goods &b) const
    {
        return weight_val < b.weight_val;
    }

};

struct Berth
{
    int x;
    int y;
    int transport_time;
    int loading_speed;
    int goods_num;//泊口货物数量
    int pre_value;//预计泊口价值
    int esti_goods_num;//估计泊口空闲货物
    vector<int> boat_num;//储存当前泊口的船只
    vector<int > robot;
    priority_queue<Goods> goods_queue;
    Berth(){goods_num = 0;pre_value = 0;esti_goods_num = 0;}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
        this -> goods_num = 0;
    }
}berth_a[berth_num + 10];


struct Boat
{
    int num, pos, status;
    int remain_capacity;//增加船剩余容量
}boat[10];

struct Point
{
    int x, y, priority;
    Point(){}
    Point(int x, int y, int p){
        this->x = x;
        this->y = y;
        this->priority = p;
    }
        Point(int x,int y){
        this->x = x;
        this->y = y;
    }
   bool operator< (const Point &a)const{
    return priority > a.priority;
   }
};

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];
int gds_b[N][N];//记录泊口的货物情况
// int mp[N][N];
int direction[4][2] = {0, 1, 0, -1, -1, 0, 1, 0};
int grid_x[] = {1,1,2,2}, grid_y[] = {1,2,1,2};
int berth_sign[5] = {0};
int get_money1;
int sum_money;
int lost_money;
int max_berth_time = 0;//最长的泊口时间
int max_transport_time;//13000贞后货物只运往有船的泊口
int nar[N][N];
vector<Goods> Goods_all;
vector<Goods> Goods_all_pending;//待处理货物
vector<Berth> berth;
vector<set<int>> berth_connection;
vector<int> selected_berth;
vector<int> unselected_berth;

vector<int > find_road(int x1, int y1, int x2, int y2, int mp[N][N]);
vector<int > find_road(int x1, int x2, int x3, int x4);//计算两点之间距离
int heuristic(int x, int y, int x2, int y2);
void RobotCmd();
void Move(int robot_id);
void ShipCmd();
void ChooseGoods(int robot_id);
void Test();
void Robot_Init(int id);
void Wall_Check(int i, int mp[N][N]);
void mp_Init(int mp[N][N]);
int update(int x, int y, int sign[N][N][2]);
void Get_path_berth(Goods& g, int berth_id);//寻找货物到其他泊口的路径
void Get_path_berth(Goods& g);//寻找泊口到其他泊口的路径
void Goods_process();
void Del_Goods();
int find_most_berth(int i, int status);
void connectionInit();//初始化泊口的连通分量
void selectBerth();
bool cmp(int a,int b);
int get_berth_id(int x_d, int y_d, int sign[N][N][2]);
int Get_Goods(int i);
int find_most_berth_unselected();
void Go(int i);
void Transfer(int i, int max_id);
void update_esti_num();
void update_goods_info(int i, int robot_id);
void ShipCmd_end();
bool check_direction(int x, int y);
vector<int> find_unstuck_road(int x1, int y1, int mp[N][N]);
bool check_normal(int i, int x, int y, int x1, int y1);
void InitNarrow();
bool check_robot_narrow(int narrow_id);
void dfs(int x,int y,int vis[N][N],int num);
vector<int> pointDirection(int i,int j);
bool checkPoint(int x,int y);
bool check_robot_narrow(int narrow_id, int robot_id);

void Init()
{
    for(int i = 0; i < n; i ++)
        scanf("%s", ch[i]);
    for(int i = 0; i < berth_num; i ++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth_a[id].x, &berth_a[id].y, &berth_a[id].transport_time, &berth_a[id].loading_speed);
        if(berth_a[id].transport_time > max_berth_time)
        max_berth_time = berth_a[id].transport_time;
    }
    max_transport_time = 15000 - max_berth_time - 650;
    scanf("%d", &boat_capacity);
    for(int i = 0;i<5;i++)
    boat[i].remain_capacity = boat_capacity;
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

int Input()
{
    scanf("%d%d", &id, &money);
    int num;
    scanf("%d", &num);
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);sum_money += val;
        Goods_all_pending.push_back(Goods(id, x, y, val));
    }
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &robot[i].status);
    }
    for(int i = 0; i < 5; i ++)
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);
    char okk[100];
    scanf("%s", okk);
    return id;
}

int pull_sum;
int main()
{
    Init();
    ofs.open("set.txt",ios::out);
    ofss.open("map.txt",ios::out);
    for(int zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
        // if(id == 1)
        // {
        //     InitNarrow();
        //     for(int i = 0; i < 200; i++)
        //     {
        //         for(int j = 0; j < 200; j++)
        //         {
        //             ofs << setw(4) <<nar[i][j] <<' ';
        //         }
        //         ofs <<endl;
        //     }
        // }
        if(id <= 5)
        {
            Robot_Init(id-1);
            Robot_Init(id+4);
            connectionInit();
        }
        else
        {
            if(id == 9 || id == 10)InitNarrow();
            if(id == 9000)
            selectBerth();
            Goods_process(); 
            RobotCmd();
            ShipCmd();
            for(int i = 0; i < 5; i++)
            ofs <<boat[i].remain_capacity <<' ' <<boat[i].status <<boat[i].pos <<"; ";
            ofs <<get_money1 <<' ' <<id <<' ' <<money <<' ';
            ofs <<sum_money <<' ' <<lost_money <<' ';
            ofs <<endl;
            for(int i = 0; i < 10; i++)
            ofs <<berth_a[i].goods_num <<' ';
        }
        puts("OK");
        fflush(stdout);
    }
    // ofs.close();
    return 0;
}

//一帧中机器人的指令
void RobotCmd()
{
    int mp[N][N] = {0};
    mp_Init(mp);
    for(int i = 0;i<10;i++)
    {
        if(robot[i].stuck) 
            continue;
        //如果还没走完，按之前计算出的路径继续走
        if(robot[i].status == 0)
        {
            if(robot[i].narrow)
            {
                if(robot[i].goods)
                {
                    Robot_Init(i);
                }
                else
                {
                    ChooseGoods(i);
                }
                //robot[i].move_order = find_road(robot[i].x, robot[i].y, berth_a[robot[i].target_berth].x + grid_x[rand() % 4], berth_a[robot[i].target_berth].y + grid_y[rand() % 4], mp);
                robot[i].narrow = 0;
            }
            continue;
        }
        if(robot[i].move_order.size())
        {
            Wall_Check(i, mp);
            Move(i);
        }
        //如果已经到达了目的地
        else
        {
            int x_d, y_d;
            if(robot[i].target_berth >= 0)
            {
                //srand(id);
                x_d = berth_a[robot[i].target_berth].x + grid_x[rand() % 4];
                y_d = berth_a[robot[i].target_berth].y + grid_y[rand() % 4];
            }
            //如果到达了货物，就进行泊口路径规划
            //如果当前坐标有货物
            if(gds[robot[i].x][robot[i].y])
            {
                printf("get %d\n",i);
                gds[robot[i].x][robot[i].y] = 0;
                robot[i].move_order = find_road(robot[i].x, robot[i].y, x_d, y_d);
                int dx = direction[robot[i].move_order.back()][0];
                int dy = direction[robot[i].move_order.back()][1];
                mp[robot[i].x+dx][robot[i].y+dy]++;
                robot[i].mbx = x_d;robot[i].mby = y_d;
                Wall_Check(i, mp);
                Move(i);
                continue;
            }

            if(ch[robot[i].x][robot[i].y] == 'B' && robot[i].goods)
            {
                //否则到达了泊口，重新进行货物路径规划
                if(robot[i].goods)
                {
                    printf("pull %d\n",i);
                    berth_a[robot[i].target_berth].goods_num ++;
                }
                ChooseGoods(i);
                if(robot[i].move_order.size() != 0)
                {
                    int dx = direction[robot[i].move_order.back()][0];
                    int dy = direction[robot[i].move_order.back()][1];
                    mp[robot[i].x+dx][robot[i].y+dy]++;
                }
                Wall_Check(i, mp);
                Move(i);
                continue;
            }
            if(robot[i].goods)
            {
                //如果机器人手上有货物但是没有路径
                if(robot[i].narrow != 0)
                {
                    if(pointDirection(robot[i].x, robot[i].y).size() < 3)
                    {
                        robot[i].move_order = find_unstuck_road(robot[i].x, robot[i].y, mp);
                    }
                    else if(check_robot_narrow(robot[i].narrow, i))
                    {
                        robot[i].move_order = find_road(robot[i].x, robot[i].y, x_d, y_d);
                        robot[i].narrow = 0;
                    }
                }
                else
                robot[i].move_order = find_road(robot[i].x, robot[i].y, x_d, y_d);
                if(robot[i].move_order.size())
                {
                    int dx = direction[robot[i].move_order.back()][0];
                    int dy = direction[robot[i].move_order.back()][1];
                    mp[robot[i].x+dx][robot[i].y+dy]++;
                    robot[i].mbx = x_d;robot[i].mby = y_d;
                    Wall_Check(i, mp);
                    Move(i);
                }
            }
            else
            {
                //如果机器人没货没路径
                if(robot[i].narrow != 0)
                {
                    if(pointDirection(robot[i].x, robot[i].y).size() < 3)
                    {
                        robot[i].move_order = find_unstuck_road(robot[i].x, robot[i].y, mp);
                    }
                    else if(check_robot_narrow(robot[i].narrow, i))
                    {
                        robot[i].move_order = find_road(robot[i].x, robot[i].y, robot[i].mbx, robot[i].mby);
                        robot[i].narrow = 0;
                    }
                }
                else
                ChooseGoods(i);
                if(robot[i].move_order.size() != 0)
                {
                    int dx = direction[robot[i].move_order.back()][0];
                    int dy = direction[robot[i].move_order.back()][1];
                    mp[robot[i].x+dx][robot[i].y+dy]++;
                    Wall_Check(i, mp);
                    Move(i);
                }
            }

        }
                
    }

}

//机器人按照路径移动
void Move(int robot_id)
{
    if(robot[robot_id].move_order.empty()) 
        return; 
    int dir = robot[robot_id].move_order.back();
    robot[robot_id].move_order.pop_back();
    printf("move %d %d\n",robot_id,dir);
}

void ChooseGoods(int robot_id)//从所有货物中选择最优的那个
{
    vector<int >path;
    if(Goods_all.size() == 0)
    {
        robot[robot_id].move_order = path;
    }
    int target = robot[robot_id].target_berth;
    int optimal_Goods = -1, dis;
    double max_weight = -1, now_weight;
    int goods_num = Goods_all.size();
    for(int i = 0; i < Goods_all.size(); i++)
    {
        if(Goods_all[i].goods_berth_dis[target] + id - Goods_all[i].id >= 1000)
        {
            continue;
        }
        if(id >= 10000)
        {
            update_goods_info(i, robot_id);
        }
        if(Goods_all[i].min_berth == -1)
        {
            continue;
        }
        dis = Goods_all[i].goods_berth_dis[target] + Goods_all[i].goods_berth_dis[Goods_all[i].min_berth];
        now_weight = ((double)Goods_all[i].val + (double)berth_a[Goods_all[i].min_berth].pre_value*0.008 + 17) / (double)(dis+10);
        //now_weight = 10000 / dis;
        //now_weight = Goods_all[i].val;
        //now_weight = (double)Goods_all[i].val*2 - (double)dis;
        if(now_weight > max_weight)
        {
            max_weight = now_weight;
            optimal_Goods = i;
        }
    }
    if(optimal_Goods != -1)
    {
        // robot[robot_id].move_order = Goods_all[optimal_Goods].berth_path[target];
        robot[robot_id].move_order = find_road(robot[robot_id].x, robot[robot_id].y, Goods_all[optimal_Goods].x, Goods_all[optimal_Goods].y);
        robot[robot_id].mbx = Goods_all[optimal_Goods].x;
        robot[robot_id].mby = Goods_all[optimal_Goods].y;
        robot[robot_id].target_berth = Goods_all[optimal_Goods].min_berth;
        get_money1 += Goods_all[optimal_Goods].val;
        berth_a[Goods_all[optimal_Goods].min_berth].pre_value -= Goods_all[optimal_Goods].val;
        Goods_all.erase(Goods_all.begin() + optimal_Goods);
        return;
    }
    else {robot[robot_id].move_order = path;}
}


int choice_unselected_berth[5];
void ShipCmd()
{
    int max_id;
    for(int i = 0; i < 5; i++)
    {
        if(id >= 10000)
        {
            ShipCmd_end();
            break;
        }
        update_esti_num();
        if(boat[i].status == 0)continue;
        if(boat[i].status == 1 && boat[i].pos >=0)
        {//如果处于正常运行状态
            int get_goods = Get_Goods(i);
            //if(boat[i].remain_capacity == 0 || (14999 - id < berth_a[boat[i].pos].transport_time + 4*max_berth_time + 502 && 14999 - id > berth_a[boat[i].pos].transport_time + 2*max_berth_time))
            if(boat[i].remain_capacity == 0)
            {
                Go(i);
                continue;
            }
            if(get_goods < berth_a[boat[i].pos].loading_speed)
            {
                max_id = find_most_berth(i, 0);
                if(max_id == -1)continue;
                if(id >= 8600)
                {
                    Transfer(i, max_id);
                }
                else if(boat[i].remain_capacity <= 0.2*boat_capacity)
                {
                    Go(i);
                }
                else if(berth_a[max_id].esti_goods_num >= boat[i].remain_capacity)
                {
                    Transfer(i, max_id);
                }
            }
        }
        else if(boat[i].status == 1 && boat[i].pos == -1)
        {
            max_id = find_most_berth(i, -1);
            if(max_id == -1)continue;
            berth_a[max_id].boat_num.push_back(i);
            printf("ship %d %d\n", i, max_id);
        }
        else if(boat[i].status == 2)
        {
            //printf("ship %d %d\n", i, boat[i].pos);
        }
    }
}

//find_road()//计算两点之间距离
vector<int > find_road(int x1, int y1, int x2, int y2)//计算两点之间距离
{
    Point c_point;
    priority_queue<Point > priority;
    vector<int > ans;
    if(x1 == x2 && y1 == y2)return ans;
    int sign[205][205][2];//记录当前点到起点的距离和上一步的方向
    for(int i = 0;i<n;i++)
    for(int j = 0;j<n;j++)
    {sign[i][j][1] = 0;sign[i][j][0] = -1;}
    priority.push(Point(x1, y1, 0));
    sign[x1][y1][0] = 0;
    sign[x1][y1][1] = -1;

    while(!priority.empty()){
        int x, y, x_d, y_d;
        c_point = priority.top();
        priority.pop();
        x = c_point.x;y = c_point.y;
        if(x == x2 && y == y2){ //是否到达终点
            int step = sign[x2][y2][1];
            while(step != -1){
                ans.push_back(step);
                x -= direction[step][0];y -= direction[step][1];
                step = sign[x][y][1];
            }
            //reverse(ans.begin(), ans.end());
            return ans;
        }
        for(int i = 0;i<4;i++){
            x_d = x+direction[i][0];y_d = y+direction[i][1];
            if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A') && sign[x_d][y_d][0] == -1){
                sign[x_d][y_d][0] = sign[x][y][0] + 1;
                sign[x_d][y_d][1] = i;
                priority.push(Point(x_d, y_d, sign[x_d][y_d][0] + heuristic(x_d, y_d, x2, y2)));
            }
        }
    }
    return ans;
}

vector<int > find_road(int x1, int y1, int x2, int y2, int mp[N][N])//计算两点之间距离,机器人当作障碍
{
    Point c_point;
    priority_queue<Point > priority;
    vector<int > ans;
    if(x1 == x2 && y1 == y2)return ans;

    int sign[210][210][2];//记录当前点到起点的距离和上一步的方向
    for(int i = 0;i<n;i++)
    for(int j = 0;j<n;j++)
    {sign[i][j][1] = 0;sign[i][j][0] = -1;}
    priority.push(Point(x1, y1, 0));
    sign[x1][y1][0] = 0;
    sign[x1][y1][1] = -1;

    while(!priority.empty()){
        int x, y, x_d, y_d;
        c_point = priority.top();
        priority.pop();
        x = c_point.x;y = c_point.y;
        if(x == x2 && y == y2){ //是否到达终点
            int step = sign[x2][y2][1];
            while(step != -1){
                ans.push_back(step);
                x -= direction[step][0];y -= direction[step][1];
                step = sign[x][y][1];
            }
            //reverse(ans.begin(), ans.end());
            return ans;
        }
        for(int i = 0;i<4;i++){
            x_d = x+direction[i][0];y_d = y+direction[i][1];
            if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A') && mp[x_d][y_d] == 0 && sign[x_d][y_d][0] == -1){
                sign[x_d][y_d][0] = sign[x][y][0] + 1;
                sign[x_d][y_d][1] = i;
                priority.push(Point(x_d, y_d, sign[x_d][y_d][0] + heuristic(x_d, y_d, x2, y2)));
            }
        }
    }
    return ans;
}
void Get_path_berth(Goods &g)//获得所有泊口到货物的路线和距离
{
    Point c_point;
    queue<Point > BFS;
    vector<int > path[10];
    int num = 0;    
    int x, y;
    //if(gds[robot[robot_id].x][robot[robot_id].y])return ans;
    int sign[N][N][2] = {0};//记录当前点的距离和上一步的方向
    BFS.push(Point(g.x, g.y, 0));
    sign[g.x][g.y][0] = 1;
    sign[g.x][g.y][1] = -1;
    while(!BFS.empty())
    {
        int x_d, y_d;
        c_point = BFS.front();
        BFS.pop();
        x = c_point.x;y = c_point.y;
        for(int i = 0;i<4;i++)
        {
            x_d = x+direction[i][0];y_d = y+direction[i][1];
            if(ch[x_d][y_d] == 'B' && sign[x_d][y_d][0] == 0)//如果找到泊口
            {
                num++;
                int berth_sign = update(x_d, y_d, sign);
                path[berth_sign].push_back(i);
                x_d = x; y_d = y;
                int step = sign[x_d][y_d][1];
                while(step != -1){
                    path[berth_sign].push_back(step);
                    x_d -= direction[step][0];y_d -= direction[step][1];
                    step = sign[x_d][y_d][1];
                }
            g.berth_path[berth_sign] = path[berth_sign];
            continue;
            }
            if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A') && sign[x_d][y_d][0] == 0)//如果该点可以移动且没有访问，则加入队列
            {
                BFS.push(Point(x_d, y_d, c_point.priority+1));
                sign[x_d][y_d][0] = 1;
                sign[x_d][y_d][1] = i;
            }
        }
    }
    int min_dis = 100000, min_id = -1;
    for(int i = 0; i < 10; i++)
    {
        g.goods_berth_dis[i] = g.berth_path[i].size();
        if(g.berth_path[i].size() == 0)
        {
            g.goods_berth_dis[i] = 0x00ffffff;
        }
        if(g.goods_berth_dis[i] < min_dis)
        {
            if(id >= 15000 && find(unselected_berth.begin(),unselected_berth.end(),i) != unselected_berth.end())
                continue;
            min_dis = g.goods_berth_dis[i];
            min_id = i;
        }
    }
    g.min_berth = min_id;
}

void Get_path_berth(Goods &g, int berth_id)//获得其他所有泊口到某一泊口的路线和距离
{
    Point c_point;
    queue<Point > BFS;
    vector<int > path[10];
    int num = 0;
    int x, y;
    //if(gds[robot[robot_id].x][robot[robot_id].y])return ans;
    int sign[N][N][2] = {0};//记录当前点的距离和上一步的方向
    BFS.push(Point(g.x, g.y, 0));
    sign[g.x][g.y][0] = 1;
    sign[g.x][g.y][1] = -1;
    while(!BFS.empty())
    {
        int x_d, y_d;
        c_point = BFS.front();
        BFS.pop();
        x = c_point.x;y = c_point.y;
        for(int i = 0;i<4;i++)
        {
            x_d = x+direction[i][0];y_d = y+direction[i][1];
            if(ch[x_d][y_d] == 'B' && sign[x_d][y_d][0] == 0)//如果找到泊口
            {
                num++;
                int berth_sign = get_berth_id(x_d, y_d, sign);
                if(berth_sign != berth_id)
                {
                    berth_sign = update(x_d, y_d, sign);
                    path[berth_sign].push_back(i);
                    x_d = x; y_d = y;
                    int step = sign[x_d][y_d][1];
                    while(step != -1){
                        path[berth_sign].push_back(step);
                        x_d -= direction[step][0];y_d -= direction[step][1];
                        step = sign[x_d][y_d][1];
                    }
                    g.berth_path[berth_sign] = path[berth_sign];
                    continue;
                }
            }
            if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A') && sign[x_d][y_d][0] == 0)//如果该点可以移动且没有访问，则加入队列
            {
                BFS.push(Point(x_d, y_d, c_point.priority+1));
                sign[x_d][y_d][0] = 1;
                sign[x_d][y_d][1] = i;
            }
        }
    }
    int min_dis = 100000, min_id = -1;
    for(int i = 0; i < 10; i++)
    {
        //if(i == berth_id)continue;
        g.goods_berth_dis[i] = g.berth_path[i].size();
        if(g.berth_path[i].size() == 0)
        {
            g.goods_berth_dis[i] = 0x00ffffff;
        }
    }
}

vector<int> find_unstuck_road(int x1, int y1, int mp[N][N])//找到一个没有被陷入的点
{
    Point c_point;
    priority_queue<Point > priority;
    vector<int > ans;

    int sign[210][210][2];//记录当前点到起点的距离和上一步的方向
    for(int i = 0;i<n;i++)
    for(int j = 0;j<n;j++)
    {sign[i][j][1] = 0;sign[i][j][0] = -1;}
    priority.push(Point(x1, y1, 0));
    sign[x1][y1][0] = 0;
    sign[x1][y1][1] = -1;

    while(!priority.empty()){
        int x, y, x_d, y_d;
        c_point = priority.top();
        priority.pop();
        x = c_point.x;y = c_point.y;
        if(pointDirection(x, y).size() == 4){ //是否找到开阔的场地
            for(int i =0; i < 4; i++)
            {
                x_d = x + direction[i][0];
                y_d = y + direction[i][1];
                if(pointDirection(x_d, y_d).size() > 2)
                {
                    for(int j =0; j < 4; j++)
                    {
                        int x_dd = x_d + direction[j][0];
                        int y_dd = y_d + direction[j][1];
                        if(pointDirection(x_dd, y_dd).size() > 2)
                        {
                            ans.push_back(j);
                            break;
                        }
                    }
                    ans.push_back(i);
                    break;
                }
            }
            int step = sign[x][y][1];
            while(step != -1){
                ans.push_back(step);
                x -= direction[step][0];y -= direction[step][1];
                step = sign[x][y][1];
            }
            //reverse(ans.begin(), ans.end());
            return ans;
        }
        for(int i = 0;i<4;i++){
            x_d = x+direction[i][0];y_d = y+direction[i][1];
            if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A') && mp[x_d][y_d] == 0 && sign[x_d][y_d][0] == -1){
                sign[x_d][y_d][0] = sign[x][y][0] + 1;
                sign[x_d][y_d][1] = i;
                priority.push(Point(x_d, y_d, sign[x_d][y_d][0]));
            }
        }
    }
    return ans;
}

int heuristic(int x, int y, int x2, int y2){//代价估计函数
    // return 0;
    return abs(x2-x) + abs(y2-y);
}

void Robot_Init(int robot_id)//机器人初始化，找到一个能到达的泊口
{
    Goods g(0, robot[robot_id].x, robot[robot_id].y, 0);
    // vector<char> chs;
    // for(int i = 0;i<10;i++)
    // {
    //     if(i==robot_id) {
    //         chs.push_back(ch[robot[i].x][robot[i].y]);
    //         continue;
    //     }
    //     chs.push_back(ch[robot[i].x][robot[i].y]);
    //     ch[robot[i].x][robot[i].y] = '#';
    // }
    Get_path_berth(g);
    // for(int i = 0;i<10;i++)
    // {
    //     ch[robot[i].x][robot[i].y] = chs[i];
    // }
    if(g.min_berth == -1)
    {
        robot[robot_id].stuck = 1;
        return;
    }
    //robot[robot_id].move_order = g.berth_path[g.min_berth];
    robot[robot_id].target_berth = g.min_berth;
    robot[robot_id].mbx = berth_a[g.min_berth].x;
    robot[robot_id].mby = berth_a[g.min_berth].y;
}

void Wall_Check(int i, int mp[N][N])//碰撞监测
{
    int next_step = -1;
    int x, y, x_d, y_d;
    if(robot[i].move_order.size() != 0)
    {
        next_step = robot[i].move_order.back();
        if(next_step != -1)
        x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
    }
    else
    {
        x = robot[i].x;
        y = robot[i].y;
    }
    if((ch[x][y] != '.' && ch[x][y] != 'B' && ch[x][y] != 'A'))//如果下一步是墙
    {
        mp[robot[i].x][robot[i].y]--;
        if(robot[i].move_order.size() != 0)
        mp[x][y]--;
        robot[i].move_order = find_road(robot[i].x, robot[i].y, robot[i].mbx, robot[i].mby);
        mp[robot[i].x][robot[i].y]++;
        if(robot[i].move_order.size() != 0)
        {
            next_step = robot[i].move_order.back();
            x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
            mp[x][y]++;
        }
    }
    else if(mp[x][y] >= 2)
    {
        //ofss <<id <<endl;
        // int d = check_direction(robot[i].x, robot[i].y, next_step);
        // if(d != -1)
        // {
        //     robot[i].move_order.push_back(d + (d%2==0?1:-1));
        //     robot[i].move_order.push_back(d);
        //     mp[x][y]--;
        //     next_step = robot[i].move_order.back();
        //     x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
        //     mp[x][y]++;
        //     return;
        // }
        if(robot[i].narrow != 0)
        {
            mp[robot[i].x][robot[i].y]--;
            if(robot[i].move_order.size() != 0)
            mp[x][y]--;
            robot[i].move_order = find_road(robot[i].x, robot[i].y, robot[i].mbx, robot[i].mby, mp);
        mp[robot[i].x][robot[i].y]++;
        if(robot[i].move_order.size() != 0)
        {
            next_step = robot[i].move_order.back();
            x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
            mp[x][y]++;
        }
        if(robot[i].move_order.size() != 0)
    {
        mp[robot[i].x][robot[i].y] --;
    }
    return;

        }
        if(check_normal(i, robot[i].x, robot[i].y, x, y) && next_step != -1)return;
        // mp[robot[i].x][robot[i].y]--;
        // if(robot[i].move_order.size() != 0)
        // mp[x][y]--;
        // if(check_direction(robot[i].x, robot[i].y))
        // {
        //     vector<int >path = find_unstuck_road(robot[i].x, robot[i].y, mp);
        //     for(int k = path.size()-1; k >= 0; k--)
        //     {
        //         robot[i].move_order.push_back(path[k] + (path[k]%2==0?1:-1));
        //     }
        //     robot[i].move_order.insert(robot[i].move_order.end(), path.begin(), path.end());
        // }
        mp[robot[i].x][robot[i].y]--;
        if(robot[i].move_order.size() != 0)
        mp[x][y]--;
        if(pointDirection(robot[i].x, robot[i].y).size() == 2)
        {
            robot[i].move_order = find_unstuck_road(robot[i].x, robot[i].y, mp);
            robot[i].narrow = nar[robot[i].x][robot[i].y];
        }
        else
        robot[i].move_order = find_road(robot[i].x, robot[i].y, robot[i].mbx, robot[i].mby, mp);
        mp[robot[i].x][robot[i].y]++;
        if(robot[i].move_order.size() != 0)
        {
            next_step = robot[i].move_order.back();
            x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
            mp[x][y]++;
        }
    }
    if(robot[i].move_order.size() != 0)
    {
        mp[robot[i].x][robot[i].y] --;
    }
    
    return;
}
void mp_Init(int mp[N][N])//机器人位置初始化
{
    int l;
    int next_step;
    int x, y, x_d, y_d;
    for(int j = 0;j<10;j++)
    {   
        x = robot[j].x;y = robot[j].y;
        mp[x][y]++;
        if(robot[j].move_order.size())
        {
            next_step = robot[j].move_order.back();
            x = robot[j].x + direction[next_step][0]; y = robot[j].y + direction[next_step][1];
            mp[x][y]++;
        }

    }
}


int update(int x, int y, int sign[N][N][2])//更新访问的泊口
{
    int x_d, y_d;
    for(int i = 0; i < 10; i++)
    {
        x_d = berth_a[i].x;
        y_d = berth_a[i].y;
        if(x <= x_d + 3 && x >= x_d && y >= y_d && y <= y_d + 3)
        {
            for(int k = 0; k <= 3; k++)
            for(int l = 0; l <= 3; l++)
            sign[x_d + k][y_d + l][0] = 1;
            return i;
        }
    }
    return -1;
}

void Goods_process()//货物加工，求货物到各个泊口的距离
{
    int i = 0;
    while(Goods_all.size())
    {
        if(id - Goods_all[i].id > 1000)
        {
            lost_money += Goods_all[0].val;
            gds[Goods_all[i].x][Goods_all[i].y] = 0;
            berth_a[Goods_all[i].min_berth].pre_value -= Goods_all[i].val;
            Goods_all.erase(Goods_all.begin());
        }
        else break;
    }
    int ele_num = Goods_all_pending.size();
    int num = min(2, ele_num);
    while(Goods_all_pending.size() && i < num)
    {
        Goods g = Goods_all_pending[0];
        Get_path_berth(g);
        if(g.min_berth != -1)
        {
            Goods_all.push_back(g);
            berth_a[g.min_berth].pre_value += g.val;
            gds[g.x][g.y] =1;
        }
        Goods_all_pending.erase(Goods_all_pending.begin());
        i++;
    }
}


int find_most_berth(int i, int status)//找到货物最合适泊口
{
    int min = 100000, fit_id = -1;
    for(int j = 0; j < 10; j++)
    {
        // if(berth_a[j].boat_num != -1)
        // continue;
        if((status == -1 && berth_a[j].transport_time*2 < 14990 - id) || (status == 0 && 550 + berth_a[j].transport_time < 14999 - id))
        if(berth_a[j].esti_goods_num != 0 && abs(berth_a[j].esti_goods_num-boat[i].remain_capacity) < min)
        {
            min = abs(berth_a[j].esti_goods_num-boat[i].remain_capacity);
            fit_id = j;
        }
    }
    return fit_id;
}

bool connected(int i)//判断第i个泊口是否已被连接
{
    for(auto s:berth_connection)
    {
        if(s.find(i)!=s.end()) return true;
    }
    return false;
}

void connectionInit()
{
    for(int i=(id-1)*2;i<=(id-1)*2+1;i++)
    {
        if(!connected(i))
        {
            set<int> s;
            s.insert(i);
            berth_connection.push_back(s);
            ofs<<"i = " <<i<<" 创建"<<endl;
        }
        else continue;
        Goods g(0, berth_a[i].x, berth_a[i].y, 0);
        Get_path_berth(g, i);
        for(int j=0;j<10;j++)
        {
            if(j==i) continue;
            auto road = g.berth_path[j];
            if(road.size())
            {
                berth_connection[berth_connection.size()-1].insert(j);
            }
        }
    }
}

//selected_berth
//unselected_berth
bool cmp(int a,int b)
{
    //return berth_a[a].pre_value > berth_a[b].pre_value;
    return berth_a[a].transport_time < berth_a[b].transport_time;
}
void selectBerth()
{
    if(selected_berth.size()) return;
    //连通分量的数量
    int num = berth_connection.size();
    vector<vector<int>> v(num);//排序连通分量泊口
    vector<int> vv;//排序所有泊口
    for(int i =0 ;i<num;i++)
    {
        for(int j:berth_connection[i])
        {
            v[i].push_back(j);
            vv.push_back(j);
        }
        sort(v[i].begin(),v[i].end(),cmp);
    }
    ofs<<"num = "<<num<<endl;
    sort(vv.begin(),vv.end(),cmp);
    for(int i=0;i<num;i++)
        selected_berth.push_back(v[i][0]);

    for(int i = 0;i<10 && selected_berth.size()<5;i++)
    {
        if(find(selected_berth.begin(),selected_berth.end(),vv[i]) ==selected_berth.end())
            selected_berth.push_back(vv[i]);
    }
    for(int i = 0;i<10;i++)
    {
        if(find(selected_berth.begin(),selected_berth.end(),vv[i]) == selected_berth.end())
            unselected_berth.push_back(vv[i]);
    }

    ofs<<"selected: ";
    for(auto it:selected_berth) ofs<<it<<" ";
    ofs<<endl<<"unselected: ";
    for(auto it:unselected_berth) ofs<<it<<" ";

}

int get_berth_id(int x, int y, int sign[N][N][2])
{
    int x_d, y_d;
    for(int i = 0; i < 10; i++)
    {
        x_d = berth_a[i].x;
        y_d = berth_a[i].y;
        if(x <= x_d + 3 && x >= x_d && y >= y_d && y <= y_d + 3)
        {
            return i;
        }
    }
    return -1;
}

int Get_Goods(int i)
{
    int get_goods = min(boat[i].remain_capacity, min(berth_a[boat[i].pos].goods_num, berth_a[boat[i].pos].loading_speed));
    boat[i].remain_capacity -= get_goods;//取货
    berth_a[boat[i].pos].goods_num -= get_goods;
    return get_goods;
}

void Go(int i)
{
    printf("go %d\n", i);
    boat[i].remain_capacity = boat_capacity;
    auto it = find(berth_a[boat[i].pos].boat_num.begin(), berth_a[boat[i].pos].boat_num.end(), i);
    berth_a[boat[i].pos].boat_num.erase(it);
    return;
}

void Transfer(int i, int max_id)
{
    berth_a[max_id].boat_num.push_back(i);
    auto it = find(berth_a[boat[i].pos].boat_num.begin(), berth_a[boat[i].pos].boat_num.end(), i);
    if(berth_a[boat[i].pos].boat_num.end() != it)
    berth_a[boat[i].pos].boat_num.erase(it);
    printf("ship %d %d\n", i, max_id);
    return;
}

void update_esti_num()
{
    for(int i = 0; i < 10; i++)
    {
        int pull_goods = 0;
        for(int j = 0; j < berth_a[i].boat_num.size(); j++)
        {
            pull_goods += boat[berth_a[i].boat_num[j]].remain_capacity;
        }
        berth_a[i].esti_goods_num = max(0, berth_a[i].goods_num - pull_goods);
    }
}

void update_goods_info(int i, int robot_id)//到点了，货物不能乱送，精准扶贫
{
    Goods& g = Goods_all[i];

    int target = robot[robot_id].target_berth;
    int dis = Goods_all[i].goods_berth_dis[target] + Goods_all[i].goods_berth_dis[Goods_all[i].min_berth];
    if(id + berth_a[g.min_berth].transport_time + dis > 14999)
    {
        int min_dis = 100000, min_id = -1;
        for(int i = 0; i < 10; i++)
        {
            dis = Goods_all[i].goods_berth_dis[target] + Goods_all[i].goods_berth_dis[i];
            if(id + berth_a[i].transport_time + dis <= 14999)
            {
                if(dis < min_dis)
                {
                    min_dis = dis;
                    min_id = i;
                }
            }
        }
        g.min_berth = min_id;
    }
    if(id > max_transport_time)
    {
        int min_dis = 1000000, max_value = -1, min_id = -1;
        for(int i = 0; i < 10; i++)
        {
            if(berth_a[i].boat_num.size() != 0 && berth_a[i].esti_goods_num == 0 && g.berth_path[i].size() != 0)
            {
                if(g.goods_berth_dis[i] + g.goods_berth_dis[target] < min_dis)
                 //or g.berth_a[i].esti_goods_value > min_value ？？？
                 //or berth_a[i].transport_time ???
                {
                    min_dis = g.goods_berth_dis[i];
                    min_id = i;
                }
                // if(berth_a[i].pre_value > max_value)
                // {
                //     max_value = berth_a[i].pre_value;
                //     min_id = i;
                // }
                // if(berth_a[i].transport_time < min_dis)
                // {
                //     max_value = berth_a[i].transport_time;
                //     min_id = i;
                // }
            }
        }
        g.min_berth = min_id;
        // else
        // g.goods_berth_dis[g.min_berth] = 0x00ffffff;
    }
}
int boat_sign[5];
void ShipCmd_end()
{
    int max_id;
    for(int i = 0; i < 5; i++)
    {
        update_esti_num();
        if(14999 - id < berth_a[boat[i].pos].transport_time + 2*max_berth_time +752 && 14999 - id > berth_a[boat[i].pos].transport_time + 2*max_berth_time+220)
        //if(boat_sign[i] == 0)
        {
            if(boat[i].status == 1 && boat[i].pos >=0 && boat[i].remain_capacity < boat_capacity)
            {
                //if(14999 - id < berth_a[boat[i].pos].transport_time + 2*max_berth_time + 402 && 14999 - id > berth_a[boat[i].pos].transport_time + 2*max_berth_time - 100)
                //{
                    int get_goods = Get_Goods(i);
                    if(get_goods < berth_a[boat[i].pos].loading_speed)
                    Go(i);
                    continue;
                //}
            }
        }
        if(boat[i].status == 0)continue;
        if(boat[i].status == 1 && boat[i].pos >=0)
        {//如果处于正常运行状态
            int get_goods = Get_Goods(i);
            if(boat[i].remain_capacity == 0 || 14999 - id <= berth_a[boat[i].pos].transport_time)
            {
                Go(i);
                continue;
            }
            if(get_goods < berth_a[boat[i].pos].loading_speed)
            {
                max_id = find_most_berth(i, 0);
                if(max_id == -1)continue;
                    Transfer(i, max_id);
            }
        }
        else if(boat[i].status == 1 && boat[i].pos == -1)
        {
            max_id = find_most_berth(i, -1);
            if(max_id == -1)continue;
            berth_a[max_id].boat_num.push_back(i);
            printf("ship %d %d\n", i, max_id);
        }
        else if(boat[i].status == 2)
        {
            //printf("ship %d %d\n", i, boat[i].pos);
        }
    }
}

bool check_direction(int x, int y)//检查是不是单通道，如果是，返回true
{
    int load_dir_sum = 0;
    int dir = -1;
    for(int i = 0; i < 4; i++)
    {
        int x_d, y_d;
        x_d = x + direction[i][0];
        y_d = y + direction[i][1];
        if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A'))
        {
            load_dir_sum++;
        }
    }
    if(load_dir_sum == 2)
    {
        return true;
    }
    else return false;
}

bool check_normal(int i, int x, int y, int x1, int y1)
{
    int next_step;
    for(int j = 0; j < 10; j++)
    {
        if(j == i)continue;
        if(robot[j].x == x1 && robot[j].y == y1)
        {
            if(robot[j].move_order.size() > 0)
            {
                next_step = robot[j].move_order.back();
                int x_d = robot[j].x + direction[next_step][0];
                int y_d = robot[j].y + direction[next_step][1];
                if(x_d != x || y_d != y)return true;
            }
        }
    }
    return false;
}

bool check_robot_narrow(int narrow_id, int robot_id)
{
    for(int i = 0; i < 10; i++)
    {
        if(i == robot_id)continue;
        if(nar[robot[i].x][robot[i].y] == narrow_id)
        {
            return false;
        }
    }
    return true;
}

bool checkPoint(int x,int y)
{
    if(ch[x][y] == '.' || ch[x][y]=='A' || ch[x][y]=='B') return true;
    return false;
}

//返回一个点可走的方向，0为左，1为上，2为右，3为下
vector<int> pointDirection(int i,int j)
{
    vector<int> dir;
    //左边
    if(i-1>=0 && checkPoint(i-1,j)) dir.push_back(0);
    //右边
    if(i+1 < 200 && checkPoint(i+1,j)) dir.push_back(2);
    //上面
    if(j+1 < 200 && checkPoint(i,j+1)) dir.push_back(1);
    //下面
    if(j-1 >= 0 && checkPoint(i,j-1)) dir.push_back(3);
    return dir;
}

void dfs(int x,int y,int vis[N][N],int num)
{
    vector<int> v = pointDirection(x,y);
    if(v.size() > 2 || vis[x][y] || !checkPoint(x,y)) return;
    vis[x][y] = 1;
    int dirs[] = {-1,0,1,0,-1};
    nar[x][y] = num;
    for(int i=0;i<v.size();i++)
    {
        dfs(x+dirs[v[i]],y+dirs[v[i]+1],vis,num);
    }
}

void InitNarrow()
{
    static int vis[N][N];
    memset(vis,0,sizeof(vis));
    int num = 0;
    int dirs[] = {-1,0,1,0,-1};
    if(id==9)
    for(int i=0;i<100;i++)
    {
        for(int j=0;j<200;j++)
        {
            if(!checkPoint(i,j) || vis[i][j]) continue;
            vector<int> dir = pointDirection(i,j); 
            if(dir.size() ==2)
            {//单向通道
            ofs<<"find one && num = "<< num <<endl;
                dfs(i,j,vis,num);
                num++;
            }
        }
        
    }
    if(id==10)
    for(int i=100;i<200;i++)
    {
        for(int j=0;j<200;j++)
        {
            if(!checkPoint(i,j) || vis[i][j]) continue;
            vector<int> dir = pointDirection(i,j); 
            if(dir.size() ==2)
            {//单向通道
            ofs<<"find one && num = "<< num <<endl;
                dfs(i,j,vis,num);
                num++;
            }
        }
        
    }
}