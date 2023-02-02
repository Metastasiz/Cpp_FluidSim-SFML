//
// Created by Mark on 14/04/2021.
//

#ifndef FLUIDSIM_FLUID_H
#define FLUIDSIM_FLUID_H

#endif //FLUIDSIM_FLUID_H
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#define WIDTH 24*8;
#define HEIGHT 18*8;
using std::cout;
using std::string;
using std::endl;
class Fluid {
public:
    const static int h = HEIGHT;
    const static int w = WIDTH;
    const static int outOfBound = 999999;
    double g = 10;
    double deg = M_PI/8;
    const double denMul = 1;
    bool spawn = false;
    int chance = 100;
    int three = 3;
    double vy[h][w], vx[h][w], vyn[h][w], vxn[h][w], den[h][w], denn[h][w],
    inkR[h][w], inkG[h][w], inkB[h][w], inkRn[h][w], inkGn[h][w], inkBn[h][w];
    bool debug = false, once = false, autospawn = true;
    int debugcounter = 0;
    string out[h][w];

    void INIT(){
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                out[i][j] = " ";
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++){
                vy[i][j] = 0;
                vx[i][j] = 0;
                den[i][j] = outOfBound;
            }
        for (int i = 1; i < h-1; i++)
            for (int j = 1; j < w-1; j++){
                vy[i][j] = 0;
                vx[i][j] = 0;
                vxn[i][j] = 0;
                vyn[i][j] = 0;
                den[i][j] = 1000;
                denn[i][i] = 0;

            }
        setInk();
    }
    void setInk(){
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++){
                inkR[i][j] = 1000;
                inkG[i][j] = 1000;
                inkB[i][j] = 1000;
                inkRn[i][j]=0;
                inkGn[i][j]=0;
                inkBn[i][j]=0;
            }
    }
    void addInkN(int i, int j, double r, double g, double b){
        inkRn[i][j] += r;
        inkGn[i][j] += g;
        inkBn[i][j] += b;
    }
    void addInk(int i, int j, double r, double g, double b){
        inkR[i][j] += r;
        inkG[i][j] += g;
        inkB[i][j] += b;
    }
    void addFluid(int y, int x, double vey, double vex){
        vy[y][x] += vey;
        vx[y][x] += vex;
    }
    void runIns(){
        if(spawn) {
            srand(time(nullptr));
            int tmp = (int)(rand()%100);
            if (autospawn){
                int addh = (int) (rand()%(h - 20)) + 10;
                int addw = (int) (rand()%(w - 20)) + 10;
                double vey = ((fmod((double)rand()/1000,6)+1) - 3);
                double vex = ((fmod((double)rand()/1000,6)+1) - 3);
                while (abs(vey) == abs(vex)) {
                    vey = ((fmod((double)rand()/1000,6)+1) - 3);
                    vex = ((fmod((double)rand()/1000,6)+1) - 3);
                }
                addFluid(addh, addw, vey, vex);
            }
            else if (tmp <= chance) {
                int addh = (int) (rand()%(h - 20)) + 10;
                int addw = (int) (rand()%(w - 20)) + 10;
                double vey = ((fmod((double)rand()/1000,6)+1) - 3);
                double vex = ((fmod((double)rand()/1000,6)+1) - 3);
                addFluid(addh, addw, vey, vex);
            }
        }
        runVelo();
        runDen();
        applyNext();
        printTest();
    }
    static bool inBwn(double value, double top, double bot){ return value <= top && value >= bot; }
    static double conv(double rad){if (rad < 0) rad += M_PI*2; return rad;}

    void printTest(){
        if (debug){
            int tmp = 0;
            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    if (den[i][j] != inkR[i][j]){if(tmp < 1)debugcounter++;tmp++;}
                    cout << den[i][j] << "_" << denn[i][j] << " ";
                    //cout << inkR[i][j] << "_" << inkG[i][j] << "_" << inkB[i][j] << " ";
                }
                cout << endl;
            }
            cout << endl << endl;
        }
    }
    double getSurDen(int y, int x){
        double avg = 0;
        for (int i = -1; i < 1; i++)
            for (int j = -1; j < 1; j++){
                if (i == 0 && j == 0) continue;
                avg += den[y+i][x+j];
            }
        return avg/8;
    }
    void runVelo() {
        for (int i = 1; i < h - 1; i++)
            for (int j = 1; j < w - 1; j++) {
                //
                if (den[i][j] >= outOfBound) {
                    vx[i][j] = 0;
                    vy[i][j] = 0;
                    continue;
                }
                double tmpden = den[i][j];
                if (!(vy[i][j] == 0 && vx[i][j] == 0)) {
                    if (abs(vy[i][j]) + abs(vx[i][j]) < 0.00625) {
                        vy[i][j] = 0;
                        vx[i][j] = 0;
                        continue;
                    } else if (den[i][j] < getSurDen(i, j)) {
                        vy[i][j] /= 2;
                        vx[i][j] /= 2;
                        continue;
                    }
                    if      (i == 1)        {vy[i][j] *= -1;vx[i][j] *= -1;}
                    else if (i == h - 2)    {vy[i][j] *= -1;vx[i][j] *= -1;}
                    else if (j == 1)        {vy[i][j] *= -1;vx[i][j] *= -1;}
                    else if (j == w - 2)    {vy[i][j] *= -1;vx[i][j] *= -1;}
                    double rad = atan2(vy[i][j], vx[i][j]);
                    rad = conv(rad);
                    double mxrad = fmod(conv(rad + deg), (M_PI * 2)), mnrad = fmod(conv(rad - deg), (M_PI * 2));
                    int quarmn = -1, quarmx = -1;
                    bool pepe = false;
                    for (int k = 0; k < 8; k++) {
                        if (k == 0) {
                            if (inBwn(mxrad, deg, 0) || inBwn(mxrad, M_PI*2, M_PI*2-deg)) { quarmx = k;
                                pepe= true;
                            }
                            if (inBwn(mnrad, deg, 0) || inBwn(mnrad, M_PI*2, M_PI*2-deg)) { quarmn = k;
                                pepe= true;
                            }
                        } else {
                            if (inBwn(mxrad, (deg * k * 2) + deg, (deg * k * 2) - deg)) { quarmx = k; }
                            if (inBwn(mnrad, (deg * k * 2) + deg, (deg * k * 2) - deg)) { quarmn = k; }
                        }
                    }
                    if (false){
                        cout << quarmx << "_" << quarmn << " " << mxrad << "_" << mnrad << endl;
                    }
                    double magL = ((deg * (quarmx + 1) * 2) - fmod((mxrad+deg),(M_PI*2))) / (deg * 2);
                    double magU = 1 - magL;
                    //
                    int qMxY = -2, qMxX = -2;
                    switch (quarmx) {
                        case 0:qMxY = 0;qMxX = 1;break;
                        case 1:qMxY = -1;qMxX = 1;break;
                        case 2:qMxY = -1;qMxX = 0;break;
                        case 3:qMxY = -1;qMxX = -1;break;
                        case 4:qMxY = 0;qMxX = -1;break;
                        case 5:qMxY = 1;qMxX = -1;break;
                        case 6:qMxY = 1;qMxX = 0;break;
                        case 7:qMxY = 1;qMxX = 1;break;
                    }
                    if (!(den[i + qMxY][j + qMxX] >= outOfBound) && (qMxX != -2) && (qMxY != -2)) {
                        vyn[i + qMxY][j + qMxX] += magU * vy[i][j] / 2;
                        vxn[i + qMxY][j + qMxX] += magU * vx[i][j] / 2;
                        //
                        long m = mag(vy[i][j], vx[i][j]) * magU * den[i][j] / g;
                        //
                        long iR = mag(vy[i][j], vx[i][j]) * magU * inkR[i][j]/g;
                        long iG = mag(vy[i][j], vx[i][j]) * magU * inkG[i][j]/g;
                        long iB = mag(vy[i][j], vx[i][j]) * magU * inkG[i][j]/g;
                        addInkN(i + qMxY, j + qMxX, iR,iG,iB);
                        addInk(i, j, -iR,-iG,-iB);
                        //
                        denn[i + qMxY][j + qMxX] += m;
                        den[i][j] -= m;
                    }
                    if (once && (inkBn[i][j] != inkRn[i][j] || inkB[i][j] != inkR[i][j])){
                        cout << "1: " <<  " > " << i << " " << j << endl;
                        once = false;
                    }
                    //
                    int qMnY = -2, qMnX = -2;
                    switch (quarmn) {
                        case 0:qMnY = 0;qMnX = 1;break;
                        case 1:qMnY = -1;qMnX = 1;break;
                        case 2:qMnY = -1;qMnX = 0;break;
                        case 3:qMnY = -1;qMnX = -1;break;
                        case 4:qMnY = 0;qMnX = -1;break;
                        case 5:qMnY = 1;qMnX = -1;break;
                        case 6:qMnY = 1;qMnX = 0;break;
                        case 7:qMnY = 1;qMnX = 1;break;
                    }

                    if (!(den[i + qMnY][j + qMnX] >= outOfBound) && (qMnY != -2) && (qMnX != -2)) {
                        vyn[i + qMnY][j + qMnX] += magL * vy[i][j] / 2;
                        vxn[i + qMnY][j + qMnX] += magL * vx[i][j] / 2;
                        //
                        long m = mag(vy[i][j], vx[i][j]) * magL * den[i][j] / g;
                        //
                        long iR = mag(vy[i][j], vx[i][j]) * magL * inkR[i][j]/g;
                        long iG = mag(vy[i][j], vx[i][j]) * magL * inkG[i][j]/g;
                        long iB = mag(vy[i][j], vx[i][j]) * magL * inkB[i][j]/g;
                        addInkN(i + qMnY, j + qMnX, iR,iG,iB);
                        addInk(i, j, -iR,-iG,-iB);
                        //
                        denn[i+qMnY][j+qMnX] += m;
                        den[i][j] -= m;
                    }
                    if (den[i][j] >= outOfBound || denn[i][j] >= outOfBound){
                        cout << tmpden << "_" << den[i][j] << " " << quarmx << "_" << quarmn << " " << mxrad << "_" << mnrad << " " << magU << "_" << magL << endl;
                    }
                    if (once && (inkBn[i][j] != inkRn[i][j] || inkB[i][j] != inkR[i][j])){
                        cout << "2: " << den[i][j] << " " << inkR[i][j] << " > " << i << " " << j << endl;
                        once = false;
                    }
                        //unstable
                        vy[i][j] /= 2;
                        vx[i][j] /= 2;
                    }
                }

    }
    void runDen(){
        for (int i = 1; i < h-1; i++)
            for (int j = 1; j < w-1; j++){
                if (den[i][j]>=outOfBound)continue;
                for (int yAxis = -1; yAxis < 2; yAxis++){
                    for (int xAxis = -1; xAxis < 2; xAxis++){
                        if (yAxis == 0 && xAxis == 0 || den[i+yAxis][j+xAxis]>=outOfBound)continue;
                        double tmp;
                        if (den[i][j] > den[i+yAxis][j+xAxis]){
                            long m = ((den[i][j] - den[i+yAxis][j+xAxis])*g/(g*8*denMul))>1?((den[i][j] - den[i+yAxis][j+xAxis])*g/(g*8*denMul)):(den[i][j] - den[i+yAxis][j+xAxis])/2;
                            //
                            long iR = ((inkR[i][j] - inkR[i+yAxis][j+xAxis])*g/(g*8*denMul))>1?((inkR[i][j] - inkR[i+yAxis][j+xAxis])*g/(g*8*denMul)):(inkR[i][j] - inkR[i+yAxis][j+xAxis])/2;
                            long iG = ((inkG[i][j] - inkG[i+yAxis][j+xAxis])*g/(g*8*denMul))>1?((inkG[i][j] - inkG[i+yAxis][j+xAxis])*g/(g*8*denMul)):(inkG[i][j] - inkG[i+yAxis][j+xAxis])/2;
                            long iB = ((inkB[i][j] - inkB[i+yAxis][j+xAxis])*g/(g*8*denMul))>1?((inkB[i][j] - inkB[i+yAxis][j+xAxis])*g/(g*8*denMul)):(inkB[i][j] - inkB[i+yAxis][j+xAxis])/2;
                            addInk(i+yAxis,j+xAxis,iR,iG,iB);
                            addInkN(i,j,-iR,-iG,-iB);
                            //
                            den[i+yAxis][j+xAxis]+=m;
                            denn[i][j]-=m;

                            if (once && (inkBn[i][j] != inkRn[i][j] || inkB[i][j] != inkR[i][j])){
                                cout << "3: " << inkBn[i][j] << " " << inkRn[i][j] << " > "<< i << " " << j <<endl;
                                once = false;
                            }
                        }
                    }
                }
            }
    }
    void applyInk(int i, int j){
        inkR[i][j] += inkRn[i][j];
        inkG[i][j] += inkGn[i][j];
        inkB[i][j] += inkBn[i][j];
        inkRn[i][j] = 0;
        inkGn[i][j] = 0;
        inkBn[i][j] = 0;
    }
    void applyNext(){
        for (int i = 1; i < h-1; i++)
            for (int j = 1; j < w-1; j++) {
                double tmp1, tmp2;
                tmp1 = den[i][j];
                den[i][j]+=denn[i][j];
                denn[i][j]=0;
                applyInk(i,j);
                if (once && (denn[i][j] != inkRn[i][j] || den[i][j] != inkR[i][j])){
                    cout << "4: " << denn[i][j] << " " << inkRn[i][j] << " > "<< i << " " << j <<endl;
                    once = false;
                }
                tmp2 = denn[i][j];
                if (false && (tmp2 >= outOfBound || tmp1 >= outOfBound)){
                    cout << i << " " << j << ": " << inkR[i][j] << " " << inkRn[i][j] << endl;
                }
                vx[i][j]+=vxn[i][j];
                vxn[i][j]=0;
                vy[i][j]+=vyn[i][j];
                vyn[i][j]=0;
            }
    }
    double mag(double y, double x){
        return 1+sqrt(y*y+x*x);
    }
    void INIT_border(){
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                string t = out[i][j];
                if ((t == "╠") || (t == "╣") || (t == "╩") || (t == "╦"));
                else if (i == 0 && j == 0) out[i][j] = "╔";
                else if (i == 0 && j == w-1) out[i][j] = "╗";
                else if (i == h-1 && j == 0) out[i][j] = "╚";
                else if (i == h-1 && j == w-1) out[i][j] = "╝";
                else if (i == h-1 || i == 0) out[i][j] = "═";
                else if (j == w-1 || j == 0) out[i][j] = "║";
            }
        }
    }
    Fluid(){
        INIT();
        INIT_border();
    }
};
