#ifndef MIXERTREE_H
#define MIXERTREE_H


#include "arraylist.h"
#include "guicast.h"
#include "mixer.inc"
#include "mixertree.inc"


// Stores values of all mixer parameters.  The top row of nodes are each
// mixer parameter.  Each mixer parameter has an array for the channel values.
//
// When the mixer is drawn on the screen, the widgets are ordered according to
// the order of nodes in the tree.
// Toggles are stacked on top of each other.


class MixerTree : public ArrayList<MixerNode*>
{
public:
	MixerTree(Mixer *mixer);
	~MixerTree();

	MixerNode* add_node(int type, int min, int max, char *title, int channels = 0, int record = 0);

	Mixer *mixer;
};

class MixerNode : public ArrayList<MixerNode*>
{
public:
	MixerNode(Mixer *mixer, MixerTree *parent_tree, MixerNode *parent_node);
	~MixerNode();

	int record;
	int min;
	int max;
	int value[MAX_CHANNELS];
	int type;
	int channels;
	int show;
	char title[BCTEXTLEN];

	enum
	{
		TYPE_NONE,
		TYPE_TOGGLE,
		TYPE_SLIDER,
		TYPE_POT,
		TYPE_SLIDER_RECORDABLE,
		TYPE_POT_RECORDABLE
	};

	Mixer *mixer;
	MixerTree *parent_tree;
	MixerNode *parent_node;
};


#endif
