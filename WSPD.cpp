#include <utility>
#include <set>
#include <string.h>
#include <CGAL/number_utils.h>
#include <CGAL/squared_distance_2.h>

#include "easylogging++.h"
#include "WSPD.h"

#define SWAP(x,y) do \
{ \
    unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
    memcpy(swap_temp,&y,sizeof(x)); \
    memcpy(&y,&x,       sizeof(x)); \
    memcpy(&x,swap_temp,sizeof(x)); \
} while(0)

typedef Kernel::Segment_2 Segment_2;


double dist_rectangles(BBox b1, BBox b2)
{
    /*double min_dist = -1;
    for (int i = 0; i < 4; i++)
    {
        Segment_2 seg1(b1.vertex(i), b1.vertex(i+1));
        for (int j = 0; j < 4; j++)
        {
            Segment_2 seg2(b2.vertex(i), b2.vertex(i+1));
            double curr_dist = CGAL::sqrt(CGAL::squared_distance(seg1, seg2));
            if ((min_dist == -1) || (min_dist > curr_dist))
            {
                min_dist = curr_dist;
            }
        }
    }
    return min_dist;*/
    double x_coord_dist = CGAL::max(CGAL::max(b1.xmin() - b2.xmax(),
                                       b2.xmin() - b1.xmax()),
                             0.0);
    double y_coord_dist = CGAL::max(CGAL::max(b1.ymin() - b2.ymax(),
                                       b2.ymin() - b1.ymax()),
                             0.0);
    return CGAL::sqrt(x_coord_dist * x_coord_dist + y_coord_dist * y_coord_dist);
}

// for subclasses
WSPD::WSPD(double s) : s(s) {}

WSPD::WSPD(QuadTree* tree, double s) :
    WSPD(tree, tree, s, -1.0)
{ }

WSPD::WSPD(QuadTree* tree1, QuadTree* tree2, double s) :
    WSPD(tree1, tree2, s, -1.0) // lb_=-1 if no lower bound needed
{ }

WSPD::WSPD(QuadTree* tree1, QuadTree* tree2, double s, double lb) :
    s(s), lb_(lb) // default (no lower bound)
{
    VLOG(7) << "Constructing WSPD";
    pairs = pairing(tree1, tree2);
    collect_distances();
		VLOG(7) << to_string();
		VLOG(5) << "Total pairs found: " << pairs.size();
		VLOG(5) << "Number of distances: " << distances_.size();
}


/* No longer needed.
 * use grid at the level of ub to avoid building the entire tree.
 *
NodePairs WSPD::traverse(QuadTreeTwoClasses* tree, double ub)
{
    NodePairs node_pairs;
    // radius of current node is at least the diameter of its children
    if (tree->radius() > ub)
    {
        for (int i = 0; i < 4; i++)
        {
            QuadTreeTwoClasses* qt_ptr = dynamic_cast<QuadTreeTwoClasses *>(tree->child(i));
            NodePairs ch_pairs = traverse(qt_ptr, ub);
            append_vector(node_pairs, ch_pairs);
        }
        return node_pairs;
    }
    else
    {
        return pairing(tree, tree);
    }
}
*/

NodePairs WSPD::pairing(QuadTree* t1, QuadTree* t2)
{
    NodePairs pairs;

    Vector_2 v = t1->center() - t2->center();
    double dist = dist_rectangles(t1->bbox, t2->bbox);
    VLOG(8) << t1->to_string() << endl << t2->to_string();
    VLOG(8) << "rect distances: " << dist << endl;

    bool swapped = false;
    if (t1->radius() < t2->radius())
    {
        swapped = true;
        SWAP(t1, t2);
    }

    // if lower bound is defined (-1 if undefined)
    /*if (lb_ > 0)
    {
        dist = max(dist, lb_);
    }*/

    // t1 has larger radius by now
    // Well-separated condition: diameter * s <= dist between bbox
    if (2 * s * t1->radius() <= dist)
    {
				LOG_IF(dist == 0 && t1->node_type != QuadTree::LEAF, WARNING) 
						<< "none leaf node pairs satisfy the well-separated condition but has distance 0:\n"
						<< t1->to_string() << ";   " << t2->to_string();
				// for the pair of two trivial quadtrees with 1 point that is the same
				// (the only case when radius = dist = 0), we do not insert the pair
				if (dist != 0)
				{
					pair<QuadTree*, QuadTree*> pair = swapped ? make_pair(t2, t1) : make_pair(t1, t2);
					pairs.insert(pair);
				}
    }
    else
    {
				// if t1->radius() has size lb_, no need to subdivide since its children are not active
				if (t1->radius() <= lb_)
				{
					pair<QuadTree*, QuadTree*> pair = swapped ? make_pair(t2, t1) : make_pair(t1, t2);
					pairs.insert(pair);
				}
				else
				{
					// find the children of this quadtree node
					t1->subdivide();
					// pairing the children of t1 with t2
					for (auto& qt : t1->ch_)
					{
							if (qt->node_type == QuadTree::EMPTY)
							{
									VLOG(9) << "EMPTY";
									continue;
							}
							NodePairs p = swapped ? pairing(t2, qt) : pairing(qt, t2);

							pairs.insert(p.begin(), p.end());
							p.clear();
					}
				}
    }
    return pairs;
}

NodePairs WSPD::pairing2(QuadTreeTwoClasses* t1, QuadTreeTwoClasses* t2)
{
    NodePairs pairs;
    VLOG(7) << "sizes: "<< t1->get_size(0) << " " << t2->get_size(1);

		// check if t1 is active for curve 1, and if t2 is active for curve 2
    if ((t1->get_size(0) == 0) || (t2->get_size(1) == 0))
    {
        return pairs;
    }

    Vector_2 v = t1->center() - t2->center();
    double dist = dist_rectangles(t1->bbox, t2->bbox);
    VLOG(7) << "Tree1: " << t1->to_string();
    VLOG(7) << "Tree2: " << t2->to_string();
    VLOG(7) << "rect distances: " << dist << endl;

    bool swapped = false;
    if (t1->radius() < t2->radius())
    {
        swapped = true;
        SWAP(t1, t2);
    }

    // lower bound is defined
    if (lb_ != -1)
    {
        dist = max(dist, lb_);
    }

    // t1 has larger radius by now
    // Well-separated condition: diameter * s <= dist between bbox
    if (2 * s * t1->radius() <= dist)
    {
        pair<QuadTree*, QuadTree*> pair = swapped ? make_pair(t2, t1) : make_pair(t1, t2);
        pairs.insert(pair);
    }
    else
    {
        // pairing the children of t1 with t2
        for (auto& qt : t1->ch_)
        {
            QuadTreeTwoClasses* qttc = dynamic_cast<QuadTreeTwoClasses*>(qt);
            NodePairs p = swapped ? pairing2(t2, qttc) : pairing2(qttc, t2);

            pairs.insert(p.begin(), p.end());
            p.clear();
        }
    }
    return pairs;
}

/*
 * Resulting distances are sorted in ascending order.
 */
void WSPD::collect_distances()
{
    set<double> dist_set;
    for (auto& qt_pair : pairs)
    {
        dist_set.insert(qt_pair.first->quadtree_dist(*(qt_pair.second)));
    }
    for (auto& dist : dist_set)
    {
        distances_.push_back(dist);
    }
}

vector<double> WSPD::distances()
{
    return distances_;
}

