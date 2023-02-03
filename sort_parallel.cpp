#include <thread>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <vector>
#include <set>

using namespace std;


//here we write number of threads : 4 in homework
const int THREAD_COUNT = 4 ; 
thread thrs[THREAD_COUNT];


//queue tasks
queue< pair<int, int> > tasks;

//mutexs (for set & queue task)
mutex q_mutex, s_mutex;

//condition
condition_variable cv;

//set
set<int> ss;

//////////////////////////////////////////////////////

//function 3 required : swap 2 elements
template <typename t>
void swap_fun(t& x, t& y)
{
    t temp = x;
    x = y;
    y = temp;
    return;
}

//////////////////////////////////////////////////////


//function 2 required : partition algorithm
template <typename T>
int partition(vector<T>& arr, int l, int r)
{
    T tmp = arr[r];
    int i = l - 1;

    for (int j = l; j <= r - 1; j++)
        if (arr[j] < tmp)
        {
            i++;
            swap_fun(arr[i], arr[j]);
        }

    swap(arr[i + 1], arr[r]);
    i++;
    return i;
}


///////////////////////////////////////////////////


//function 5 required : Sorting function
template <typename T>
void quick_sort(vector<T>& arr)
{
    while (true)
    {
        unique_lock<mutex> u_lock(q_mutex);
        if (ss.size() == arr.size())
            return;

        if (tasks.size() > 0) //queue task not empty
        {
            pair<int, int> cur_task = tasks.front();
            tasks.pop();

            int l = cur_task.first, r = cur_task.second;

            if (l < r)
            {
                int q = partition(arr, l, r);

                //Add indexes in set
                s_mutex.lock();
                ss.insert(q);
                ss.insert(l);
                ss.insert(r);
                s_mutex.unlock();

                //push new tasks for left and right part
                tasks.push(make_pair(l, q - 1));
                tasks.push(make_pair(q + 1, r));

                cv.notify_one();
            }
        }
        else
            //if queue empty
            cv.wait(u_lock);
    }
}


///////////////////////////////////////////////////////


//check sorting if sucess
bool is_sorted(const vector<int>& arr)
{
    for (size_t i = 0; i < arr.size() - 1; i++)
        if (!(arr[i] <= arr[i + 1]))
            return false;
    return true;
}

//////////////////////////////////////////////////////


// function 5 required : print_arr array
template <typename T>
void print_arr(const vector<T>& arr)
{
    for (size_t i = 0; i < arr.size(); i++)
        cout << arr[i] << " ";
    cout << endl;
}



////////////////////////////////////////////////////////


// function 1 required : main function
int main()
{
    cout << "Parallel Version \n\n";
    int ARR_SIZE;
    cout << "enter number of element to sort : ";
    cin >> ARR_SIZE;
    cout << endl;
    vector<int> arr(ARR_SIZE);

    cout << "enter elements : ";
    for (int i = 0; i < ARR_SIZE; i++)
    {
        cin >> arr[i];
    }

    cout << "\nArray before sorting : "<< endl;
    print_arr(arr);
    cout << endl ;


    tasks.push(make_pair(0, arr.size() - 1));

    /////////////////////////////

    for (int i = 0; i < THREAD_COUNT; i++)
        thrs[i] = thread(quick_sort<int>, ref(arr));

    ////////////////////////////

    for (auto& th : thrs)
        th.join();

    cout << "Sorting finished!\n\n\n" ;

    cout << "Array after sorting : \n" ;
    print_arr(arr);

    cout << endl << endl;

    if (is_sorted(arr) == 0)
        cout << "sorting faild" << endl;
    else
        cout << "sorting sucess" << endl;
    system("pause");
    return 0;
}
