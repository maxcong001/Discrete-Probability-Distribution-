
#include "logger/logger.hpp"
#include "DPD/dpd.hpp"
#include "DPD/rdpd.hpp"
#include <map>
int main()
{
    set_log_level(logger_iface::log_level::debug);

    std::map<int, int> m;

    RDPD<int> dpd;
    dpd.add_obj(1, 10);
    dpd.add_obj(2, 10);
    dpd.add_obj(3, 30);
    dpd.add_obj(4, 10);
    dpd.inc_weight(4, 10);
    dpd.del_obj(1);
    dpd.inc_weight(4, 10);
    dpd.inc_weight(2, 20);



    int _get_value = 0;
    for (int i = 0; i < 60000; i++)
    {
        dpd.get_obj(_get_value);
        ++m[_get_value];
        __LOG(debug, "get : " << _get_value);
    }
    for (auto p : m)
    {
        std::cout << p.first << " generated " << p.second << " times\n";
    }
}
