#include "mixertree.h"

MixerTree::MixerTree(Mixer *mixer)
 : ArrayList<MixerNode*>()
{
	this->mixer = mixer;
}

MixerTree::~MixerTree()
{
	remove_all_objects();
}

MixerNode* MixerTree::add_node(int type, 
	int min, 
	int max, 
	char *title, 
	int channels)
{
	MixerNode *result = new MixerNode(mixer, this, 0);
	result->type = type;
	result->min = min;
	result->max = max;
	strcpy(result->title, title);
	result->channels = channels;
	return result;
}









MixerNode::MixerNode(Mixer *mixer, 
	MixerTree *parent_tree, 
	MixerNode *parent_node)
 : ArrayList<MixerNode*>()
{
	this->mixer = mixer;
	this->parent_tree = parent_tree;
	this->parent_node = parent_node;
	title[0] = 0;
	show = 1;
}

MixerNode::~MixerNode()
{
}








