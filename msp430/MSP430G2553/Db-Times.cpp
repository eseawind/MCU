/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:		2013/7/19
 *      Last    :       2013/7/19
 * 		Notes	:       µ¥Î»×ª»» db <---> Times
 * 		Tool    :       personal computer
 **********************************************/
#include <iostream>
#include <math.h>

using namespace std;

double Db2times(double db);     // DB    --->  times
double Times2Db(double times);  // times --->  Db

int main()
{
    double times = 0;

    while(1)
    {
        cout << "Input the value of times:" << endl;
        cin >> times;
        cout << "Control V: " << ((Times2Db(times) / (-40)) - 1) << endl;
    }
    return 0;
}

double Db2times(double db)
{
    return pow(10,db / 20);
}

double Times2Db(double times)
{
    return log10(times)*20;
}
