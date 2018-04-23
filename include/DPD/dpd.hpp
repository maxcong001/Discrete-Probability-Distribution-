#pragma once

#include <random>

template <typename DIST_OBJ>
class DPD
{
  public:
    DPD() : _gen(_rd())
    {
    }
    bool add_obj(DIST_OBJ obj, unsigned int weight = 0) { return update_obj(obj, weight); }
    bool del_obj(DIST_OBJ obj)
    {
        // note: there should only have one obj in the vector!!
        for (auto it = _obj_vector.begin(); it != _obj_vector.end(); ++it)
        {
            if (std::get<0>(*it) == obj)
            {
                _obj_vector.erase(it);
            }
        }
        update();
        return true;
    }

    bool get_obj(DIST_OBJ &obj)
    {
        try
        {
            obj = std::get<0>(_obj_vector.at(_dist(_gen)));
        }
        catch (const std::out_of_range &oor)
        {
            __LOG(error, "Out of Range error: " << oor.what());
            return false;
        }
        return true;
    }
    // note: return 0 if not found
    unsigned int get_weight(DIST_OBJ obj)
    {
        for (auto it : _obj_vector)
        {
            if (std::get<0>(it) == obj)
            {
                return std::get<1>(it);
            }
        }
        return 0;
    }
    bool update_obj(DIST_OBJ obj, unsigned int weight)
    {
        _obj_vector.push_back(std::make_pair(obj, weight));
        return update();
    }
    bool update()
    {
        int vector_size = _obj_vector.size();
        std::vector<double> init_list;
        //std::array<double, 10000> init_list;
        for (int i = 0; i < vector_size; i++)
        {
            init_list.push_back(std::get<1>(_obj_vector[i]));
        }
        std::discrete_distribution<int> second_dist(init_list.begin(), init_list.end());
        _dist.param(second_dist.param());
        return true;
    }

    bool inc_obj(DIST_OBJ obj, unsigned int weight)
    {
        unsigned int _weight = get_weight(obj);
        _weight += weight;
        return update_obj(obj, _weight);
    }
    bool dec_obj(DIST_OBJ obj, unsigned int weight)
    {
        unsigned int _weight = get_weight(obj);
        _weight -= weight;
        if (_weight < 0)
        {
            _weight = 0;
        }
        return update_obj(obj, _weight);
    }

    std::vector<std::pair<DIST_OBJ, unsigned int>> _obj_vector;
    std::random_device _rd;
    std::mt19937 _gen;
    std::discrete_distribution<int> _dist;
};
