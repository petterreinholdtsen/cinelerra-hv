#include "edl.h"
#include "edlsession.h"
#include "deleteallindexes.h"
#include "mwindow.h"
#include "performanceprefs.h"
#include "preferences.h"
#include <string.h>

PerformancePrefs::PerformancePrefs(MWindow *mwindow, PreferencesWindow *pwindow)
 : PreferencesDialog(mwindow, pwindow)
{
	hot_node = -1;
}

PerformancePrefs::~PerformancePrefs()
{
	delete ipath;
	delete ipathtext;
	delete isize;
	delete csize;
	delete deleteall;
//	delete smp;
	delete icount;
}

int PerformancePrefs::create_objects()
{
	int x = 5, y = 5;
	char string[1024];

	node_list = 0;
	generate_node_list();

// 	add_border(get_resources()->get_bg_shadow1(),
// 		get_resources()->get_bg_shadow2(),
// 		get_resources()->get_bg_color(),
// 		get_resources()->get_bg_light2(),
// 		get_resources()->get_bg_light1());
	add_subwindow(new BC_Title(x, y, "Performance", LARGEFONT, BLACK));

	y += 30;
	add_subwindow(new BC_Title(x, y, "Index files go here:", MEDIUMFONT, BLACK));
	add_subwindow(ipathtext = new IndexPathText(x + 230, y, pwindow, pwindow->thread->preferences->index_directory));
	add_subwindow(ipath = new BrowseButton(mwindow,
		this,
		ipathtext, 
		x + 230 + ipathtext->get_w(), 
		y, 
		pwindow->thread->preferences->index_directory,
		"Index Path", 
		"Select the directory for index files",
		1));

	y += 30;
	add_subwindow(new BC_Title(x, y + 5, "Size of index file:", MEDIUMFONT, BLACK));
	sprintf(string, "%ld", pwindow->thread->preferences->index_size);
	add_subwindow(isize = new IndexSize(x + 230, y, pwindow, string));
	y += 30;
	add_subwindow(new BC_Title(x, y + 5, "Number of index files to keep:", MEDIUMFONT, BLACK));
	sprintf(string, "%ld", pwindow->thread->preferences->index_count);
	add_subwindow(icount = new IndexCount(x + 230, y, pwindow, string));
	add_subwindow(deleteall = new DeleteAllIndexes(mwindow, pwindow, 350, y));
	y += 30;
	add_subwindow(new BC_Title(x, y + 5, "Cache items:", MEDIUMFONT, BLACK));
	sprintf(string, "%ld", pwindow->thread->edl->session->cache_size);
	add_subwindow(csize = new CICacheSize(x + 230, y, pwindow, string));
	y += 30;
	add_subwindow(new BC_Title(x, y + 5, "Seconds to preroll renders:"));
	PrefsRenderPreroll *preroll = new PrefsRenderPreroll(pwindow, 
		this, 
		x + 230, 
		y);
	preroll->create_objects();
	y += 30;
	add_subwindow(new PrefsForceUniprocessor(pwindow, x, y));
	
	
	
	y += 40;




	int xmargin2 = 310;
	int xmargin3 = 560;
	add_subwindow(new BC_Title(x, y, "Render Farm", LARGEFONT, BLACK));
	y += 30;

	add_subwindow(new PrefsRenderFarm(pwindow, x, y));
	y += 30;

	y += 30;
	add_subwindow(new BC_Title(x, y, "Nodes:"));
	add_subwindow(new BC_Title(x + xmargin2, y, "Hostname:"));
	add_subwindow(new BC_Title(x + xmargin3, y, "Port:"));
	y += 25;
	add_subwindow(node_list = new PrefsRenderFarmNodes(pwindow, this, x, y));

	add_subwindow(edit_node = new PrefsRenderFarmEditNode(pwindow, this, x + xmargin2, y));
	PrefsRenderFarmPort *port = new PrefsRenderFarmPort(pwindow, this, x + xmargin3, y);
	port->create_objects();

	y += 30;


	add_subwindow(new PrefsRenderFarmReplaceNode(pwindow, this, x + xmargin2, y));
	y += 30;
	add_subwindow(new PrefsRenderFarmNewNode(pwindow, this, x + xmargin2, y));
	y += 30;
	add_subwindow(new PrefsRenderFarmDelNode(pwindow, this, x + xmargin2, y));
	y += 30;
	add_subwindow(new PrefsRenderFarmSortNodes(pwindow, 
		this, 
		x + xmargin2, 
		y));
	y += 35;
//	add_subwindow(new PrefsRenderFarmConsolidate(pwindow, 
//		x + xmargin2, 
//		y));
//	y += 30;
	add_subwindow(new BC_Title(x + xmargin2, y, "Filesystem prefix on remote nodes:"));
	add_subwindow(new PrefsRenderFarmMountpoint(pwindow, 
		this, 
		x + xmargin3, 
		y));
	y += 30;
	add_subwindow(new BC_Title(x + xmargin2, y, "Total jobs to create:"));
	add_subwindow(new BC_Title(x + xmargin2, y + 30, "(overridden if new file at each label is checked)"));
	PrefsRenderFarmJobs *jobs = new PrefsRenderFarmJobs(pwindow, 
		this, 
		x + xmargin3, 
		y);
	jobs->create_objects();

	return 0;
}

void PerformancePrefs::generate_node_list()
{
	int selected_row = node_list ? node_list->get_selection_number(0, 0) : -1;
	nodes[0].remove_all_objects();
	nodes[1].remove_all_objects();
	nodes[2].remove_all_objects();
	for(int i = 0; 
		i < pwindow->thread->preferences->renderfarm_nodes.total; 
		i++)
	{
		BC_ListBoxItem *item;
		nodes[0].append(item = new BC_ListBoxItem(
			(char*)(pwindow->thread->preferences->renderfarm_enabled.values[i] ? "X" : " ")));
		if(i == selected_row) item->set_selected(1);

		nodes[1].append(item = new BC_ListBoxItem(
			pwindow->thread->preferences->renderfarm_nodes.values[i]));
		if(i == selected_row) item->set_selected(1);

		char string[BCTEXTLEN];
		sprintf(string, "%d", pwindow->thread->preferences->renderfarm_ports.values[i]);
		nodes[2].append(item = new BC_ListBoxItem(string));
		if(i == selected_row) item->set_selected(1);
	}
}

static char *titles[] = 
{
	"On",
	"Hostname",
	"Port"
};

static int widths[] = 
{
	30,
	200,
	50
};


void PerformancePrefs::update_node_list()
{
	node_list->update(nodes,
						titles,
						widths,
						3,
						node_list->get_xposition(),
						node_list->get_yposition(),
						node_list->get_selection_number(0, 0));
}




IndexPathText::IndexPathText(int x, int y, PreferencesWindow *pwindow, char *text)
 : BC_TextBox(x, y, 240, 1, text)
{
	this->pwindow = pwindow; 
}

IndexPathText::~IndexPathText() {}

int IndexPathText::handle_event()
{
	strcpy(pwindow->thread->preferences->index_directory, get_text());
}




IndexSize::IndexSize(int x, int y, PreferencesWindow *pwindow, char *text)
 : BC_TextBox(x, y, 100, 1, text)
{ 
	this->pwindow = pwindow; 
}

int IndexSize::handle_event()
{
	long result;

	result = atol(get_text());
	if(result < 64000) result = 64000;
	//if(result < 500000) result = 500000;
	pwindow->thread->preferences->index_size = result;
	return 0;
}



IndexCount::IndexCount(int x, int y, PreferencesWindow *pwindow, char *text)
 : BC_TextBox(x, y, 100, 1, text)
{ 
	this->pwindow = pwindow; 
}

int IndexCount::handle_event()
{
	long result;

	result = atol(get_text());
	if(result < 1) result = 1;
	pwindow->thread->preferences->index_count = result;
	return 0;
}



PrefsSMP::PrefsSMP(int x, int y, PreferencesWindow *pwindow)
 : BC_TextBox(x, y, 100, 1, pwindow->thread->edl->session->smp + 1)
{ 
	this->pwindow = pwindow; 
}

PrefsSMP::~PrefsSMP()
{
}

int PrefsSMP::handle_event()
{
	pwindow->thread->edl->session->smp = atol(get_text()) - 1;
	if(pwindow->thread->edl->session->smp < 0) pwindow->thread->edl->session->smp = 0;
	return 0;
}


CICacheSize::CICacheSize(int x, int y, PreferencesWindow *pwindow, char *text)
 : BC_TextBox(x, y, 100, 1, text)
{ 
	this->pwindow = pwindow; 
}

int CICacheSize::handle_event()
{
	long result;

	result = atol(get_text());
	pwindow->thread->edl->session->cache_size = result;
	return 0;
}


PrefsRenderPreroll::PrefsRenderPreroll(PreferencesWindow *pwindow, 
		PerformancePrefs *subwindow, 
		int x, 
		int y)
 : BC_TumbleTextBox(subwindow, 
 	(float)pwindow->thread->preferences->render_preroll,
	(float)0, 
	(float)100,
	x,
	y,
	100)
{
	this->pwindow = pwindow;
}
PrefsRenderPreroll::~PrefsRenderPreroll()
{
}
int PrefsRenderPreroll::handle_event()
{
	pwindow->thread->preferences->render_preroll = atof(get_text());
	return 1;
}











PrefsRenderFarm::PrefsRenderFarm(PreferencesWindow *pwindow, int x, int y)
 : BC_CheckBox(x, 
 	y, 
	pwindow->thread->preferences->use_renderfarm,
	"Use render farm for rendering")
{
	this->pwindow = pwindow;
}
PrefsRenderFarm::~PrefsRenderFarm()
{
}
int PrefsRenderFarm::handle_event()
{
	pwindow->thread->preferences->use_renderfarm = get_value();
	return 1;
}




PrefsForceUniprocessor::PrefsForceUniprocessor(PreferencesWindow *pwindow, int x, int y)
 : BC_CheckBox(x, 
 	y, 
	pwindow->thread->edl->session->force_uniprocessor,
	"Force single processor use")
{
	this->pwindow = pwindow;
}
PrefsForceUniprocessor::~PrefsForceUniprocessor()
{
}
int PrefsForceUniprocessor::handle_event()
{
	pwindow->thread->edl->session->force_uniprocessor = get_value();
	return 1;
}







PrefsRenderFarmConsolidate::PrefsRenderFarmConsolidate(PreferencesWindow *pwindow, int x, int y)
 : BC_CheckBox(x, 
 	y, 
	pwindow->thread->preferences->renderfarm_consolidate,
	"Consolidate output files on completion")
{
	this->pwindow = pwindow;
}
PrefsRenderFarmConsolidate::~PrefsRenderFarmConsolidate()
{
}
int PrefsRenderFarmConsolidate::handle_event()
{
	pwindow->thread->preferences->renderfarm_consolidate = get_value();
	return 1;
}





PrefsRenderFarmPort::PrefsRenderFarmPort(PreferencesWindow *pwindow, 
	PerformancePrefs *subwindow, 
	int x, 
	int y)
 : BC_TumbleTextBox(subwindow, 
 	(long)pwindow->thread->preferences->renderfarm_port,
	(long)1, 
	(long)65535,
	x,
	y,
	100)
{
	this->pwindow = pwindow;
}

PrefsRenderFarmPort::~PrefsRenderFarmPort()
{
}

int PrefsRenderFarmPort::handle_event()
{
	pwindow->thread->preferences->renderfarm_port = atol(get_text());
	return 1;
}



PrefsRenderFarmNodes::PrefsRenderFarmNodes(PreferencesWindow *pwindow, 
	PerformancePrefs *subwindow, int x, int y)
 : BC_ListBox(x, 
		y, 
		300, 
		250,
		LISTBOX_TEXT,                         // Display text list or icons
		subwindow->nodes,
		titles,
		widths,
		3)
{
	this->subwindow = subwindow;
	this->pwindow = pwindow;
}
PrefsRenderFarmNodes::~PrefsRenderFarmNodes()
{
}

int PrefsRenderFarmNodes::column_resize_event()
{
	for(int i = 0; i < 3; i++)
		widths[i] = get_column_width(i);
	return 1;
}

int PrefsRenderFarmNodes::handle_event()
{
	if(get_selection_number(0, 0) >= 0)
	{
		subwindow->hot_node = get_selection_number(1, 0);
		subwindow->edit_node->update(get_selection(1, 0)->get_text());
		if(get_cursor_x() < widths[0])
		{
			pwindow->thread->preferences->renderfarm_enabled.values[subwindow->hot_node] = 
				!pwindow->thread->preferences->renderfarm_enabled.values[subwindow->hot_node];
			subwindow->generate_node_list();
			subwindow->update_node_list();
		}
	}
	else
	{
		subwindow->hot_node = -1;
		subwindow->edit_node->update("");
	}
	return 1;
}	
int PrefsRenderFarmNodes::selection_changed()
{
	handle_event();
	return 1;
}







PrefsRenderFarmEditNode::PrefsRenderFarmEditNode(PreferencesWindow *pwindow, PerformancePrefs *subwindow, int x, int y)
 : BC_TextBox(x, y, 200, 1, "")
{
	this->pwindow = pwindow;
	this->subwindow = subwindow;
}

PrefsRenderFarmEditNode::~PrefsRenderFarmEditNode()
{
}

int PrefsRenderFarmEditNode::handle_event()
{
	return 1;
}






PrefsRenderFarmNewNode::PrefsRenderFarmNewNode(PreferencesWindow *pwindow, PerformancePrefs *subwindow, int x, int y)
 : BC_GenericButton(x, y, "Add Node")
{
	this->pwindow = pwindow;
	this->subwindow = subwindow;
}
PrefsRenderFarmNewNode::~PrefsRenderFarmNewNode()
{
}
int PrefsRenderFarmNewNode::handle_event()
{
	pwindow->thread->preferences->add_node(subwindow->edit_node->get_text(),
		pwindow->thread->preferences->renderfarm_port,
		1);
	subwindow->generate_node_list();
	subwindow->update_node_list();
	subwindow->hot_node = -1;
	return 1;
}







PrefsRenderFarmReplaceNode::PrefsRenderFarmReplaceNode(PreferencesWindow *pwindow, PerformancePrefs *subwindow, int x, int y)
 : BC_GenericButton(x, y, "Replace Node")
{
	this->pwindow = pwindow;
	this->subwindow = subwindow;
}
PrefsRenderFarmReplaceNode::~PrefsRenderFarmReplaceNode()
{
}
int PrefsRenderFarmReplaceNode::handle_event()
{
	if(subwindow->hot_node >= 0)
	{
		pwindow->thread->preferences->edit_node(subwindow->hot_node, 
			subwindow->edit_node->get_text(),
			pwindow->thread->preferences->renderfarm_port,
			pwindow->thread->preferences->renderfarm_enabled.values[subwindow->hot_node]);
		subwindow->generate_node_list();
		subwindow->update_node_list();
	}
	return 1;
}





PrefsRenderFarmDelNode::PrefsRenderFarmDelNode(PreferencesWindow *pwindow, PerformancePrefs *subwindow, int x, int y)
 : BC_GenericButton(x, y, "Delete Node")
{
	this->pwindow = pwindow;
	this->subwindow = subwindow;
}
PrefsRenderFarmDelNode::~PrefsRenderFarmDelNode()
{
}
int PrefsRenderFarmDelNode::handle_event()
{
	if(strlen(subwindow->edit_node->get_text()))
	{
		pwindow->thread->preferences->delete_node(subwindow->hot_node);
		subwindow->generate_node_list();
		subwindow->update_node_list();
		subwindow->hot_node = -1;
	}
	return 1;
}





PrefsRenderFarmSortNodes::PrefsRenderFarmSortNodes(PreferencesWindow *pwindow, 
	PerformancePrefs *subwindow, 
	int x, 
	int y)
 : BC_GenericButton(x, y, "Sort nodes")
{
	this->pwindow = pwindow;
	this->subwindow = subwindow;
}

PrefsRenderFarmSortNodes::~PrefsRenderFarmSortNodes()
{
}

int PrefsRenderFarmSortNodes::handle_event()
{
	pwindow->thread->preferences->sort_nodes();
	subwindow->generate_node_list();
	subwindow->update_node_list();
	subwindow->hot_node = -1;
	return 1;
}







PrefsRenderFarmJobs::PrefsRenderFarmJobs(PreferencesWindow *pwindow, 
		PerformancePrefs *subwindow, 
		int x, 
		int y)
 : BC_TumbleTextBox(subwindow, 
 	(long)pwindow->thread->preferences->renderfarm_job_count,
	(long)1, 
	(long)100,
	x,
	y,
	100)
{
	this->pwindow = pwindow;
}
PrefsRenderFarmJobs::~PrefsRenderFarmJobs()
{
}
int PrefsRenderFarmJobs::handle_event()
{
	pwindow->thread->preferences->renderfarm_job_count = atol(get_text());
	return 1;
}



PrefsRenderFarmMountpoint::PrefsRenderFarmMountpoint(PreferencesWindow *pwindow, 
		PerformancePrefs *subwindow, 
		int x, 
		int y)
 : BC_TextBox(x, 
 	y, 
	100,
	1,
	pwindow->thread->preferences->renderfarm_mountpoint)
{
	this->pwindow = pwindow;
	this->subwindow = subwindow;
}
PrefsRenderFarmMountpoint::~PrefsRenderFarmMountpoint()
{
}
int PrefsRenderFarmMountpoint::handle_event()
{
	strcpy(pwindow->thread->preferences->renderfarm_mountpoint, get_text());
	return 1;
}

