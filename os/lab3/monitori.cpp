#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <optional>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <sys/wait.h>
#include <random>
#include <bits/atomic_base.h>
#include <sys/random.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;


bool raceStart;
pthread_cond_t race_start;

typedef struct
{
    pthread_cond_t pit_free;

    pthread_cond_t car_arrived;

    pthread_cond_t lower_flag;

    pthread_cond_t mechanics_done;

    pthread_cond_t car_left;

    int pit_occupied;

    int mechanics_finished;

    int pit_cycle;

    int car_in_pit;

} Team;

typedef struct
{
    int team;
    int car;

} carData;

struct CarVisual {
    float progress;
    bool inPit;
    bool finished;

    int team;
    int car;
    int pitWait;
    bool celebrated;
    int place;
    float time;
};
vector<CarVisual> visualCars;


VertexArray track(sf::PrimitiveType::LineStrip, 5);


vector<Team> teams;
int carsDone;
int numOfCars;
std::atomic<bool> raceDone(false);
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
vector<pair<carData, float>> standings;
sf::RenderWindow window;
RenderWindow carRanking;
RenderWindow teamRanking;
int teamNum;
int carNum;
vector<Color> teamColors={Color::Red,Color::Blue,Color::Green,Color::Yellow,Color::Magenta};
const sf::Font font("arial.ttf");

float randomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> dist(min, max);

    return dist(gen);
}

int getMaxIndex(vector<int> v) {
    int max=-1;
    int index=-1;
    for (int i=0;i<v.size();i++) {
        if (v[i]>max) {
            max=v[i];
            index=i;
        }
    }
    return index;
}

int getMinIndex(vector<int> v) {
    int min = INT_MAX;
    int index = -1;

    for (int i = 0; i < v.size(); i++) {
        if (v[i] < min) {
            min = v[i];
            index = i;
        }
    }

    return index;
}

void* lollipopMan(void* cardata) {
    carData* data = (carData*)cardata;
    int team = data->team;
    while (!raceDone) {
        pthread_mutex_lock(&mx);
        while (teams[team].car_in_pit==-1 && !raceDone) {
            pthread_cond_wait(&teams[team].car_arrived,&mx);
        }
        if (raceDone) {
            pthread_mutex_unlock(&mx);
            delete data;
            return NULL;
        }
        teams[team].mechanics_finished=0;
        teams[team].pit_cycle++;
        pthread_cond_broadcast(&teams[team].lower_flag);

        while (teams[team].mechanics_finished<4 && !raceDone) {
            pthread_cond_wait(&teams[team].mechanics_done,&mx);
        }
        if (raceDone) {
            pthread_mutex_unlock(&mx);
            delete data;
            return NULL;
        }
        teams[team].car_in_pit=-1;
        teams[team].pit_occupied = 0;

        pthread_cond_broadcast(&teams[team].car_left);
        pthread_cond_signal(&teams[team].pit_free);
        pthread_mutex_unlock(&mx);
    }
    delete data;
    return NULL;
}

void* mehanicar(void* cardata) {
    carData* data = (carData*)cardata;
    int team = data->team;
    int last_cycle = 0;
    while (!raceDone) {
        pthread_mutex_lock(&mx);
        while (teams[team].pit_cycle == last_cycle && !raceDone) {
            pthread_cond_wait(&teams[team].lower_flag,&mx);
        }
        if (raceDone) {
            delete data;
            pthread_mutex_unlock(&mx);
            return NULL;
        }
        last_cycle = teams[team].pit_cycle;
        cout<<"Mechanic ["<<team<<"] started"<<endl;
        pthread_mutex_unlock(&mx);
        float mt=randomFloat(2,3);
        usleep((mt * 1000000));
        pthread_mutex_lock(&mx);
        teams[team].mechanics_finished++;
        cout<<"Mechanic ["<<team<<"] "<<teams[team].mechanics_finished<<" finished in "<<mt<<endl;
        if (teams[team].mechanics_finished == 4) {
            pthread_cond_signal(&teams[team].mechanics_done);
        }
        pthread_mutex_unlock(&mx);
    }
    delete data;
    return NULL;
}

int getMaxPit(int team) {
    int max=-1;
    for (int i=0;i<visualCars.size();i++) {
        if (visualCars[i].team==team) {
            if (visualCars[i].pitWait>max) {
                max=visualCars[i].pitWait;
            }
        }
    }
    return max;
}
void movePit(int team) {
    for (int i=0;i<visualCars.size();i++) {
        if (visualCars[i].team==team) {
            if (visualCars[i].pitWait>0) {
                visualCars[i].pitWait--;
            }
        }
    }
}
void driverPit(int team, int car) {
    pthread_mutex_lock(&mx);
    int id = team * carNum + car;
    visualCars[id].inPit=1;
    visualCars[id].pitWait=getMaxPit(team)+1;
    while (teams[team].pit_occupied) {
        cout<<"Car ["<<car<<"|"<<team<<"] waiting for pit"<<endl;
        pthread_cond_wait(&teams[team].pit_free,&mx);
    }
    teams[team].pit_occupied = 1;
    teams[team].car_in_pit = car;
    pthread_cond_broadcast(&teams[team].car_arrived);
    cout<<"Car ["<<car<<"|"<<team<<"] in pit"<<endl;

    while (teams[team].car_in_pit==car) {
        pthread_cond_wait(&teams[team].car_left, &mx);
    }
    movePit(team);
    cout<<"Car ["<<car<<"|"<<team<<"] left pit"<<endl;
    visualCars[id].inPit=0;
    pthread_mutex_unlock(&mx);
}

void driverDrive(int team, int car, float driveS) {
    int id = team * carNum + car;
    int steps=500;
    float sleepTime=driveS/steps;
    pthread_mutex_lock(&mx);
    cout<<"Car ["<<car<<"|"<<team<<"] diving for: "<<driveS<<"s"<<endl;
    pthread_mutex_unlock(&mx);
    for (int i=0;i<steps;i++) {
        pthread_mutex_lock(&mx);
        visualCars[id].progress+=((float)1/(float)steps);
        pthread_mutex_unlock(&mx);
        usleep((int)(sleepTime*1000000));
    }
    visualCars[id].progress=0;
}

void* vozac(void* cardata) {
    timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    carData* data = (carData*)cardata;
    int team = data->team;
    int car=data->car;
    float driveTime=randomFloat(2.f,5.f);
    float driveTime2=7-driveTime;
    float driveTime3=randomFloat(2,3);
    float totalTime=driveTime+driveTime2+driveTime3;
    pthread_mutex_lock(&mx);
    while (!raceStart)
        pthread_cond_wait(&race_start, &mx);
    pthread_mutex_unlock(&mx);

    driverDrive(team, car, driveTime);

    driverPit(team, car);

    driverDrive(team, car, driveTime2);

    driverPit(team, car);

    driverDrive(team, car, driveTime3);

    pthread_mutex_lock(&mx);
    ++carsDone;
    cout<<"Car ["<<car<<"|"<<team<<"] Finished "<<carsDone<<"/"<<numOfCars<<endl;
    clock_gettime(CLOCK_MONOTONIC, &end);

    float elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1000000000.0f;
    standings[carsDone - 1] = {{team, car}, elapsed};
    int id = team * carNum + car;
    visualCars[id].place=carsDone;
    visualCars[id].finished=1;
    visualCars[id].time=elapsed;
    if (carsDone==numOfCars) {
        raceDone=true;
        for (int i = 0; i < teams.size(); i++) {
            pthread_cond_broadcast(&teams[i].lower_flag);
            pthread_cond_broadcast(&teams[i].car_arrived);
            pthread_cond_broadcast(&teams[i].car_left);
            pthread_cond_broadcast(&teams[i].mechanics_done);
            pthread_cond_broadcast(&teams[i].pit_free);
        }
    }
    pthread_mutex_unlock(&mx);
    delete data;
    return NULL;
}
VertexArray getTrack(string track, Vector2f offset, float scale) {
    if (track=="monza") {
        Vector2f vertices[18]={{130,-75},{60,-75},{60,-73},{55,-75},{40,-75},{20,-65},{14,-45},{15,-20},{12,-20},{0,0},{20,5},{27,-15},{65,-55},{70,-52},{75,-55},{140,-55},{145,-60},{130,-75}};
        VertexArray ct(PrimitiveType::LineStrip,18);
        for (int i=0;i<18;i++) {
            vertices[i].y=vertices[i].y*(-1);
            ct[i].position=vertices[i]*scale+offset;
            ct[i].color=Color::Black;
        }
        return ct;
    }
    return VertexArray();
}

pair<int, float> getTrackPos(vector<float> lens, float part, float opseg) {
    float drv=opseg*part;
    int i=0;
    while (drv>0 && i < lens.size()) {
        if (drv>=lens[i]) {
            drv-=lens[i];
            i++;
        }else {
            float perc=drv/lens[i];
            return pair<int, float>(i,perc);
        }
    }
    return {lens.size()-1, 1.f};
}

void stvoriDretve(int t, int cars, string track) {
    teamNum=t;
    carNum=cars;
    window.create(VideoMode({900,600}),"window");
    window.setPosition({0,0});
    window.setFramerateLimit(60);
    vector<pthread_t> threads;
    numOfCars=t*cars;
    raceDone=0;
    teams.resize(t);
    standings.resize(t*cars);
    visualCars.resize(t * cars);


    int idx = 0;

    for (int i = 0; i < t; i++) {
        for (int j = 0; j < cars; j++) {

            visualCars[idx].progress = 0.f;
            visualCars[idx].inPit = false;
            visualCars[idx].finished = false;
            visualCars[idx].team = i;
            visualCars[idx].car = j;
            visualCars[idx].celebrated=false;
            visualCars[idx].pitWait=0;
            visualCars[idx].place=-1;
            visualCars[idx].time=0;
            idx++;
        }
    }
    for (int i=0;i<t;i++) {
        pthread_cond_init(&teams[i].pit_free, NULL);
        pthread_cond_init(&teams[i].car_arrived, NULL);
        pthread_cond_init(&teams[i].mechanics_done, NULL);
        pthread_cond_init(&teams[i].lower_flag, NULL);
        pthread_cond_init(&teams[i].car_left, NULL);
        teams[i].pit_occupied = 0;
        teams[i].mechanics_finished = 0;
        teams[i].pit_cycle = 0;
        teams[i].car_in_pit = -1;
    }
    for (int i=0;i<t;i++) {
        for (int j=0;j<cars;j++) {
            pthread_t tid;
            carData* data = new carData;
            data->team = i;
            data->car = j;
            pthread_create(&tid,NULL,vozac,data);
            threads.push_back(tid);
        }
    }
    for (int i=0;i<t;i++) {
        for (int j=0;j<4;j++) {
            pthread_t tid;
            carData* data = new carData;
            data->team = i;
            data->car = 0;
            pthread_create(&tid,NULL,mehanicar,data);
            threads.push_back(tid);
        }
    }
    for (int i=0;i<t;i++) {
        pthread_t tid;
        carData* data = new carData;
        data->team = i;
        data->car = 0;
        pthread_create(&tid,NULL,lollipopMan,data);
        threads.push_back(tid);
    }

    cout<<"utrka krece"<<endl;

    pthread_mutex_lock(&mx);
    raceStart = true;
    pthread_cond_broadcast(&race_start);
    pthread_mutex_unlock(&mx);
    vector<float> lineLen;
    VertexArray ct;
    float opseg=0;
    float cr=10;
    carRanking.create(VideoMode({150,(unsigned int)(cr*2*t*cars)}),"window");
    carRanking.setPosition({975,0});
    carRanking.setFramerateLimit(60);
    teamRanking.create(VideoMode({100,(unsigned int)30*t}),"window");
    teamRanking.setFramerateLimit(60);
    teamRanking.setPosition({975,(int)(cr*2*t*cars)+100});
    if (track=="monza") {
        if (track=="monza") {
            ct=getTrack("monza", {100,100},5);
            opseg=0;
            Vector2f len;
            lineLen.resize(17);
            for (int i=0;i<17;i++) {
                len=(ct[i+1].position-ct[i].position);
                lineLen[i]=sqrt(len.x*len.x+len.y*len.y);
                opseg+=lineLen[i];
            }
        }
    }
    while (window.isOpen()) {
        window.clear(Color::White);
        carRanking.clear(Color::White);
        teamRanking.clear(Color::White);
        if (raceDone) {

            vector<int> teamStandings;
            teamStandings.resize(t);
            for (int i=0;i<numOfCars;i++) {
                teamStandings[standings[i].first.team] += i+1;
            }
            for (int i=0;i<t;i++) {
                int best = getMinIndex(teamStandings);
                RectangleShape rs;
                Vector2f pos=teamRanking.getView().getCenter()-teamRanking.getView().getSize()/2.f;
                rs.setPosition({pos.x,pos.y+30*i});
                rs.setSize({100,30});
                rs.setFillColor(teamColors[best]);
                teamRanking.draw(rs);
                teamStandings[best] = INT_MAX;
            }
        }
        while (optional<Event> ev = window.pollEvent()) {
            if (ev->is<Event::Closed>()) {
                window.close();
                carRanking.close();
                teamRanking.close();
            }
        }
        while (optional<Event> ev = carRanking.pollEvent()) {
            if (ev->is<Event::Closed>()) {
                window.close();
                carRanking.close();
                teamRanking.close();
            }
        }
        while (optional<Event> ev = teamRanking.pollEvent()) {
            if (ev->is<Event::Closed>()) {
                window.close();
                carRanking.close();
                teamRanking.close();
            }
        }
        Vector2f offset=window.getView().getCenter();
            float radius=100;
        if (track=="circle"){
            offset.x-=radius;
            offset.y-=radius;
            CircleShape circ;
            circ.setOutlineThickness(2);
            circ.setOutlineColor(Color::Black);
            circ.setRadius(radius);
            circ.setPosition(offset);
            window.draw(circ);
        }else if (track=="monza"){
            window.draw(ct);

        }
        pthread_mutex_lock(&mx);
        for (auto &c : visualCars) {
            Vector2f place;
            if (track=="circle") {
                float angle;
                if (c.inPit==0) {
                    angle = c.progress * 2.f * 3.1415926f;
                    place.x=radius*cos(angle)+radius+offset.x;
                    place.y=radius*sin(angle)+radius+offset.y;
                }else {
                    angle=0;
                    place.x=radius*cos(angle)+radius+offset.x+cr*2*(c.team+1);
                    place.y=radius*sin(angle)+radius+offset.y-cr*2*c.pitWait+cr*2;
                }
            }else if (track=="monza") {
                if (c.inPit==0) {
                    pair<int, float> pf =getTrackPos(lineLen,c.progress,opseg);
                    int seg=pf.first;
                    float perc=pf.second;
                    float delx=ct[seg+1].position.x-ct[seg].position.x;
                    float dely=ct[seg+1].position.y-ct[seg].position.y;
                    place.x=ct[seg].position.x+delx*perc;
                    place.y=ct[seg].position.y+dely*perc;
                }else {
                    place.x=ct[0].position.x+cr*2*(c.team-(int)(t/2));
                    place.y=ct[0].position.y+cr*2*(c.pitWait);
                }
            }
            CircleShape car;
            car.setRadius(cr);
            car.setPosition({place.x-cr,place.y-cr});
            car.setFillColor(teamColors[c.team]);
            Text txt(font);
            txt.setCharacterSize(cr);
            txt.setString(to_string(c.car));
            txt.setPosition({place.x-cr/2,place.y-cr/2});
            txt.setFillColor(Color::Black);
            txt.setStyle(Text::Bold);
            if (!c.finished) {
                window.draw(car);
                window.draw(txt);
            }else {
                View v=carRanking.getView();
                place.x=v.getCenter().x-v.getSize().x/2;
                place.y=v.getCenter().y-v.getSize().y/2+c.place*2*cr;
                car.setPosition({place.x,place.y-cr*2});
                txt.setPosition({place.x+cr/2,place.y-cr*2+cr/2});
                carRanking.draw(car);
                carRanking.draw(txt);
                Text txt2(font);
                txt2.setCharacterSize(2*cr);
                txt2.setString(to_string(c.time)+"s");
                txt2.setPosition({place.x-cr/2+cr*4,place.y-cr*2});
                txt2.setFillColor(Color::Black);
                carRanking.draw(txt2);
            }
        }
        pthread_mutex_unlock(&mx);
        window.display();
        carRanking.display();
        teamRanking.display();
    }
    for (pthread_t tid : threads)
    {
        pthread_join(tid, NULL);
    }
    cout<<"race Finished"<<endl;
    cout<<"Rankings:"<<endl;
    for (int i=0;i<numOfCars;i++) {
        cout<<i+1<<". Car:"<<standings[i].first.car+1<<" Team: "<<standings[i].first.team+1<<" Time: "<<standings[i].second<<endl;
    }
    pthread_mutex_destroy(&mx);
}


int main() {
    pthread_cond_init(&race_start, NULL);
    cout<<"Teams: ";
    int t;
    cin>>t;
    cout<<"Cars: ";
    int c;
    cin>>c;
    cout<<"Track: ";
    string track;
    cin>>track;
    stvoriDretve(t,c,track);
}