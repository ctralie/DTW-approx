
#include "easylogging++.h"
#include "QuadTreeTwoClasses.h"

using namespace std;
QuadTreeTwoClasses::QuadTreeTwoClasses(const vector<Point_2> &point_set1,
                                       const vector<Point_2> &point_set2):
    QuadTree(point_set1), point_set2_(point_set2)
{ }

void QuadTreeTwoClasses::choose_representatives()
{
    if (!point_set1_.empty())
        representatives_[0] = point_set1_[0];
    if (!point_set2_.empty())
        representatives_[1] = point_set2_[0];
}

void QuadTreeTwoClasses::init()
{
    set_sizes_[0] = point_set1_.size();
    set_sizes_[1] = point_set2_.size();

    int tot_size = point_set1_.size() + point_set2_.size();
    vector<Point_2> all_points(point_set1_);
    all_points.reserve(tot_size);
    all_points.insert(all_points.end(), point_set2_.begin(), point_set2_.end());
    switch (tot_size)
    {
        case 0:
            set_empty();
            break;
        case 1:
            node_type = LEAF;
            calc_bbox(all_points);
            break;
        default:
            node_type = NODE;
            calc_bbox(all_points);
            subdivide();
    }
    choose_representatives();
}

void QuadTreeTwoClasses::subdivide()
{
    vector<vector<Point_2>> ch_point_sets1 = partition(point_set1_);
    vector<vector<Point_2>> ch_point_sets2 = partition(point_set2_);

    // continue to subdivide if more than 1 quadrant has points
    int num_ch = 0;
    for (int i = 0; i < 4; i++)
    {
        if ((!ch_point_sets1[i].empty()) || (!ch_point_sets2[i].empty()))
        {
            num_ch++;
        }
    }
    if (num_ch <= 1)
    {
        return;
    }

    //subdivide
    for (int i = 0; i < 4; i++)
    {
        ch_[i] = new QuadTreeTwoClasses(ch_point_sets1[i], ch_point_sets2[i]);
        ch_[i]->init();
    }
}

int QuadTreeTwoClasses::get_size(int index)
{
    return set_sizes_[index];
}

Point_2 QuadTreeTwoClasses::get_representative(int index)
{
    return representatives_[index];
}
