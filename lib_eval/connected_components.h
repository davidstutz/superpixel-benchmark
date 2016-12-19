/**
 * Written and published by Ali Rahimi;
 * see http://xenia.media.mit.edu/~rahimi/connected/.
 */

#ifndef _CONNECTED_H
#define _CONNECTED_H

#include <vector>
#include <algorithm>

/** \brief Efficient (multi-label) connected components algorithm.
 * \see http://xenia.media.mit.edu/~rahimi/connected/
 * \author Ali Rahimi
 */
class ConnectedComponents
{
public:
    ConnectedComponents(int soft_maxlabels) : labels(soft_maxlabels) {
	clear();
    }
    void clear() {
	std::fill(labels.begin(), labels.end(), Similarity());
	highest_label = 0;
    }
    template<class Tin, class Tlabel, class Comparator, class Boolean>
    int connected(const Tin *img, Tlabel *out,
		     int width, int height, Comparator,
		  Boolean K8_connectivity);

private:
    struct Similarity {
	Similarity() : id(0), sameas(0) {}
	Similarity(int _id, int _sameas) : id(_id), sameas(_sameas) {}
	Similarity(int _id) : id(_id), sameas(_id) {}
	int id, sameas, tag;
    };

    bool is_root_label(int id) {
	return (labels[id].sameas == id);
    }
    int root_of(int id) {
	while (!is_root_label(id)) {
	    // link this node to its parent's parent, just to shorten
	    // the tree.
	    labels[id].sameas = labels[labels[id].sameas].sameas;

	    id = labels[id].sameas;
	}
	return id;
    }
    bool is_equivalent(int id, int as) {
	return (root_of(id) == root_of(as));
    }
    bool merge(int id1, int id2) {
	if(!is_equivalent(id1, id2)) {
	    labels[root_of(id1)].sameas = root_of(id2);
	    return false;
	}
	return true;
    }
    int new_label() {
	if(highest_label+1 > labels.size())
	    labels.reserve(highest_label*2);
	labels.resize(highest_label+1);
	labels[highest_label] = Similarity(highest_label);
	return highest_label++;
    }


    template<class Tin, class Tlabel, class Comparator, class Boolean>
    void label_image(const Tin *img, Tlabel *out,
		     int width, int height, Comparator,
		     Boolean K8_connectivity);
    template<class Tlabel>
    int relabel_image(Tlabel *out, int width, int height);


    std::vector<Similarity> labels;
    int highest_label;
};

template<class Tin, class Tlabel, class Comparator, class Boolean>
int
ConnectedComponents::connected(const Tin *img, Tlabel *labelimg,
			       int width, int height, Comparator SAME,
			       Boolean K8_connectivity)
{
    label_image(img,labelimg, width,height, SAME, K8_connectivity);
    return relabel_image(labelimg, width,height);
}

template<class Tin, class Tlabel, class Comparator, class Boolean>
void
ConnectedComponents::label_image(const Tin *img, Tlabel *labelimg,
				 int width, int height, Comparator SAME,
				 const Boolean K8_CONNECTIVITY)
{
    const Tin *row = img;
    const Tin *last_row = 0;
    struct Label_handler {
	Label_handler(const Tin *img, Tlabel *limg) :
	    piximg(img), labelimg(limg) {}
	Tlabel &operator()(const Tin *pixp) { return labelimg[pixp-piximg]; }
	const Tin *piximg;
	Tlabel *labelimg;
    } label(img, labelimg);

    clear();

    label(&row[0]) = new_label();

    // label the first row.
    for(int c=1; c<width; ++c) {
	if(SAME(row[c], row[c-1]))
	    label(&row[c]) = label(&row[c-1]);
	else
	    label(&row[c]) = new_label();
    }

    // label subsequent rows.
    for(int r=1; r<height; ++r)    {
	// label the first pixel on this row.
	last_row = row;
	row = &img[width*r];

	if(SAME(row[0], last_row[0]))
	    label(&row[0]) = label(&last_row[0]);
	else
	    label(&row[0]) = new_label();

	// label subsequent pixels on this row.
	for(int c=1; c<width; ++c)	{
	    int mylab = -1;

	    // inherit label from pixel on the left if we're in the same blob.
	    if(SAME(row[c],row[c-1]))
		mylab = label(&row[c-1]);
	    for(int d=(K8_CONNECTIVITY?-1:0); d<1; ++d) {
		// if we're in the same blob, inherit value from above pixel.
		// if we've already been assigned, merge its label with ours.
		if(SAME(row[c], last_row[c+d])) {
		    if(mylab>=0) merge(mylab, label(&last_row[c+d]));
		    else mylab = label(&last_row[c+d]);
		}
	    }
	    if(mylab>=0) label(&row[c]) = static_cast<Tlabel>(mylab);
	    else label(&row[c]) = new_label();

	    if(K8_CONNECTIVITY && SAME(row[c-1], last_row[c]))
		merge(label(&row[c-1]), label(&last_row[c]));
	}
    }
}

template<class Tlabel>
int
ConnectedComponents::relabel_image(Tlabel *labelimg, int width, int height)
{
    int newtag = 0;
    for(int id=0; id<labels.size(); ++id)
	if(is_root_label(id))
	    labels[id].tag = newtag++;

    for(int i = 0; i<width*height; ++i)
	labelimg[i] = labels[root_of(labelimg[i])].tag;

    return newtag;
}


#endif // _CONNECTED_H