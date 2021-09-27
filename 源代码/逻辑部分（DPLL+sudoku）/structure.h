#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define True 1
#define False -1
#define Unset 0
#define SafeDelete(_x)   \
    {                    \
        if (_x)          \
        {                \
            delete (_x); \
            (_x) = NULL; \
        }                \
    }
#define SafeDeleteArray(_x) \
    {                       \
        if (_x)             \
        {                   \
            delete[](_x);   \
            (_x) = NULL;    \
        }                   \
    }
#define max(a, b) ((a) > (b) ? (a) : (b))
class lit;
class Solver;
class Clause;

/**************************
 * lit为变元，区分正负
 * ***************************/
class lit
{
public:
    int x;
    lit() {this->x=0;}
    lit(int x)
    {
        this->x = (x > 0) ? (2 * x + 1) : (-2 * x);
    }
    bool sign()
    { //return if the literal is signed
        return (this->x) & 1;
    }
    int var()
    { //return the underlying variable of the literal
        return (this->x) >> 1;
    }
    int index()
    { //convert the literal to a "small" integer suitable for array indexing
        return this->x;
    }
    int no()
    { //return the index of literal with opposite value '非'
        return this->x ^ 1;
    }
};
/**************************
 * 自定义的vector，用于存储可变数组
 * ***************************/
template <class T>
class mVector{
    friend lit;
public:
    //默认构造函数
    mVector(){
        this->length=0;
        this->maxlength=10;
        this->data=new T[this->maxlength]();

    };
    mVector(const mVector& v){
        this->length=v.length;
        this->maxlength=v.maxlength;
        this->data=new T[this->maxlength]();
        for (int i=0;i<this->maxlength;i++)
        {
            this->data[i]=v.data[i];
        }
    };
    ~mVector(){
        this->length=0;
        this->maxlength=0;
        SafeDeleteArray(this->data);
    };
    //插入一个元素到最后
    void push_back(T element){
        if (this->length>=this->maxlength)
        {
            int i;
            T* dataTemp=new T[this->maxlength*2]();
            for (i=0;i<this->maxlength;i++)
            {
                dataTemp[i]=this->data[i];
            }
            this->maxlength=this->maxlength*2;
            SafeDeleteArray(this->data);
            this->data=dataTemp;
        }
        this->data[this->length]=element;
        this->length++;
    };
    //清空vector
    void clear(){
        SafeDeleteArray(this->data);
        this->length=0;
        this->maxlength=10;
        this->data=new T[this->maxlength]();

    };
    void pop_back(){
        this->length--;
    }

    //获得vector元素个数
    int size(){
        return this->length;
    };
    //重载[]操作符
    T& operator[](int i){
        return this->data[i];
    };
    //重载=操作符
    void resize(int i){
        this->length=i;
    }
    mVector& operator=(const mVector & v){
        this->length=v.length;
        this->maxlength=v.maxlength;
        SafeDeleteArray(this->data);
        this->data=new T[this->maxlength]();
        for (int i=0;i<this->maxlength;i++)
        {
            this->data[i]=v.data[i];
        }
        return *this;
    };
private:
    T* data;    //存储数据的数组
    int length;   //数组元素
    int maxlength;
};


/**************************
 * 自定义数据结构mQueue，用作队列
 * ***************************/
template <class T>
class mQueue
{
public:
    mQueue() : Front(NULL), rear(NULL), count(0)
    {
    }

    ~mQueue()
    {
        clear();
    }

    void push(const T& node)
    {
        if (Front == NULL)
            Front = rear = new QueueNode(node);
        else
        {
            QueueNode* newqueuenode = new QueueNode(node);
            rear->next = newqueuenode;
            rear = newqueuenode;
        }
        count++;
    }

    void push_back(const T& node)
    {
        if (Front == NULL)
            Front = rear = new QueueNode(node);
        else
        {
            QueueNode* newqueuenode = new QueueNode(node);
            rear->next = newqueuenode;
            rear = newqueuenode;
        }
        count++;
    }
    bool empty() const
    {
        return Front == NULL;
    }

    int size() const
    {
        return count;
    }

    void clear()
    {
        while (Front)
        {
            QueueNode* FrontofQueue = Front;
            Front = Front->next;
            delete FrontofQueue;
        }
        count = 0;
    }

    void pop()
    {
        QueueNode* FrontofQueue = Front;
        Front = Front->next;
        delete FrontofQueue;
        count--;
    }

    void pop_back()
    {
        if(count==1){
            delete Front;
            count--;
        }else{
            QueueNode* t=Front;
            while(t->next!=rear){
                t=t->next;
            }
            QueueNode* rearofQueue=rear;
            t->next=NULL;
            rear=t;
            delete rearofQueue;
            count--;
        }
    }

    T& front()
    {

        return Front->data;
    }

    T front() const
    {

        return Front->data;
    }

    T Rear() const
    {
        return rear->data;
    }

    T* tolist()
    {
        T* array=new T[count];
        QueueNode* p=Front;
        for(int i = 0;i<count;i++){
            array[i]=p->data;
            p=p->next;
        }
        return array;
    }

//private: //也可以直接用来链表list直接构造
    struct QueueNode
    {
        T data;
        QueueNode* next;
        QueueNode(const T& Newdata, QueueNode* nextnode = NULL) : data(Newdata), next(nextnode)
        {
        }
        // QueueNode() = default;
    };
    QueueNode* Front; //队头指针
    QueueNode* rear;  // 队尾指针
    int count;
};

/**************************
 * 将decay参数合成结构体，便于传参
 * ***************************/
struct SearchParams
{
    double var_decay, clause_decay, rand_feq;
    SearchParams(double v = 1, double c = 1, double r = 0)
    {
        var_decay = v;
        clause_decay = c;
        rand_feq = r;
    }
};



/**************************
 * clause语句的定义
 * ***************************/
class Clause
{
public:
    bool learnt = 0;
    double activity = 0;
    mVector<lit> lits;
    bool operator<(const Clause x)
    {
        return activity < x.activity;
    }
    void calcReason(Solver& S, lit p, mVector<lit>& out_reason);
    bool Clause_new(Solver& S, mVector<lit> ps, bool learnt, Clause*& out_clause);
    bool locked(Solver S);
    bool simplify(Solver& S);
    bool propagate(Solver& S, lit p);

};
