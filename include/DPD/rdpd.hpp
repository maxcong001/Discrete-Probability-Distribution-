#pragma once

#include <random>
#include <mutex>

template <typename DIST_OBJ>
class RDPD
{
  public:
    RDPD() : _gen(_rd())
    {
    }
    bool add_obj(DIST_OBJ obj, unsigned int weight = 0) { return update_obj(obj, weight); }
    bool del_obj(DIST_OBJ obj)
    {
        // note: there should only have one obj in the vector!!
        {
            std::lock_guard<std::mutex> lck(_mutex);
            for (auto it = _obj_vector.begin(); it != _obj_vector.end(); ++it)
            {
                if (std::get<0>(*it) == obj)
                {
                    _obj_vector.erase(it);
                }
            }
        }
        update();
        return true;
    }
    // note: there is no lock here
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
        std::lock_guard<std::mutex> lck(_mutex);
        for (auto it : _obj_vector)
        {
            if (std::get<0>(it) == obj)
            {
                return std::get<1>(it);
            }
        }
        return 0;
    }
    std::vector<double> get_weight_list()
    {
        std::lock_guard<std::mutex> lck(_mutex);
        std::vector<double> init_list;
        for (auto it : _obj_vector)
        {
            init_list.push_back(std::get<1>(it));
        }
        return init_list;
    }
    bool update_obj(DIST_OBJ obj, unsigned int weight)
    {
        {
            std::lock_guard<std::mutex> lck(_mutex);
            std::vector<std::pair<DIST_OBJ, unsigned int>> tmp_obj_vector;

            bool found = false;
            for (auto it = _obj_vector.begin(); it != _obj_vector.end(); ++it)
            {
                if (std::get<0>(*it) == obj)
                {
                    tmp_obj_vector.push_back(std::make_pair(obj, weight));
                    found = true;
                }
                else
                {
                    tmp_obj_vector.push_back(*it);
                }
            }
            if (!found)
            {
                tmp_obj_vector.push_back(std::make_pair(obj, weight));
            }
            _obj_vector.swap(tmp_obj_vector);
        }
        return update();
    }
    bool update()
    {
        std::lock_guard<std::mutex> lck(_mutex);

        int vector_size = _obj_vector.size();
        std::vector<double> init_list;
        //std::array<double, 10000> init_list;
        unsigned int max_weight = 0;
        unsigned int min_weight = std::get<1>(_obj_vector[0]);
        for (int i = 0; i < vector_size; i++)
        {
            unsigned int tmp_weight = std::get<1>(_obj_vector[i]);
            if (tmp_weight > max_weight)
            {
                max_weight = tmp_weight;
            }
            if (tmp_weight < min_weight)
            {
                min_weight = tmp_weight;
            }
        }
        for (int i = 0; i < vector_size; i++)
        {
            unsigned int tmp_weight = max_weight - std::get<1>(_obj_vector[i]);
            if (tmp_weight == 0)
            {
#if 0
                // make sure this is larger than 0
                tmp_weight += (max_weight - min_weight) / vector_size;
                if (tmp_weight == 0)
                {
                    tmp_weight = 1;
                }
#endif
                tmp_weight = 1;
            }
            init_list.push_back(tmp_weight);
        }

        __LOG(debug, "weight is :");
        for (auto it : init_list)
        {
            __LOG(debug, "--> " << it);
        }
        std::discrete_distribution<int> second_dist(init_list.begin(), init_list.end());
        _dist.param(second_dist.param());
        return true;
    }

    bool inc_weight(DIST_OBJ obj, unsigned int weight)
    {
        unsigned int _weight = 0;
        _weight = get_weight(obj);
        _weight += weight;

        return update_obj(obj, _weight);
    }
    bool dec_weight(DIST_OBJ obj, unsigned int weight)
    {
        unsigned int _weight = 0;
        _weight = get_weight(obj);
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
    std::mutex _mutex;
};
