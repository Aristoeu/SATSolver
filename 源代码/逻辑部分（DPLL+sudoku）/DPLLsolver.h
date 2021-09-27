#include "structure.h"
/**************************
 * solver of SAT using dpll
 * ***************************/
class Solver
{
    friend Clause;

public:
    /*****constraint database******/
    Clause **constrs; //list of problem constraints
    int constrs_size = 0;
    mQueue<Clause *> learnts; //list of learnt clauses
    double cla_inc = 1;       //clause activivty increment-amount to bump with
    double cla_decay = 1;     //decay factor for clause activity
    int literalNum;
    int clauseNum;

    /*****variable order******/
    double *activity;     //heuristic measurement of the activity of a variable
    double var_inc = 1;   //variable activivty increment-amount to bump with
    double var_decay = 1; //decay factor for variable activity

    /*****propagation******/
    mQueue<Clause *> *watches; //list of constraints watching each literal
    mQueue<lit> propQ;         //propagation queue

    /*****assignments******/
    char *assigns;         //current assignments indexed on variables
    mVector<lit> trail;     //list of assignments in chronological order
    mVector<int> trail_lim; //separator indices for different decision levels in 'trail'
    Clause **reason;       //constraint to imply each lit's value
    int *level;            //decision level each variable is assigned
    int root_level = 0;

    /*****result******/
    int *model;
    int model_size = 0;

    /*****helping methods******/
    int nVars() { return this->literalNum; }
    int nAssigns() { return trail.size(); }
    int nConstraints() { return this->clauseNum; }
    int nLearnts() { return learnts.size(); }
    int decisionLevel() { return trail_lim.size(); }

    Clause* propagate()
    {
        while (!propQ.empty())
        {
            lit p = propQ.front();
            propQ.pop();
            mQueue<Clause*> tmp;
            while(!watches[p.x].empty())
            {
                Clause *t=watches[p.x].front();
                tmp.push(t);
                watches[p.x].pop();
            }
            //tmp = watches[p.x];
            //(*watches[p.x]).clear();
            while(!tmp.empty()){
                Clause* c=tmp.front();
                tmp.pop();
                if((*c).propagate(*this,p)==0)
                {
                    while(!tmp.empty()){
                        Clause *cl=tmp.front();
                        tmp.pop();
                        watches[p.x].push(cl);
                    }
                    while(!propQ.empty())
                    propQ.pop();
                    return c;
                }
            }
        }
        return nullptr;
    }

    bool enqueue(lit p, Clause* from)
    {
        if (this->assigns[p.var()] != Unset)
        {
            int opt;
            if (p.sign() == 0)
                opt = False;
            else
                opt = True;
            if (this->assigns[p.var()] != opt)
                return 0; //conflicting enqueued assignment
            else
                return 1; //existing assignment-don't enqueue
        }
        else
        {
            int x = p.var();
            if (p.sign() == 0)
            {
                assigns[p.var()] = False;
            }
            else
            {
                assigns[p.var()] = True;
            }
            level[p.var()] = decisionLevel();
            reason[p.var()] = from;
            trail.push_back(p);
            propQ.push(p);
            return 1;
        }
    }

    void analyze(Clause* confl, mVector<lit>& out_learnt, int& out_btlevel)
    {
        int vars = nVars();
        bool* seen;
        seen = (bool*)malloc(vars * sizeof(bool*));
        for (int i = 0; i < nVars(); i++)
            seen[i] = false;
        int counter = 0;
        lit p;
        p.x = 0;
        mVector<lit> p_reason;
        out_learnt.push_back(p);
        out_btlevel = root_level;
        do
        {
            p_reason.clear();
            (*confl).calcReason(*this, p, p_reason);

            for (int j = 0; j < p_reason.size(); j++)
            {
                lit q = p_reason[j];
                if (!seen[q.var()])
                {
                    seen[q.var()] = 1;
                    if (level[q.var()] > root_level)
                    {
                        lit tmp;
                        tmp.x = (q.x) ^ 1;
                        varBumpActivity(q);
                    }
                    if (level[q.var()] == decisionLevel())
                        counter++;
                    else if (level[q.var()] > root_level)
                    {
                        lit tmp;
                        tmp.x = (q.x) ^ 1;
                        out_learnt.push_back(tmp);
                        out_btlevel = (out_btlevel > level[q.var()]) ? out_btlevel : level[q.var()];
                    }
                }
            }

            do
            {
                p = trail[trail.size() - 1];
                confl = reason[p.var()];
                undoOne();
            } while (!seen[p.var()]);
            counter--;
        } while (counter > 0);
        lit tmp;
        tmp.x = p.no();
        out_learnt[0] = tmp;
    }

    void record(mVector<lit> clause)
    {
        Clause* c = new Clause;
        Clause* learnt = nullptr;
        int tmp = (*c).Clause_new(*this, clause, 1, learnt);
        if (learnt == nullptr)
            return;
        enqueue(clause[0], learnt);
        if (learnt != NULL)
        {
            learnts.push_back(learnt);
        }
    }

    void undoOne()
    {
        lit p = trail[trail.size() - 1];
        int x = p.var();
        assigns[x] = Unset;
        reason[x] = nullptr;
        level[x] = -1;
        activity[p.x] += var_inc;
        trail.pop_back();
    }

    bool assume(int num)
    {
        lit p;
        p.x = num;
        trail_lim.push_back(trail.size());
        return enqueue(p, nullptr);
    }

    void cancel()
    {
        int c = trail.size() - trail_lim[trail_lim.size() - 1];
        while (c)
        {
            undoOne();
            c--;
        }
        trail_lim.pop_back();
    }

    void cancelUntil(int level)
    {
        while (decisionLevel() > level)
            cancel();
    }

    int search(int nof_conflicts, int nof_learnt, SearchParams params)
    {
        var_decay = 1 / params.var_decay;
        cla_decay = 1 / params.clause_decay;
        int conflictC = 0;
        //model.clear();
        for(int i = 0;i<literalNum + 1;i++)
        model[i]=0;
        model_size=0;
        while (1)
        {
            Clause* confl = propagate();
            if (confl)
            {
                conflictC++;
                if (decisionLevel() == root_level)
                    return False;
                mVector<lit> learnt_clause;
                int backtr_level;
                analyze(confl, learnt_clause, backtr_level);
                cancelUntil(max(backtr_level, root_level));

                record(learnt_clause);
                if (learnt_clause.size() == 1)
                {
                    level[learnt_clause[0].var()] = 0;
                }
                claDecayActivity();
                varDecayActivity();
            }
            else
            {
                if (decisionLevel() == root_level)
                {
                    simplifyDB();
                }
                if ((int)learnts.size() - (int)nAssigns() >= nof_learnt && nof_learnt >= 0)
                {
                    reduceDB();
                }
                if (nAssigns() == nVars())
                {
                    //find the answer
                    for (int i = 1; i <= nVars(); i++)
                    {
                        model[i]=((assigns[i] == True));
                        model_size++;
                    }
                    cancelUntil(root_level);
                    return 1;
                }
                else
                {
                    if (conflictC >= nof_conflicts && nof_conflicts >= 0)
                    {
                        cancelUntil(root_level);
                        return Unset;
                    }
                    //to find a literal to assign
                    int tmp = -1;
                    double m = -1;
                    lit p;

                    for (int i = 1; i <= nVars(); i++)
                    {
                        p = lit(i);
                        if (m < activity[p.index()] && assigns[p.var()] == Unset)
                        {
                            tmp = p.index();
                            m = activity[p.index()];
                        }
                        p = lit(-i);
                        if (m < activity[p.index()] && assigns[p.var()] == Unset)
                        {
                            tmp = p.index();
                            m = activity[p.index()];
                        }
                    }
                    if (tmp < 0)
                    {
                        for (int i = 1; i <= nVars(); i++)
                        {
                            model[i]=((assigns[i] == True));
                            model_size++;
                        }
                        cancelUntil(root_level);
                        return 1;
                    }
                    assume(tmp ^ 1);
                }
            }
        }
    }

    void varBumpActivity(lit p)
    {
        if (var_decay < 0)
            return;
        if ((activity[p.x] += var_inc) > 1e100)
            varRescaleActivity();
    }
    void varRescaleActivity()
    {
        for (int i = 0; i <= literalNum * 2 + 1; i++)
        {
            activity[i] *= 1e-100;
        }
        var_inc *= 1e-100;
    }
    void varDecayActivity()
    {
        if (var_decay >= 0)
            var_inc *= var_decay;
    }
    void claDecayActivity()
    {
        cla_inc *= cla_decay;
    }
    void claRescaleActivity()
    {
        Clause** arr = learnts.tolist();

        for (int i = 0; i < learnts.size(); i++)
        {
            arr[i]->activity *= 1e-20;
            //learnts[i]->activity *= 1e-20;
            //p=p->next;
        }
        delete(arr);
        cla_inc *= 1e-20;
    }
    void claBumpActivity(Clause* c)
    {
        c->activity += cla_inc;
        if (c->activity > 1e20)
            claRescaleActivity();
    }
    void decayActivities()
    {
        varDecayActivity();
        claDecayActivity();
    }

    void QuickSort(Clause**& a, int low, int high)
    {
        Clause* pivot;
        int i = low, j = high;
        if (low < high)
        {
            pivot = a[low]; //将第一个元素作为枢轴
            //接下来进行第一次partition
            while (i < j)
            {
                while (i < j && a[j]->activity >= pivot->activity) //从右往左找到第一个小于枢轴的数
                    j--;
                if (i < j)
                { //将该数移动到左指针的位置
                    //a.set(i, a[j]);
                    a[i] = a[j];
                    i++;
                }
                while (i < j && a[i]->activity < pivot->activity) //从左往右找到第一个大于等于枢轴的数
                    i++;
                if (i < j)
                { //将该数移动到右指针的位置
                    a[j] = a[i];
                    //a.set(j, a[i]);
                    j--;
                }
            }
            //a.set(i, pivot);
            a[i] = pivot; //循环退出时i == j即枢轴的位置
            //递归处理当前枢轴的左右两部分
            QuickSort(a, low, i - 1);
            QuickSort(a, i + 1, high);
        }
    }

    void reduceDB()
    {
        int i, j;
        double lim = cla_inc / learnts.size();
        Clause** arr=learnts.tolist();
        QuickSort(arr, 0, learnts.size() - 1);
        int learnts_size=learnts.size();
        learnts.clear();
        for (i = j = 0; i < learnts_size / 2; i++)
        {
            if (!(*arr[i]).locked(*this))
            {
            }
            else
            {
                //learnts.set(j++, learnts[i]);
                //arr[j++] = arr[i];
                learnts.push(arr[i]);
            }
        }
        for (; i < learnts_size; i++)
        {
            if (!(*arr[i]).locked(*this) && (*arr[i]).activity < lim)
            {
            }
            else
            {
                //learnts.set(j++, learnts[i]);
                learnts.push(arr[i]);
                //arr[j++] = arr[i];
            }
        }


        delete(arr);
        //learnts.resize(j);
    }

    bool simplifyDB()
    {
        if (propagate() != nullptr)
        {
            return 0;
        }

            Clause** cs =  learnts.tolist() ;
            int j = 0;
            learnts.clear();
            for (int i = 0; i < learnts.size(); i++)
            {
                if ((*cs[i]).simplify(*this))
                {
                }
                else
                    //cs.set(j++, cs[i]);
                    learnts.push(cs[i]);
                    //cs[j++] = cs[i];
            }
            //cs.resize(j);
        delete(cs);
         j = 0;
        for(int i = 0;i<constrs_size;i++)
        {
            if((*constrs[i]).simplify(*this)){

            }else constrs[j++]=constrs[i];
        }
        constrs_size=j;
        return 1;
    }

    int solve()
    {
        SearchParams params(0.95, 0.999);
        double nof_conflicts = 100;
        double nof_learnts = constrs_size / 3;
        int status = Unset;
        root_level = decisionLevel();
        if (propagate())
        {
            cancelUntil(root_level);
            return False;
        }
        simplifyDB();
        while (status == Unset)
        {
            status = search((int)nof_conflicts, (int)nof_learnts, params);
            nof_learnts *= 1.1;
            nof_conflicts *= 1.5;
        }
        cancelUntil(root_level);
        return status == True;
    }
    int read(char filename[])
    {
        FILE* f;
        char s[200];
        int k = 0, j = 0, i;
        if ((f = fopen(filename, "rb")) == NULL)
        {
            printf("open error!\n");
            return -1;
        }
        do
        {
            fgets(s, 200, f);
        } while (s[0] != 'p');
        while (s[j] != '\0')
        {
            if (s[j] - '0' <= 9 && s[j] - '0' >= 0)
                k = k * 10 + (s[j] - '0');
            if (s[j] == ' ')
            {
                literalNum = k;
                k = 0;
            }
            j++;
        }
        clauseNum = k;
        activity = (double*)malloc((literalNum * 2 + 2) * sizeof(double));
        assigns = (char*)malloc((literalNum + 1) * sizeof(char));
        reason = (Clause**)malloc((literalNum + 1) * sizeof(Clause*));
        level = (int*)malloc((literalNum + 1) * (sizeof(int)));
        //watches.resize(2*literalNum+2);
        watches=(mQueue<Clause*>*)malloc((2*literalNum+2)*sizeof(mQueue<Clause*>));
        model=(int*)malloc((literalNum + 1)*sizeof(int));
        //model.resize(literalNum + 1);
        constrs=(Clause**)malloc(clauseNum*sizeof(Clause*));
        for(int i = 0;i<2*literalNum+2;i++){
            //mVector tmp=mVector();
            //watches[i]=&tmp;
            watches[i]=mQueue<Clause*>();
            //watches[i]=(mVector*)malloc(sizeof(mVector));
        }
        //model = (int*)malloc((literalNum + 1) * sizeof(int*));
        for (int i = 0; i < literalNum * 2 + 2; i++)
            activity[i] = 0;
        for (int i = 0; i < literalNum + 1; i++)
        {
            assigns[i] = 0;
            reason[i] = nullptr;
            level[i] = -1;
            model[i]=0;
        }
        for (int i = 0; i < clauseNum; i++)
        {
            //fgets(s,200,f);
            Clause* V = new Clause;
            while (1)
            {
                int tmp;
                fscanf(f, "%d", &tmp);
                if (tmp == 0)
                    break;
                V->lits.push_back(lit(tmp));
                activity[lit(tmp).no()] += var_inc;
            }

            constrs[i]=V;
            constrs_size++;
        }
        for (int i = 0; i < clauseNum; i++)
        {
            if (constrs[i]->lits.size() > 1)
            {
                watches[constrs[i]->lits[0].no()].push_back(constrs[i]);
                watches[constrs[i]->lits[1].no()].push_back(constrs[i]);
            }
            else
            {
                if (assume(constrs[i]->lits[0].x) == 0)
                    return -1;
            }
        }
        fclose(f);
        return literalNum;
    }

    void write(long long timer, int literalnum, char filename[])
    {
        int k = strlen(filename);
        filename[k - 1] = 's';
        filename[k - 2] = 'e';
        filename[k - 3] = 'r';
        FILE* f;
        if ((f = fopen(filename, "wb")) == NULL)
        {
            printf("write error");
            return;
        }
        fprintf(f, "s %d\n", model_size == literalnum);
        if (model_size == literalnum)
        {
            fprintf(f, "v ");
            for (int i = 1; i <= literalnum; i++)
            {
                fprintf(f, "%d ", (model[i] == 0) ? (-(i )) : (i ));
            }
            fprintf(f, "\n");
        }
        fprintf(f, "t %lld", timer);
        fclose(f);
    }
};


//create a new clause and add it to watcher lists
bool Clause::Clause_new(Solver& S, mVector<lit> ps, bool learnt, Clause*& out_clause)
{
    out_clause = NULL;
    if (!learnt)
    {
        for (int i = 0; i < ps.size(); i++)
        {
            if (S.assigns[ps[i].var()] == True)
                return 1;
        }
    }
    if (ps.size() == 0)
        return 0;
    else if (ps.size() == 1)
    {
        Clause* tmp = new Clause;
        return S.enqueue(ps[0], tmp);
    }
    else
    {
        Clause* c = new Clause;
        c->lits = ps;
        c->learnt = learnt;
        c->activity = 0;
        if (learnt)
        {
            //pick a second literal to watch
            double mx = -1;
            int max_i = 0;
            for (int i = 0; i < ps.size(); i++)
            {
                if (S.activity[ps[i].x] > mx)
                {
                    mx = S.activity[ps[i].x];
                    max_i = i;
                }
            }
            int tmp = ps[max_i].x;
            int t1 = ps[1].x;
            c->lits[1].x = tmp;
            c->lits[max_i].x = t1;
            //bumping
            S.claBumpActivity(c);
            for (int i = 0; i < ps.size(); i++)
            {
                lit tmp = ps[i];
                tmp.x ^= 1;
                S.varBumpActivity(tmp);
            }
        }
        S.watches[(c->lits[0]).no()].push_back(c);
        S.watches[(c->lits[1]).no()].push_back(c);
        out_clause = c;
        return 1;
    }
}

//tell if the clause is a unit (learnt clauses only)
bool Clause::locked(Solver S) { return S.reason[this->lits[0].var()] == this; }

bool Clause::simplify(Solver& S)
{
    int j = 0;
    for (int i = 0; i < lits.size(); i++)
    {
        int opt;
        if (lits[i].sign() == 0)
        {
            opt = False;
        }
        else
            opt = True;
        if (S.assigns[lits[i].var()] == opt)
            return 1;
        if (S.assigns[lits[i].var()] == Unset)
        {
            lits[j++] = lits[i];
        }
    }
    lits.resize(j);
    return 0;
}

bool Clause::propagate(Solver& S, lit p)
{
    //make sure the false literal is lits[1]
    if (lits[0].x == p.no())
    {
        lits[0].x = lits[1].x;
        lits[1].x = p.no();
    }
    //if 0th watch is true, then clause is satisfied
    int opt;
    if (lits[0].sign() == 1)
        opt = True;
    else
        opt = False;
    if (S.assigns[lits[0].var()] == opt)
    {
        S.watches[p.x].push_back(this);
        return true;
    }
    //look for a new literal to watch
    for (int i = 2; i < lits.size(); i++)
    {
        if (lits[i].sign() == 0)
            opt = True;
        else
            opt = False;
        if (S.assigns[lits[i].var()] != opt)
        {
            int tm = lits[1].x;
            lits[1].x = lits[i].x;
            lits[i].x = tm;
            S.watches[lits[1].no()].push_back(this);
            return 1;
        }
    }
    //clause is unit under assignment
    S.watches[p.x].push_back(this);
    return S.enqueue(lits[0], this);
}

void Clause::calcReason(Solver& S, lit p, mVector<lit>& out_reason)
{
    int i = 1;
    if (S.assigns[p.var()] == Unset)
    {
        i = 0;
    }
    for (; i < lits.size(); i++)
    {
        lit tmp;
        tmp.x = lits[i].no();
        out_reason.push_back(tmp);
    }
    if (learnt)
        S.claBumpActivity(this);
}