#include <bits/stdc++.h>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;
const double weight = 0.5;
const int aim_berth = 9;

// ofstream ofs;

struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;   //机器人目标点
    int target_berth;
    bool on_road; //机器人是否正在前往的路上
    bool stuck; //机器人是否被困住
    vector<int> move_order;//机器人的运行指令
    Robot() {
        stuck = 0;
        on_road = false;
        target_berth = -1;
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
    vector<int > robot;
    priority_queue<Goods> goods_queue;
    Berth(){goods_num = 0;}
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
// int mp[N][N];
int direction[4][2] = {0, 1, 0, -1, -1, 0, 1, 0};
int grid_x[] = {1,1,2,2}, grid_y[] = {1,2,1,2};
int berth_sign[5];
vector<Berth> berth;

void Init()
{
    for(int i = 0; i < n; i ++)
        scanf("%s", ch[i]);
    // for(int i = 0; i < n; i ++)
    //     for(int j = 0; j < n; j++)
    //         if(ch[i][j] == '.' || ch[i][j] == 'B' || ch[i][j] == 'A')
    //             mp[i][j] = 1;
    for(int i = 0; i < berth_num; i ++)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth_a[id].x, &berth_a[id].y, &berth_a[id].transport_time, &berth_a[id].loading_speed);
    }
    scanf("%d", &boat_capacity);
    for(int i = 0;i<5;i++)
    boat[i].remain_capacity = boat_capacity;
    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}
int GoodsValForBerth(Goods &g,Berth &b);
void AimToBerth(Goods goods);
//将货物加入最好的泊口的货物队列中
void AimToBerth(Goods goods)
{
    int m_val = 5000;int m_id = -1, no_goods = 0;
    for(int i=0;i<5;i++)
    {
        int gbv = GoodsValForBerth(goods,berth[i]);
        // if(berth[i].goods_queue.size() == 0)
        // {
        //     if(no_goods == 0)
        //     {
        //         m_val = gbv;
        //         m_id = i;
        //         no_goods = 1;
        //     }
        //     else
        //     {
        //         if( gbv < m_val)
        //         {
        //             m_val = gbv;
        //             m_id = i;
        //         } 
        //     }
        // }
        // else if( gbv < m_val && no_goods == 0)
        // {
        //     m_val = gbv;
        //     m_id = i;
        // }
        if(berth[i].goods_queue.size() == 0)
        {
            m_val = gbv;
            m_id = i;
            break;
        }
        if( gbv < m_val)
        {
            m_val = gbv;
            m_id = i;
        }
    }
    // ofs<<"maxValue = "<<m_val << "maxID = " << m_id << endl;
    if(m_id != -1)
    berth[m_id].goods_queue.push(goods);
}

int Input()
{
    scanf("%d%d", &id, &money);
    int num;
    scanf("%d", &num);
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        
        AimToBerth(Goods(id, x, y, val));
        gds[x][y] = 1;
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


vector<int > find_road(int x1, int x2, int x3, int x4);//计算两点之间距离
int heuristic(int x, int y, int x2, int y2);
void RobotCmd();
void Move(int robot_id);
void ShipCmd();
void ChooseGoods(int robot_id);
void Del_Goods();
void Test();
void find_goods(int robot_id);
int Get_Goods_id(int x, int y);
void Robot_Init(int id);
void Wall_Check(int i, int mp[N][N]);
void ChooseBerth();
void Berth_Init(int id);
void mp_Init(int mp[N][N]);

vector<int > find_road(int x1, int y1, int x2, int y2, int mp[N][N]);

int main()
{
    // ofs.open("goodsberth.txt",ios::out);
    Init();
    ChooseBerth();
    for(int zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
        if(id <= 10)
        {
            Berth_Init(id-1);
        }
        else if(id <= 20 && id >10)
        {
            Robot_Init(id-11);
        }
        else
        {
            //CheckCollision();
            //Del_Goods();
            RobotCmd();
            ShipCmd();
            // Test();
        }
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
/*
void Test()
{
    Robot& rob = robot[0];
    Berth& ber = berth[0];
    if(rob.x == ber.x && rob.y == ber.y)
        return;
    if(rob.move_order.size() > 0)
    {
        Move(0);
    }
    else
    {
        rob.move_order = find_road(rob.x,rob.y,ber.x,ber.y);
        ofstream ofs;
        ofs.open("path.txt",ios::out);
        ofs<<rob.x <<" "<<rob.y<<" "<<ber.x<<" "<<ber.y<< " "<< find_road(rob.x,rob.y,ber.x,ber.y).size() <<endl;
        for(int i=rob.move_order.size()-1;i>=0;i--)
        {
            ofs<<rob.move_order[i]<<" ";
        }
        ofs.close();
    }
    

}
*/
//一帧中机器人的指令
void RobotCmd()
{
    int mp[N][N] = {0};
    mp_Init(mp);
    for(int i = 0;i<10;i++)
    {   
        // if(!robot[i].status)
        // {
        //     robot[i].move_order.push_back(-1);
        // }
        if(!robot[i].status)
        {
            robot[i].move_order.clear();
        }
        if(!robot[i].status || robot[i].stuck) 
            continue;
        if(robot[i].on_road)
        {
            //如果还没走完，按之前计算出的路径继续走
            int x_d = berth[robot[i].target_berth].x + grid_x[i % 4];
            int y_d = berth[robot[i].target_berth].y + grid_y[i % 4];
            if(robot[i].move_order.size())
            {
                Wall_Check(i, mp);
                // CheckCollision(i);
                Move(i);
            }
            //如果已经到达了目的地
            else
            {
                //如果到达了货物，就进行泊口路径规划
                //如果当前坐标有货物
                if(gds[robot[i].x][robot[i].y])
                {
                    printf("get %d\n",i);
                    gds[robot[i].x][robot[i].y] = 0;
                    // robot[i].move_order = find_road(robot[i].x,robot[i].y,berth[0].x,berth[0].y);
                    robot[i].move_order = find_road(robot[i].x, robot[i].y, x_d, y_d);
                    if(robot[i].move_order.size() == 0)robot[i].stuck = 1;
                    robot[i].mbx = x_d;robot[i].mby = y_d;
                    // CheckCollision(i);
                    Move(i);
                    continue;
                }

                if(robot[i].goods && ch[robot[i].x][robot[i].y] == 'B')
                {
                    //否则到达了泊口，重新进行货物路径规划
                    printf("pull %d\n",i);
                    berth[robot[i].target_berth].goods_num ++;
                    ChooseGoods(i);
                    // CheckCollision(i);
                    Move(i);
                    continue;
                }
                if(robot[i].goods)
                {
                    //如果机器人手上有货物但是没有路径
                    robot[i].move_order = find_road(robot[i].x, robot[i].y, x_d, y_d);
                    robot[i].mbx = x_d;robot[i].mby = y_d;
                    //CheckCollision(i);
                    Move(i);
                }
                else
                {
                    //如果机器人没货没路径
                    ChooseGoods(i);
                    //CheckCollision(i);
                    Move(i);
                }

            }
                
        }

        else
        {
            ChooseGoods(i);
            //CheckCollision(i);
            Move(i);
        }
        
    }
}

//机器人按照路径移动
void Move(int robot_id)
{
    if(robot[robot_id].move_order.empty()) 
        return; 
    int dir = robot[robot_id].move_order.back();
    if(dir == -1){robot[robot_id].move_order.pop_back();return;}
    robot[robot_id].move_order.pop_back();
    printf("move %d %d\n",robot_id,dir);
}

void ChooseGoods(int robot_id)
{
    int x = robot[robot_id].x, y = robot[robot_id].y;
    int ber = robot[robot_id].target_berth;
    
    while( ber >=0 && ber <5 && berth[ber].goods_queue.size())
    {
        // 从头开始遍历队列
        Goods g = berth[ber].goods_queue.top();
        vector<int >path = find_road(x,y,g.x,g.y);//货物路径
        if(id - g.id + path.size() < 1000 && path.size() > 0)
        {
            robot[robot_id].mbx = g.x;robot[robot_id].mby = g.y;
            robot[robot_id].move_order = path;
            berth[ber].goods_queue.pop();
            robot[robot_id].on_road = true;
            return;
        }
        gds[g.x][g.y] = 0;
        // ofs<< "berth_id  = "<<ber<<"    queue.size = "<<berth[ber].goods_queue.size() << endl;
        berth[ber].goods_queue.pop();
    }

}

// void Del_Goods()
// {
//     // for(int i = ((int)(goods.size()-1) > 11)?11:(goods.size()-1);i>=0;i--){
//     //     if(id - goods[i].id > 1000)
//     //     {
//     //         gds[goods[i].x][goods[i].y] = 0;
//     //         goods.erase(goods.begin()+i);
//     //     }
//     // }
//     for(int i=0;i<5;i++)
//     {
//         while(berth[i].goods_queue.size() > 0)
//         {
//             Goods g = berth[i].goods_queue.front();
//             if(id - g.id > 1000)
//             {
//                 gds[g.x][g.y] = 0;
//                 berth[i].goods_queue.pop();
//             }
//             else break;
//         }
//     }
// }

void ShipCmd()
{
    for(int i = 0;i<5;i++){
        if(boat[i].status == 1 && boat[i].pos >=0){//如果处于正常运行状态
            int get_goods = min(boat[i].remain_capacity, min(berth[i].goods_num, berth[i].loading_speed));
            boat[i].remain_capacity -= get_goods;//取货
            berth[i].goods_num -= get_goods;
            if(boat[i].remain_capacity == 0 || 14990 - id == berth[i].transport_time || 14990 - id == 200 + 3*berth[i].transport_time)//判断容量是否为0
            {
                printf("go %d\n", i);
                boat[i].remain_capacity = boat_capacity;
            }
        }
        else if(boat[i].status == 2){//等待状态
            printf("ship %d %d\n", i, berth_sign[i]);
        }
        else if(boat[i].status == 1 && boat[i].pos == -1){
            printf("ship %d %d\n", i, berth_sign[i]);
        }
        //else printf("ship %d %d\n", i, i);
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

        for(int i = 0;i<4;i++){
            x_d = x+direction[i][0];y_d = y+direction[i][1];
            if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A') && sign[x_d][y_d][0] == -1){
                sign[x_d][y_d][0] = sign[x][y][0] + 1;
                sign[x_d][y_d][1] = i;
                priority.push(Point(x_d, y_d, sign[x_d][y_d][0] + heuristic(x_d, y_d, x2, y2)));
            }
            if(x_d == x2 && y_d == y2){ //是否到达终点
                int step = sign[x2][y2][1];
                x = x_d;y = y_d;
                while(step != -1){
                    ans.push_back(step);
                    x -= direction[step][0];y -= direction[step][1];
                    step = sign[x][y][1];
                }
                //reverse(ans.begin(), ans.end());
                return ans;
            }
        }
    }
    return ans;
}

vector<int > find_road(int x1, int y1, int x2, int y2, int mp[N][N])//计算两点之间距离
{
    Point c_point;
    priority_queue<Point > priority;
    vector<int > ans;
    if(x1 == x2 && y1 == y2)return ans;
    //if(num>10)return ans;
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

        for(int i = 0;i<4;i++){
            x_d = x+direction[i][0];y_d = y+direction[i][1];
            if((ch[x_d][y_d] == '.' || ch[x_d][y_d] == 'B' || ch[x_d][y_d] == 'A') && mp[x_d][y_d] == 0 && sign[x_d][y_d][0] == -1){
                sign[x_d][y_d][0] = sign[x][y][0] + 1;
                sign[x_d][y_d][1] = i;
                priority.push(Point(x_d, y_d, sign[x_d][y_d][0] + heuristic(x_d, y_d, x2, y2)));
            }
            if(x_d == x2 && y_d == y2){ //是否到达终点
                int step = sign[x2][y2][1];
                x = x_d;y = y_d;
                while(step != -1){
                    ans.push_back(step);
                    x -= direction[step][0];y -= direction[step][1];
                    step = sign[x][y][1];
                }
                //reverse(ans.begin(), ans.end());
                return ans;
            }
        }
    }
    return ans;
}


// int Get_Goods_id(int x, int y)//获取指定坐标货物在向量中的位置
// {
//     for(int i = 0;i<goods.size();i++)
//     {
//         if(goods[i].x == x && goods[i].y == y)
//         {
//             return goods[i].id;
//         }
//     }
//     return 0;
// }
int heuristic(int x, int y, int x2, int y2){//代价估计函数
    return abs(x2-x) + abs(y2-y);
}

void Berth_Init(int p_id)//泊口分配机器人
{
    for(int i = 0;i < 5; i++)
    {
        if(berth[i].robot.size() >= 2)continue;
        if(find_road(robot[p_id].x, robot[p_id].y, berth[i].x, berth[i].y).size())
        {
            robot[p_id].target_berth = i;
            berth[i].robot.push_back(p_id);
            return;
        }
    }
    for(int i = 0;i < 5; i++)
    {
        if(find_road(robot[p_id].x, robot[p_id].y, berth[i].x, berth[i].y).size())
        {
            robot[p_id].target_berth = i;
            berth[i].robot.push_back(p_id);
            return;
        }
    }
}
void Robot_Init(int p_id)//机器人分批启动
{
    ChooseGoods(p_id);
    //CheckCollision(p_id);
    Move(p_id);
}

void Wall_Check(int i, int mp[N][N])//撞墙监测
{
    if(robot[i].move_order.size() == 0)return;

    int next_step;
    int x, y, x_d, y_d;
    next_step = robot[i].move_order.back();
    x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
    if((ch[x][y] != '.' && ch[x][y] != 'B' && ch[x][y] != 'A'))//如果下一步是墙
    {
        mp[robot[i].x][robot[i].y]--;
        mp[x][y]--;
        robot[i].move_order = find_road(robot[i].x, robot[i].y, robot[i].mbx, robot[i].mby);
        mp[robot[i].x][robot[i].y]++;
        next_step = -1;
        if(robot[i].move_order.size() != 0)
        next_step = robot[i].move_order.back();
        if(next_step != -1){
        x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
        mp[x][y]++;
        }
    }
    else if(mp[x][y] >= 2)
    {
        
        mp[robot[i].x][robot[i].y]--;
        mp[x][y]--;
        robot[i].move_order = find_road(robot[i].x, robot[i].y, robot[i].mbx, robot[i].mby, mp);
        mp[robot[i].x][robot[i].y]++;
        next_step = -1;
        if(robot[i].move_order.size() != 0)
        next_step = robot[i].move_order.back();
        if(next_step != -1){
        x = robot[i].x + direction[next_step][0], y = robot[i].y + direction[next_step][1];
        mp[x][y]++;
        }
    }
    return;
}
void mp_Init(int mp[N][N])
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
            if(next_step != -1)
            {
                x = robot[j].x + direction[next_step][0]; y = robot[j].y + direction[next_step][1];
                mp[x][y]++;
            }
        }

    }
}

void ChooseBerth()
{
    int speed[10];
    for(int i = 0; i < 10; i++)
    {
        int num = 0;
        for(int j = 0; j < 10; j++)
        {
            if(berth_a[i].transport_time <= berth_a[j].transport_time)
            {
                num++;
            }
        }
        if(num >= 5 && berth.size() < 5)
        {
            berth_sign[berth.size()] = i;
            berth.push_back(berth_a[i]);
        }
    }
}
//判断点是否可行
bool CheckPoint(Point p)
{
    if(p.x <0 || p.x >=N || p.y < 0 || p.y >= N || ch[p.x][p.y] != '.' || ch[p.x][p.y] != 'B' || ch[p.x][p.y] != 'A')
        return false;
    return true;
}
int dirX[] = {1,-1,0,0}, dirY[] = {0,0,1,-1};

int GoodsValForBerth(Goods &g,Berth &b)
{
    int deltaX = abs(g.x - b.x), deltaY = abs(g.y - b.y);
    g.weight_val = (double)g.val / (double)(deltaX + deltaY);
    return (deltaX + deltaY);
}

