#ifndef QUADTREE
#define QUADTREE

#include <sstream>
#include <vector>
#include <CGAL/Simple_cartesian.h>
#include "easylogging++.h"

#include "curve.h"

using namespace std;

typedef Kernel::Direction_2 Direction_2;
typedef Kernel::Vector_2 Vector_2;
typedef Kernel::Iso_rectangle_2 BBox; //bounding box for 2D point sets

class QuadTree
{
	public:
		// every node has 4 children
		static const int QT_NODE_CAPACITY = 4;

		BBox bbox;

    // representative point (an arbitrary point in a non-empty node
		Point_2 p;

    enum NodeType {EMPTY, LEAF, NODE};
    NodeType node_type;

    QuadTree** children();
    QuadTree* child(int index);
		QuadTree* ch_[4];


		QuadTree(const vector<Point_2> &point_set);
		QuadTree(const vector<Point_2> &point_set, const vector<int> &indices);
		QuadTree(const vector<Point_2> &point_set, const vector<int> &indices, short curve);

    virtual void init();
		
		void insert(Point_2 pt, int index);

    double quadtree_dist(QuadTree that);

    string stringify_point_2(Point_2 p)
    {
        stringstream sstm;
        CGAL::set_pretty_mode(sstm);
        sstm << p;
        return sstm.str();
    }

    string stringify_bbox(BBox bbox)
    {
        stringstream sstm;
        CGAL::set_pretty_mode(sstm);
        sstm << "Bounding box: " << bbox;
        return sstm.str();
    }
		
    virtual string to_string()
    {
        stringstream sstm;
        CGAL::set_pretty_mode(sstm);
        sstm << "QuadTree: {c=" << center_ << ", r=" << radius_ << ", size=" << size() 
						<< ", idx_seg=" << stringify_idx_segments() << "} ";
        return sstm.str();
    }

    int id();
    double radius();
    Point_2 center();
		virtual int size();
		bool is_empty();
		
		vector<int> indices();
		vector<IndexSegment> idx_segments();
		
		virtual void subdivide();

  protected:

    vector<Point_2> point_set1_;
    vector<int> indices_;
		/* 
		 * segments of consecutive indices in this quadtree,
		 * computed at init().
		 */
		vector<IndexSegment> idx_segments_;

    vector<int> default_indices(int n);

    void set_empty();
    void calc_bbox(vector<Point_2> &point_set);
    vector<vector<Point_2>> partition(vector<Point_2> point_set,
                                      vector<int> indices,
                                      vector<vector<int>> &ch_indices);

  private:

    // each node has a unique id property.
    // a node closer to root always receives a small id.
    static int max_id_;
    int id_;
    double radius_;
    Point_2 center_;
		// optional: 0 for unknown; 1 for first curve; 2 for second curve
		short curve_;
		
		// flag indicating if the cell has been subdivided
		bool has_subdivided_;
		// flag indicating if the cell has be initialized
		bool has_initialized_;
		
		string stringify_indices()
		{
			if (is_empty())
			{
				return "";
			}
			stringstream sstm;
			sstm << "{" << indices_[0];
			for (int i = 1; i < indices_.size(); i++)
			{
				sstm << "," << indices_[i];
			}
			sstm << "} ";
			return sstm.str();
		}
		
		string stringify_idx_segment(pair<int, int> seg)
		{
			stringstream sstm;
			sstm << seg.first << "-" << seg.second;
			return sstm.str();
		}
		
		string stringify_idx_segments()
		{
			if (is_empty())
			{
				return "";
			}
			stringstream sstm;
			sstm << "{" << stringify_idx_segment(idx_segments_[0]);
			for (int i = 1; i < idx_segments_.size(); i++)\
			{
				sstm << "," << stringify_idx_segment(idx_segments_[i]);
			}
			sstm << "} ";
			return sstm.str();
		}

    Direction_2 pos_x_dir_;
    Direction_2 pos_y_dir_;
    Direction_2 neg_x_dir_;
    Direction_2 neg_y_dir_;

};

#endif
