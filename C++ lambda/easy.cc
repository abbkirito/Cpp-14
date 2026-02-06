#include"pch.h"

// ======================  Instrumentor  ======================
class Instrumentor
{
public:
    static Instrumentor& get()
    {
        static Instrumentor instance;
        return instance;
    }
    Instrumentor(const Instrumentor&) = delete;
    Instrumentor& operator=(const Instrumentor&) = delete;

    void BeginSession(const std::string& name,
                      const std::string& filepath = "trace.json")
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_CurrentSession) EndSession();
        m_OutputStream.open(filepath);
        WriteHeader();
        m_SessionName = name;
        m_CurrentSession = true;
    }

    void EndSession()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (!m_CurrentSession) return;
        WriteFooter();
        m_OutputStream.close();
        m_CurrentSession = false;
    }

    void WriteEvent(const char* name,
                    std::chrono::microseconds start,
                    std::chrono::microseconds dur)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (!m_CurrentSession) return;
        if (m_FirstEvent) m_FirstEvent = false;
        else m_OutputStream << ",\n";
        m_OutputStream << "{"
                       << "\"name\":\"" << name << "\","
                       << "\"cat\":\"function\","
                       << "\"ph\":\"X\","
                       << "\"ts\":" << start.count() << ','
                       << "\"dur\":" << dur.count() << ','
                       << "\"pid\":0,\"tid\":0"
                       << "}";
        m_OutputStream.flush();
    }

private:
    Instrumentor() : m_CurrentSession(false), m_FirstEvent(true) {}
    ~Instrumentor() { EndSession(); }

    void WriteHeader()
    {
        m_OutputStream << "{\"traceEvents\":[";
        m_OutputStream.flush();
        m_FirstEvent = true;
    }
    void WriteFooter()
    {
        m_OutputStream << "\n]}\n";
        m_OutputStream.flush();
    }

    std::ofstream m_OutputStream;
    std::mutex    m_Mutex;
    std::string   m_SessionName;
    bool          m_CurrentSession;
    bool          m_FirstEvent;
};

// ======================  Timer  ======================
class Timer
{
public:
    explicit Timer(std::string name)
        : m_Name(std::move(name)), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::steady_clock::now();
    }

    void Stop()
    {
        if (m_Stopped) return;
        m_Stopped = true;

        auto end = std::chrono::steady_clock::now();
        auto startUs = std::chrono::duration_cast<std::chrono::microseconds>(
                           m_StartTimepoint.time_since_epoch());
        auto durUs   = std::chrono::duration_cast<std::chrono::microseconds>(
                           end - m_StartTimepoint);

        Instrumentor::get().WriteEvent(m_Name.c_str(), startUs, durUs);
    }

    ~Timer() { if (!m_Stopped) Stop(); }

private:
    std::string m_Name;
    using time_point = std::chrono::time_point<std::chrono::steady_clock>;
    time_point  m_StartTimepoint;
    bool        m_Stopped;
};

// ======================  使用示例  ======================


/*
class Log
{
public:
 const int LogLevelError =0;
 const int LogLevelWarning = 1;
 const int LogLevelInfo =2;


  private: 
  int m_Loglevel =LogLevelInfo;

  public:
  void SetLevel(int level)
  {
    m_Loglevel = level;

  }
    void Warn(const char& message)
  {
    if(m_Loglevel>=LogLevelError)
    {
    std::cout<<"[Error]" <<message<<std::endl;
    }

  }
  void Warn(const char& message)
  {
    std::cout<<"[Warning]" <<message<<std::endl;

  }
    void Warn(const char& message)
  {
    std::cout<<"[Info]" <<message<<std::endl;

  }

};
*/


int main()
{
 /* std::vector<int> v = {1,2,3,4,5,6,7,8,9,0};

// before
for(int x : v)
{
    std::cout<<x;
}

std::cout<<"lambda 方法"<<std::endl;
// lambda 方法
std::for_each(v.begin(),v.end(), [](int x){std::cout<<x;});
int var =8;
int* ptr = & var;
std::cout<<"ptr 的原址"<<ptr <<"ptr存储的值"<<*ptr<<std::endl;

int** ptr1 = &ptr;
std::cout<<"ptr1的原址"<<ptr1 <<"*ptr 的第一层地址"<<*ptr1<<"ptr1->ptr"<<*(*ptr1)<<std::endl;

 Instrumentor::get().BeginSession("Profile");

    {
        Timer t("loop_cout");
        for (int i = 0; i < 10'000; ++i)
            std::cout << "Hello world\n";
    }

    {
        Timer t("loop_printf");
        for (int i = 0; i < 10'000; ++i)
            printf("Hello world\n");
    }

    Instrumentor::get().EndSession();   // 可选，析构也会调
    return 0;*/
  auto fun =[](int x){return x; };
  std::cout<<fun(89)<<std::endl;


}
